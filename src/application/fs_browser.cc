#include "fs_browser.h"

#include "fs.h"
#include "cs.h"

#include "BF706_send.h"
#include "eepr.h"
#include "init.h"
#include "storage.h"
#include "AT45DB321.h"
#include "allFonts.h"


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
		char buf[512];
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

uint8_t TFsBrowser::impulseDirExist = 0;

TFsBrowser::TFsBrowser()
{
	// инициализация флешки и ее контроллера
	//AT45DB321_Init();
	sd_initialized = true;
}

TFsBrowser::~TFsBrowser()
{
}

emb_string TFsBrowser::CurrDir(bool collapsedPath) const
{
	if(collapsedPath)
		return curr_dir_level+curr_dir_name;
	else
	{
	#if _USE_LFN
		FILINFO fno;
		char *fn; /* This function is assuming non-Unicode cfg. */

	    static char lfn[_MAX_LFN + 1];
	    lfn[0] = 0 ;
	    fno.lfname = lfn;
	    fno.lfsize = sizeof lfn;

		f_getcwd(lfn, _MAX_LFN);
		return lfn;
	#else
		return 0;
	#endif
	}
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

	impulseDirExist = 1;
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
bool TFsBrowser::GetDataFromFile(uint8_t *buff, emb_string &err_msg)
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
	char header[5] = {0, 0, 0, 0, 0};
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
		err_msg = "unsupported format  (no MONO channel sets)";
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

	if(cab_type != CAB_CONFIG_STEREO)
	{
		if(file_size > 8192 * 3)
			file_size = 8192 * 3;
	}
	else
	{
		if(file_size > 4096 * 3)
			file_size = 4096 * 3;
	}

	// read cabinet data
	kgp_sdk_libc::memset(buff, 0, 8192 * 3);
	fs_res = f_lseek(&f, 44);
	while(1)
	{
		if(file_size > 512)
		{
			f_read(&f, (void*)buff, 512, &br);
			buff += 512;
			file_size -= 512;
		}
		else
		{
			f_read(&f, (void*)buff, file_size, &br);
			break;
		}
	}

	fs_res = f_close(&f);
	err_msg = "read cab data OK";

	return true;
}
//----------------------------------------------------------------
void TFsBrowser::LoadCab(fs_object_t &object)
{
	TCSTask::TResponse response;

	if(object.type == fotFile)
	{
		object = *curr_fs_object;
		response.file.type = object.type;

		emb_string err;
		if(GetDataFromFile(presetBuffer, err))
		{
			response.responseType = TCSTask::rpFileLoaded;
			response.file.buffer = &presetBuffer[0];

			char nameBuffer[64];
			uint8_t name_point = 0;

			while(object.name[name_point] && (name_point<62))
				nameBuffer[name_point] = object.name[name_point++]; //name_buf_temp[name_point+1] = object.name[name_point++];

			nameBuffer[++name_point] = 0;
			nameBuffer[0] = name_point;

			kgp_sdk_libc::memcpy(response.file.name, nameBuffer, 64);

			extern bool cab_data_ready;
			cab_data_ready = true;
		}
		else
		{
			response.responseType = TCSTask::rpFileInvalid;
		}
	}
	else
	{
		response.responseType = TCSTask::rpDirSelected;
	}
	CSTask->SendResponse(response);

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
			LoadCab(object);
		break;
		case bcDown:
			NextObject(object);
			LoadCab(object);
		break;
		case bcLoadImp:
			LoadCab(object);
		break;
		case bcAction:
			if(curr_fs_object->type == fotDir)
			{
				// up to /IMPULSE imposible... oops...
				if((curr_fs_object->dir=="0:IMPULSE") && (curr_fs_object->name==".."))
				{
					object.name = "..";
					object.type = fotDir;
				}
				else
				{
					SelectDir(*curr_fs_object);
					object = *curr_fs_object;
				}

				TCSTask::TResponse response;
				response.responseType = TCSTask::rpDirSelected;
				CSTask->SendResponse(response);
			}

			if(curr_fs_object->type==fotFile)
			{
				emb_string tmp = global_path;
				tmp.resize(_MAX_LFN);
				kgp_sdk_libc::memcpy(Preset::impulsePath, tmp.c_str(), _MAX_LFN);

				tmp = FSTask->Object().name;
				tmp.resize(_MAX_LFN);
				kgp_sdk_libc::memcpy(Preset::impulsePath+256, tmp.c_str(), _MAX_LFN);

				TCSTask::TResponse response;
				response.responseType = TCSTask::rpFileSelected;
				CSTask->SendResponse(response);
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
void TFsBrowser::SelectFile(const fs_object_t &fs_object)
{
	emb_string fileName = fs_object.name;
	curr_fs_object = std::find_if(fs_object_list.begin(), fs_object_list.end(),
			[&fileName](const fs_object_t& obj)
				{return fileName == obj.name;});

//	*curr_fs_object = fs_object;
//	out_file_strings(parent.name.c_str());
}

bool TFsBrowser::CreateFile(const fs_object_t &object)
{
	FIL irFile;
	FRESULT res = f_open(&irFile, object.name.c_str(), FA_WRITE | FA_OPEN_ALWAYS);

	if (res == FR_OK)
	{
		fs_created_file_object = object;
		f_close(&irFile);
		UpdateDirList();
		return true;
	}
	else return false;
}

bool TFsBrowser::AppendDataToFile(char* buffer, uint16_t dataSize)
{
	FIL irFile;
	FRESULT res = f_open(&irFile, fs_created_file_object.name.c_str(), FA_WRITE | FA_OPEN_EXISTING);
	if (res == FR_OK)
	{
		f_lseek(&irFile, f_size(&irFile));
		f_write(&irFile, buffer, dataSize, 0);
		f_close(&irFile); // can write to file. Path correct
		return true;
	}
	return false;
}

void TFsBrowser::RemoveObject(const fs_object_t &object)
{
	FRESULT res;
	FILINFO fno;

	res = f_stat(object.name.c_str(), &fno);

	if(res != FR_OK || fno.fname[0] == 0) return;
	if((fno.fname[0] == '.' && fno.fname[1] == 0) || (fno.fname[1] == '.' && fno.fname[2] == 0)) return;

	if(fno.fattrib & AM_DIR)
	{
		res = DeleteDirectoryRecursive(object.name.c_str());
		if(res == FR_OK)
		{
			f_unlink(object.name.c_str());
		}
	}
	else
	{
		f_unlink(object.name.c_str());
	}

	UpdateDirList();
}

FRESULT TFsBrowser::DeleteDirectoryRecursive(const char* path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path);
    if (res != FR_OK)
    {
        return res;
    }

    while (1)
    {
        res = f_readdir(&dir, &fno);
        if(res != FR_OK || fno.fname[0] == 0) break;

        // Пропускаем . и ..
        if((fno.fname[0] == '.' && fno.fname[1] == 0) || (fno.fname[1] == '.' && fno.fname[2] == 0)) continue;


        std::emb_string fullPath(path);
        fullPath += "/";
        fullPath += fno.fname;

        if(fno.fattrib & AM_DIR)
        {
            res = DeleteDirectoryRecursive(fullPath.c_str());
            if(res == FR_OK)
            {
                res = f_unlink(fullPath.c_str());
            }
        }
        else
        {
            res = f_unlink(fullPath.c_str());
        }

        if (res != FR_OK) break;
    }

    f_closedir(&dir);
    return res;
}

void TFsBrowser::RenameObject(const fs_object_t &srcObject, const fs_object_t &dstObject)
{
	f_rename(srcObject.name.c_str(), dstObject.name.c_str());
	UpdateDirList();
}
//---------------------------------------------------------------
void TFsBrowser::CreateDir(fs_object_t create_object)
{
	f_mkdir(create_object.name.c_str());
	UpdateDirList();
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
		impulseDirExist = 0;
		//rmsg("walk_dir: FatFS error %s,  path=%s\n",f_err2str(res), select_object.name.c_str());
		return;
	}

	const emb_string curr_dir_name_tmp(curr_dir_name);
	f_getcwd(lfn, _MAX_LFN);
	CollapseAbsPath(lfn, curr_dir_name, curr_dir_level);

	UpdateDirList();

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

void TFsBrowser::UpdateDirList()
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

	fs_object_list.clear();

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
	}
}
