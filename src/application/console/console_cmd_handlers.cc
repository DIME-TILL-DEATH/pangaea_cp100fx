#include "console_handlers.h"

#include "eepr.h"
#include "ff.h"

#include "amt.h"

#include "BF706_send.h"
#include "init.h"

#include "controllers.h"
#include "gui_task.h"

#include "system.h"

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
	currentPreset.modules.rawData[147] = delay_time;
	currentPreset.modules.rawData[148] = delay_time>>8;

	currentPresetNumber;
	EEPR_writePreset(currentPresetNumber);

	DSP_SendPresetData(currentPreset.modules.rawData);

	DSP_SendPrimaryCabData(cab1.data, currentPresetNumber+1);
	if(cab_type == CAB_CONFIG_STEREO) DSP_SendSecondaryCabData(cab2.data, currentPresetNumber+1);

	Preset::Change();

	msg_console("%s\r\n", args[0]);
}

static void pchange_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	if(count > 1)
	{
		char *end;
		currentPresetNumber = kgp_sdk_libc::strtol(args[1], &end, 16);

		sys_para[System::LAST_PRESET_NUM] = currentPresetNumber;
		Preset::Change();

		msg_console("%s\r\n", args[0]);
	}
	else
	{
		msg_console("%s\rINCORRECT_ARGUMENT\n", args[0]);
	}
}

static void plist_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
//	msg_console("plist");

	msg_console("%s\r", args[0]);
	for (int p = 0; p < 99; p++)
	{
		Preset::TPresetBrief presetData;
		EEPROM_loadBriefPreset(p, &presetData);

		char *cab1NameSrc = presetData.cab1Name + 1;
		char *cab2NameSrc = presetData.cab2Name + 1;
		char cab1NameDst[64];
		char cab2NameDst[64];

		uint8_t pos=0;

		while(*cab1NameSrc)
		{
			if(*cab1NameSrc != '|' && *cab1NameSrc != '\r' && *cab1NameSrc != '\n')
			{
				cab1NameDst[pos] = *cab1NameSrc;
				pos++;
			}
			cab1NameSrc++;
		}

		pos=0;
		while(*cab2NameSrc)
		{
			if(*cab2NameSrc != '|' && *cab2NameSrc != '\r' && *cab2NameSrc != '\n')
			{
				cab2NameDst[pos] = *cab1NameSrc;
				pos++;
			}
			cab2NameSrc++;
		}

		msg_console("\r%d|%s|%s|%s|%s|", p, presetData.name, presetData.comment, cab1NameDst, cab2NameDst);

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

	char hex[3] = {0, 0, 0};
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
	DSP_ContrSendParameter(DSP_ADDRESS_PRESET_VOLUME, currentPreset.modules.paramData.preset_volume, 0);
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
