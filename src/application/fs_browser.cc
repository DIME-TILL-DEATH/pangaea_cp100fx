/*
 * fs_browser.cc
 *
 *  Created on: 31 марта 2014 г.
 *      Author: klen
 */

#include "fs_browser.h"
#include "fs.h"
#include "BF706_send.h"
#include "eepr.h"
#include "storage.h"
#include "AT45DB321.h"
#include "gui/elements/allFonts.h"

const char *fs_object_type_strings[] =
{"File", "Dir", "Unknown"};
uint8_t name_point = 0;
volatile uint8_t file_fl = 0;
volatile uint8_t action_fl = 0;
volatile uint8_t imp_dir_fl = 0;
extern uint8_t cab_type;
extern uint8_t sd_buf[];
extern uint8_t impulse_path[];

//----------------------------------------------------------------------
char buf[512];
void out_file_strings(const char *file_name)
{
	// открытие файла на томе №0 , путь data/data.txt
	FIL f;
	FRESULT fs_res = f_open(&f, file_name, FA_READ);
	////rmsg( "f_open %s:  %s", file_name , f_err2str(fs_res));

	char *res;
	if(fs_res==FR_OK)
	{
		// чтение строк файла до тех пор пока они не кончатся
		do
		{
			// чтение строки из текстового файла
			res = f_gets(buf, 512, &f);

			// выход из цикла если файл кончился
			if(!res)
				break;

			size_t l = kgp_sdk_libc::strlen(res);

			if(res[l-1]=='\n'||res[l-1]=='\r')
				res[l-1] = 0;

			// вывод на дисплей строки

			////rmsg("\t\"%s%c\n",buf,'\"');

		} while(1);

		// закрытие файла
		fs_res = f_close(&f);
		////rmsg( "f_close %s:  %s", file_name , f_err2str(fs_res));
	}
}
//------------------------------------------------------------------------

TFsBrowser::TFsBrowser()
{
	// инициализация флешки и ее контроллера
	//AT45DB321_Init();
	sd_initialized = true;
}

TFsBrowser::~TFsBrowser()
{
}

void TFsBrowser::PrevObject(fs_object_t &object)
{
	if(curr_fs_object!=fs_object_list.begin())
		curr_fs_object--;

	object = *curr_fs_object;
}
//---------------------------------------------------------------
FRESULT TFsBrowser::FsMount(const emb_string &init_dir)
{
	// монтирование раздела  №0
	fs_res = f_mount(&fs, "0:", 1);

	if(fs_res!=FR_OK)
		return fs_res;

	fs_object_t object;
	object.name = init_dir;
	object.type = fotDir;
	curr_dir_name = "0:IMPULSE";
	curr_dir_level = "*";

	fs_object_list.push_back(object);
	curr_fs_object = fs_object_list.begin();

	imp_dir_fl = 1;
	SelectDir(object);
	PrevObject(object);

	return fs_res;
}
//---------------------------------------------------------------
FRESULT TFsBrowser::FsUmount()
{
	fs_object_list.clear();
	curr_fs_object = fs_object_list.begin();
	return f_mount(0, "0:", 0);
}
//---------------------------------------------------------------
void TFsBrowser::NextObject(fs_object_t &object)
{
	if(curr_fs_object!=fs_object_list.end()-1)
		curr_fs_object++;

	object = *curr_fs_object;
}
//---------------------------------------------------------------
void out_str(const emb_string &string)
{
	////rmsg( "%s\n", string.c_str()) ;
}
//----------------------------------------------------------------
emb_string global_path;
bool TFsBrowser::GetCabConfig(uint8_t *buff, emb_string &err_msg)
{

	FIL f;
	FRESULT fs_res = f_open(&f, curr_fs_object->name.c_str(), FA_READ);
	if(fs_res!=FR_OK)
	{
		err_msg = f_err2str(fs_res);
		////rmsg( "f_open %s:  %s", curr_fs_object->name.c_str() , err_msg.c_str());
		curr_fs_object->full_dir = emb_string("");
		return false;
	}

	f_getcwd((char*)buff, _MAX_LFN-1);
	curr_fs_object->full_dir = emb_string((char*)buff);
	global_path = emb_string((char*)buff);

	f_lseek(&f, 0);
	//  read header and check as RIFF
	char header[5] =
	{0, 0, 0, 0, 0};
	UINT br;
	fs_res = f_read(&f, header, 4, &br);
	if(kgp_sdk_libc::strcmp(header, "RIFF"))
	{
		err_msg = "unsupported format  (no RIFF headr)";
		////rmsg(err_msg.c_str());
		return false;
	}
	// read header and check as MONO
	fs_res = f_lseek(&f, 22);
	uint16_t chanal_count;
	fs_res = f_read(&f, (void*)&chanal_count, 2, &br);
	if(chanal_count!=1)
	{
		err_msg = "unsupported format  (no MONO chanal sets)";
		////rmsg(err_msg.c_str());
		return false;
	}
	// read header and check as sample rate 48000
	fs_res = f_lseek(&f, 24);
	uint32_t sample_rate;
	fs_res = f_read(&f, (void*)&sample_rate, 4, &br);
	if(sample_rate!=48000)
	{
		err_msg = "unsupported format  (no 48000 sample rate sets)";
		//rmsg(err_msg.c_str());
		return false;
	}
//-----------------------------------------------------------------------------------------------
	// read cabinet data size
	fs_res = f_lseek(&f, 40);
	uint32_t file_size;
	fs_res = f_read(&f, (void*)&file_size, 4, &br);
	if(cab_type!=2)
	{
		if(file_size>24576)
			file_size = 24576;
	}
	else
	{
		if(file_size>12288)
			file_size = 12288;
	}

	// read cabinet data
	kgp_sdk_libc::memset(buff, 0, 24576);
	fs_res = f_lseek(&f, 44);
	while(1)
	{
		if(file_size>512)
		{
			f_read(&f, (void*)sd_buf, 512, &br);
			for(uint16_t i = 0; i<512; i++)
				*buff++ = sd_buf[i];
			file_size -= 512;
		}
		else
		{
			f_read(&f, (void*)sd_buf, file_size, &br);
			for(uint16_t i = 0; i<file_size; i++)
				*buff++ = sd_buf[i];
			break;
		}
	}
	// закрытие файла
	fs_res = f_close(&f);
	err_msg = "read cab data OK";

	return true;
}
//----------------------------------------------------------------
void TFsBrowser::Load_cab(fs_object_t &object)
{
	object = *curr_fs_object;
	emb_string err;
	extern bool cab_data_ready;
	name_point = 0;
	//send_codec(0xa381);//Codec mute
	cab_data_ready = GetCabConfig(preset_temp, err);
	if(!cab_num)
		gui_send(16, 1);
	else
		gui_send(17, 1);
	//send_codec(0xa301);
	while(object.name[name_point]&&(name_point<62))
		name_buf_temp[name_point+1] = object.name[name_point++];

	name_buf_temp[++name_point] = 0;
	name_buf_temp[0] = name_point;
}
void TFsBrowser::Browse(const browse_command_t browse_command, fs_object_t &object, fs_object_list_t &object_list)
{
	emb_string tmp;
	switch(browse_command)
	{
		case bcBack:
			if(curr_fs_object->type==fotFile)
			{
				while(curr_fs_object->name!="..")
					PrevObject(object);
				FSTask->SendCommand(TFsBrowser::bcAction);
			}
		break;
		case bcCurrent:
			object = *curr_fs_object;
		break;
		case bcUp:
			PrevObject(object);
			if(curr_fs_object->type==fotFile)
			{
				Load_cab(object);
			}
		break;
		case bcDown:
			NextObject(object);
			if(curr_fs_object->type==fotFile)
			{
				Load_cab(object);
			}
		break;
		case bcLoadImp:
			if(curr_fs_object->type==fotFile)
			{
				Load_cab(object);
			}
		break;
		case bcAction:
			if(curr_fs_object->type==fotDir)
			{
				action_fl = 1;
				// up to /IMPULSE imposible... oops...
				if((curr_fs_object->dir=="0:IMPULSE")&&(curr_fs_object->name==".."))
				{
					object.name = "..";
					object.type = fotDir;
					break;
				}
				SelectDir(*curr_fs_object);
				object = *curr_fs_object;
			}
			if(curr_fs_object->type==fotFile)
			{
				file_fl = 1;
				action_fl = 1;
				emb_string tmp = global_path;
				tmp.resize(_MAX_LFN);
				kgp_sdk_libc::memcpy(impulse_path, tmp.c_str(), _MAX_LFN);
				tmp = FSTask->Object().name;
				tmp.resize(_MAX_LFN);
				kgp_sdk_libc::memcpy(impulse_path+256, tmp.c_str(), _MAX_LFN);
			}
		break;

		case bcStartup:
			*curr_fs_object = object;
			SelectDir(*curr_fs_object);
			object = *curr_fs_object;
		break;
		case bcPwd:
			char pwd[_MAX_LFN+1];
			f_getcwd(pwd, _MAX_LFN);
			object.name = pwd;
			object.type = fotDir;
		break;
		case bcList:
			object_list = fs_object_list;
		break;
		case bcFsMount:
			tmp = "0:IMPULSE";
			FsMount(tmp);
		break;
		case bcFsUmount:
			FsUmount();
		break;
		default:
			//rmsg( "TFsBrowser::Browse UNKNOWN COMMAND \n" ) ;
		break;
	}
}
//---------------------------------------------------------------
void TFsBrowser::Print(outstr_func_t func)
{

}
//---------------------------------------------------------------
void TFsBrowser::SelectFile(const fs_object_t &parent)
{
	out_file_strings(parent.name.c_str());
}
//---------------------------------------------------------------

char *file_name;
size_t slash_count;
void TFsBrowser::CollapseAbsPath(char *abs_path, emb_string &name, emb_string &level)
{
	/*
	 file_name = NULL ;
	 slash_count = 0 ;
	 for (size_t i = 0 ; i < strlen(abs_path) ; i++ )
	 {
	 if (abs_path[i] == '/')
	 {
	 slash_count++ ;
	 file_name = abs_path + i + 1 ;
	 }

	 }
	 level.assign( slash_count, '*') ;
	 name = file_name ;
	 */

	level = abs_path;
	size_t pos = level.find_last_of('/');
	name = level.substr(pos+1);
	slash_count = 0;
	for(auto &symbol : level)
		if(symbol=='/')
			slash_count++;
	level.assign(slash_count, '/');
}
//---------------------------------------------------------------
void TFsBrowser::SelectDir(fs_object_t select_object)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	char *fn; /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    lfn[0] = 0 ;
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
  #endif

	//if ((curr_fs_object->dir == "0:IMPULSE") && (curr_fs_object->name == ".."))

	fs_object_list.clear();

	res = f_chdir(select_object.name.c_str());

	if(res!=FR_OK)
	{
		imp_dir_fl = 0;
		//rmsg("walk_dir: FatFS error %s,  path=%s\n",f_err2str(res), select_object.name.c_str());
		return;
	}

	const emb_string curr_dir_name_tmp(curr_dir_name);
	f_getcwd(lfn, _MAX_LFN);
	CollapseAbsPath(lfn, curr_dir_name, curr_dir_level);

	res = f_opendir(&dir, "."); /* Open the directory */

	if(res==FR_OK)
	{
		for(;;)
		{
			res = f_readdir(&dir, &fno); /* Read a directory item */
			if(res!=FR_OK||fno.fname[0]==0)
				break; /* Break on error or end of dir */
			if((fno.fname[0]=='.'&&fno.fname[1]==0))
				continue; /* Ignore dot entry */
#if _USE_LFN
          fn = *fno.lfname ? fno.lfname : fno.fname;
  #else
			fn = fno.fname;
#endif

			fs_object_t object;
			object.name = fn;
			object.dir = CurrDir();
			//if (object.name==".." && curr_dir_name=="IMPULSE") continue ; // пропуск элемента .. в директори IMPULSE
			object.type = fno.fattrib&AM_DIR ? fotDir : fotFile;
			fs_object_list.push_back(object);
		}

		sort(fs_object_list.begin(), fs_object_list.end(), sort_fs_object());

		//установка текущим объектом первого элемента директории в которую зашли
		curr_fs_object = fs_object_list.begin();
		if(select_object.name=="..")
		{
			//установка текущим объектом директории из которой поднялись
			for(auto i = fs_object_list.begin(); i!=fs_object_list.end(); i++)
			{
				if(i->name==curr_dir_name_tmp)
				{
					curr_fs_object = i;
					break;
				}
			}
		}

		if(select_object.startup.size()) // bsStartup , field have a file name to select to current
		{
			for(auto i = fs_object_list.begin(); i!=fs_object_list.end(); i++)
			{
				if(i->name==select_object.startup)
				{
					curr_fs_object = i;
					break;
				}
			}
			select_object.startup.clear();
		}

	}
	else
	{
		//rmsg("walk_dir: FatFS error %s,  path=%s\n",f_err2str(res), select_object.name.c_str());
	}
}
