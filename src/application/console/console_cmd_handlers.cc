#include "console_handlers.h"

#include "eepr.h"
#include "ff.h"

#include "amt.h"

#include "BF706_send.h"
#include "init.h"

#include "controllers.h"
#include "gui_task.h"

#include "console_helpers.h"
#include "resonance_filter_handlers.h"
#include "gate_handlers.h"
#include "compressor_handlers.h"
#include "preamp_handlers.h"
#include "amp_handlers.h"
#include "ir_handlers.h"
#include "eq_handlers.h"
#include "phaser_handlers.h"
#include "flanger_handlers.h"
#include "chorus_handlers.h"
#include "delay_handlers.h"
#include "early_handlers.h"
#include "reverb_handlers.h"
#include "tremolo_handlers.h"

#include "syssettings_handlers.h"

bool consoleBusy = false;

uint16_t getDataPartFromStream(TReadLine *rl, char *buf, int maxSize)
{
	kgp_sdk_libc::memset(buf, 0, maxSize);
	int streamPos = 0;
	do
	{
		int c;
		rl->RecvChar(c);
		if(c == '\r' || c == '\n')
		{
			return streamPos;
		}
		buf[streamPos++] = c;
	} while(streamPos < maxSize);

	return streamPos;
}

static void amtdev_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	char hex[3] =
	{0, 0, 0};
	i2hex(5, hex);
	msg_console("%s\r%s\n", args[0], hex);
}

static void amtver_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r%s\n", args[0], amt_ver);
}

static void psave_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
//	currentPreset.modules.rawData[147] = delay_time;
//	currentPreset.modules.rawData[148] = delay_time>>8;
//
//	send_cab_data(0, currentPresetNumber+1, 0);
//	if(cab_type == 2)
//		send_cab_data1(0, currentPresetNumber+1);
//
//	preselectedPresetNumber = currentPresetNumber;
//	sys_para[LAST_PRESET_NUM] = currentPresetNumber;
//
//	eepr_write(currentPresetNumber);
//	prog_ch();

	currentPreset.modules.rawData[147] = delay_time;
	currentPreset.modules.rawData[148] = delay_time>>8;

	preselectedPresetNumber = currentPresetNumber;
	eepr_write(preselectedPresetNumber);

	// Not onle cab data, but preset too
	send_cab_data(0, preselectedPresetNumber+1, 0);
	if(cab_type==2)
		send_cab_data1(0, preselectedPresetNumber+1);


	prog_ch();

	msg_console("%s\r\n", args[0]);
}

static void pchange_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	if(count > 1)
	{
		char *end;
		currentPresetNumber = kgp_sdk_libc::strtol(args[1], &end, 16);

		preselectedPresetNumber = currentPresetNumber;

		sys_para[LAST_PRESET_NUM] = currentPresetNumber;
		prog_ch();
	}
	msg_console("%s\r\n", args[0]);
}

//static void current_cabinet_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
//{
//	std::emb_string err_str;
//	if(count == 1)
//	{
//
//		console_out_currnt_cab(err_str, rl);
//		return;
//	}
//	if(count == 3)
//	{
//		if(args[2][0] == '0' && strlen(args[2]) == 1)
//		{
//			delete_current_cab(err_str, rl);
//			FATFS fs;
//			FRESULT res;
//			FIL f;
//			UINT f_size;
//			res = f_mount(&fs, "0:", 1);
//			if(res != FR_OK)
//			{
//				msg_console("error: 'f_mount' result '%s'\n", f_err2str(res) );
//				return;
//			}
//			emb_string file_name;
//			file_name = "0:/Bank_";
//			file_name += (size_t)bank_pres[0];
//			file_name += "/Preset_";
//			file_name += (size_t)bank_pres[1];
//			file_name += "/";
//			file_name += args[1];
//
//			FRESULT fs_res = f_open(&f, file_name.c_str(), FA_CREATE_ALWAYS | FA_WRITE);
//
//			if(fs_res == FR_OK)
//			{
//				size_t chunk = 2;
//				char hex[3] =
//				{0, 0, 0};
//				i2hex(chunk, hex);
//				msg_console("RX %s\n",hex);
//				do
//				{
//					int c;
//					char w;
//					for(size_t i = 0; i < chunk; i++)
//					{
//						rl->RecvChar(c);
//						if(c == '\r')
//						{
//							f_close(&f);
//							f_mount(0, "0:", 0);
//							msg_console("END\n");
//							return;
//						}
//						if(c > 57)
//							c -= 39;
//						w = (c - '0') << 4;
//						rl->RecvChar(c);
//						if(c == '\r')
//						{
//							f_close(&f);
//							f_mount(0, "0:", 0);
//							msg_console("SYNC ERROR\n");
//							return;
//						}
//						if(c > 57)
//							c -= 39;
//						w |= c - '0';
//						UINT bw;
//						FRESULT fs_res = f_write(&f, &w, 1, &bw);
//						if(bw == 0 || fs_res != FR_OK)
//						{
//							f_close(&f);
//							f_unlink(file_name.c_str());
//							msg_console("file write operation faild\n" );
//							f_mount(0, "0:", 0);
//							return;
//						}
//					}
//					msg_console("NX\n");
//				} while(1);
//			}
//			else
//			{
//				msg_console("error: file %s not created, 'f_open' result '%s'\n", args[1] , f_err2str(res));
//				f_mount(0, "0:", 0);
//				return;
//			}
//		}
//		else
//		{
//			// write to memory
//			//--------------------
//			return;
//		}
//	}
//	msg_console("invalid args count\n" );
//}

static void plist_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
//	msg_console("plist");

	msg_console("%s\r", args[0]);
	for (int p = 0; p < 99; p++)
	{
		Preset::TPresetBrief presetData;
		EEPROM_loadBriefPreset(p, &presetData);
		msg_console("\r%d|%s|%s|%s|%s|", p, presetData.name, presetData.comment, presetData.cab1Name, presetData.cab2Name);

		uint8_t enabled[14];
		kgp_sdk_libc::memcpy(enabled, &presetData.switches, 14);
		for(uint8_t i=0; i<14; i++) msg_console("%d", enabled[i]);
	}
	msg_console("\n");
}

static void state_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	for(size_t i = 0; i < 512; i++)
	{
		char hex[3] =
		{0, 0, 0};
		i2hex(currentPreset.modules.rawData[i], hex);
		msg_console("%s", hex);
	}
	msg_console("\n");
}

static void cntrls_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	for(size_t i = 0; i < controllersCount; i++)
	{
		char hex[3] =
		{0, 0, 0};

		i2hex(currentPreset.controller[i].src, hex);
		msg_console("%s", hex);
		i2hex(currentPreset.controller[i].dst, hex);
		msg_console("%s", hex);
		i2hex(currentPreset.controller[i].minVal, hex);
		msg_console("%s", hex);
		i2hex(currentPreset.controller[i].maxVal, hex);
		msg_console("%s", hex);
	}
	msg_console("\n");
}

static void pnum_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);

	char hex[3] =
	{0, 0, 0};
	i2hex(currentPresetNumber, hex);
	msg_console("%s", hex);

	msg_console("\n");
}

static void pname_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if(count > 1)
	{
		std::emb_string command = args[1];

		if(command == "set")
		{
			consoleBusy = true;
			getDataPartFromStream(rl, (char*)currentPreset.name, 15);
			consoleBusy = false;
		}
	}
	msg_console("%s\n", currentPreset.name);
}

static void pcomment_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if(count > 1)
	{
		std::emb_string command = args[1];

		if(command == "set")
		{
			consoleBusy = true;
			getDataPartFromStream(rl, (char*)currentPreset.comment, 15);
			consoleBusy = false;
		}
	}
	msg_console("%s\n", currentPreset.comment);
}

static void controller_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if (count > 3)
	{
		char *end;
		uint8_t cntrlNum = kgp_sdk_libc::strtol(args[1], &end, 16);
		std::emb_string command = args[2];
		uint8_t value = kgp_sdk_libc::strtol(args[3], &end, 16);

		msg_console("%d\r%s\r%d", cntrlNum, args[2], value);

		if(command == "dst")
		{
			currentPreset.controller[cntrlNum].dst = value;
			goto ending;
		}

		if(command == "src")
		{
			currentPreset.controller[cntrlNum].src = value;
			goto ending;
		}

		if(command == "min")
		{
			currentPreset.controller[cntrlNum].minVal = value;
			goto ending;
		}

		if(command == "max")
		{
			currentPreset.controller[cntrlNum].maxVal = value;
			goto ending;
		}


		msg_console("\rundefined type");
	}
	else
	{
		msg_console("incorrect args");
	}

ending:
	write_sys();
	msg_console("\n");
}

static void controller_pc_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.pcOut, rl, args, count);
}

static void controller_set_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val;
	default_param_handler(&val, rl, args, count);

	currentPreset.set = val | 0x80;
}

static void preset_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.paramData.preset_volume, rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PRESET_VOLUME, currentPreset.modules.paramData.preset_volume, 0);
}

static void preset_volume_control_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.paramData.volume_control, rl, args, count);
}
//------------------------------------------------------------------------------
void ConsoleSetCmdHandlers(TReadLine *rl)
{
	SetConsoleCmdDefaultHandlers(rl);
	/*
	 rl->AddCommandHandler("cc", current_cabinet_command_handler);
	 rl->AddCommandHandler("ce", cabinet_enable_command_handler);
	 */

	rl->AddCommandHandler("amtdev", amtdev_command_handler);
	rl->AddCommandHandler("amtver", amtver_command_handler);

	rl->AddCommandHandler("pchange", pchange_command_handler);
	rl->AddCommandHandler("psave", psave_command_handler);

	rl->AddCommandHandler("state", state_command_handler);
	rl->AddCommandHandler("cntrls", cntrls_command_handler);

	rl->AddCommandHandler("plist", plist_command_handler);

	rl->AddCommandHandler("pnum", pnum_command_handler);
	rl->AddCommandHandler("pname", pname_command_handler);
	rl->AddCommandHandler("pcomment", pcomment_command_handler);

	rl->AddCommandHandler("cntrl", controller_command_handler);
	rl->AddCommandHandler("cntrl_pc", controller_pc_command_handler);
	rl->AddCommandHandler("cntrl_set", controller_set_command_handler);

	rl->AddCommandHandler("vl_pr", preset_volume_command_handler);
	rl->AddCommandHandler("vl_pr_cntrl", preset_volume_control_command_handler);

	set_syssettings_handlers(rl);

	set_resonance_filter_handlers(rl);
	set_gate_handlers(rl);
	set_compressor_handlers(rl);
	set_preamp_handlers(rl);
	set_amp_handlers(rl);
	set_ir_handlers(rl);
	set_eq_handlers(rl);
	set_phaser_handlers(rl);
	set_flanger_handlers(rl);
	set_chorus_handlers(rl);
	set_delay_handlers(rl);
	set_early_handlers(rl);
	set_reverb_handlers(rl);
	set_tremolo_handlers(rl);
}

//------------------------------------------------------------------------------
