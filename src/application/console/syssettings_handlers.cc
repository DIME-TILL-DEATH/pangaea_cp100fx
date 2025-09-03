#include "syssettings_handlers.h"

#include "console_helpers.h"
#include "modules.h"
#include "BF706_send.h"

#include "display.h"

#include "eepr.h"
#include "init.h"

#include "system.h"

static void sys_settings_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);

	sys_para[System::MASTER_EQ_FREQ_LO] = (mstEqMidFreq>>8) & 0xF;
	sys_para[System::MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xF;

	for(size_t i = 0; i < 512; i++)
	{
		char hex[3] =
		{0, 0, 0};
		i2hex(sys_para[i], hex);
		msg_console("%s" , hex );
	}
	msg_console("\n");
}

static void attenuator_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::ATTENUATOR], rl, args, count);
	DisplayTask->Pot_Write();
	write_sys();
}

static void master_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MASTER_VOLUME], rl, args, count);
	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, sys_para[System::MASTER_VOLUME], 0);
	write_sys();
}

static void phones_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::PHONES_VOLUME], rl, args, count);
	DisplayTask->Pot_Write();
	write_sys();
}

static void meq_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MASTER_EQ_ON], rl, args, count);
	DSP_GuiSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[System::MASTER_EQ_ON]);
	write_sys();
}

static void meq_low_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MASTER_EQ_LOW], rl, args, count);
	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_LOW]);
	write_sys();
}

static void meq_mid_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MASTER_EQ_MID], rl, args, count);
	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_MID]);
	write_sys();
}

static void meq_high_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MASTER_EQ_HIGH], rl, args, count);
	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_HIGH]);
	write_sys();
}

static void meq_mid_freq_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	if(count > 0)
	{
		if (count == 2)
		{
			char *end;
			mstEqMidFreq = kgp_sdk_libc::strtol(args[1], &end, 16);
		}

		msg_console("%s\n", args[1]);
	}

	sys_para[System::MASTER_EQ_FREQ_LO] = mstEqMidFreq >> 8;
	sys_para[System::MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xFF;

	DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_LO]);
	DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_HI]);
	write_sys();
}

static void cab_mode_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::CAB_SIM_DISABLED], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[System::CAB_SIM_DISABLED], 0);
	write_sys();
}

static void cab_num_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::CAB_SIM_CONFIG], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[System::CAB_SIM_CONFIG], 0);
	write_sys();
	//Reset?????
}

static void midi_ch_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MIDI_CHANNEL], rl, args, count);
	write_sys();
}

static void expr_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, rl, args, count);

	if(!val)
	{
		adc_init(0);
		sys_para[System::EXPR_TYPE] &= 0x7f;
		ext_send(127);
	}
	else
	{
		sys_para[System::EXPR_TYPE] |= 0x80;
		adc_init(1);
	}

	write_sys();
}

static void expr_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, rl, args, count);

	sys_para[System::EXPR_TYPE] = (val & 0x7F) | (sys_para[System::EXPR_TYPE] & 0x80);

	if((sys_para[System::EXPR_TYPE] & 0x7f) > 2)
		ext_send(127);

	write_sys();
}

static void expr_cc_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::EXPR_CCN], rl, args, count);
	write_sys();
}

static void expr_slev_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::EXPR_STORE_LEVEL], rl, args, count);
	write_sys();
}

static void spdif_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::SPDIF_OUT_TYPE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_SPDIF, sys_para[System::SPDIF_OUT_TYPE], 0);
	write_sys();
}

static void tempo_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TAP_TYPE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[System::TAP_TYPE], sys_para[System::TAP_HIGH]);
	write_sys();
}

static void time_format_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TIME_FORMAT], rl, args, count);
	write_sys();
}

static void swap_conf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::SWAP_SWITCH], rl, args, count);
	write_sys();
}

static void tuner_speed_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TUNER_SPEED], rl, args, count);
	tun_del_val = (127-sys_para[System::TUNER_SPEED])*(90.0f/127.0f)+10.0f;
	write_sys();
}

static void tuner_ctrl_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, rl, args, count);

	if(!val)
	{
		if(sys_para[System::TUNER_EXTERNAL] & 0x80) sys_para[System::TUNER_EXTERNAL] &= ~0x80;
	}
	else
	{
		if(!(sys_para[System::TUNER_EXTERNAL] & 0x80)) sys_para[System::TUNER_EXTERNAL] |= 0x80;
	}
	write_sys();
}

static void tuner_cc_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, rl, args, count);

	sys_para[System::TUNER_EXTERNAL] = (val & 0x7F) | (sys_para[System::TUNER_EXTERNAL] & 0x80);
	write_sys();
}

static void fsw_speed_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::FSW_SPEED], rl, args, count);
	write_sys();
}

static void midi_map_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	if(count>2)
	{
		char *end;
		uint8_t mapPos = kgp_sdk_libc::strtol(args[1], &end, 16);
		uint8_t mapVal = kgp_sdk_libc::strtol(args[2], &end, 16);

		sys_para[System::MIDI_MAP_START + mapPos] = mapVal;
		write_sys();

		msg_console("%s\r%d\r%d\n", args[0], mapPos, mapVal);
	}
}

static void fsw_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if (count > 3)
	{
		char *end;
		uint8_t fswNum = kgp_sdk_libc::strtol(args[1], &end, 16);
		std::emb_string command = args[2];
		uint8_t value = kgp_sdk_libc::strtol(args[3], &end, 16);

		msg_console("%d\r%s\r%d", fswNum, args[2], value);
		if(command == "mode")
		{
			sys_para[System::FSW1_MODE + fswNum] = value;
			goto ending;
		}

		if(command == "ptype")
		{
			sys_para[System::FSW1_PRESS_TYPE + fswNum] = value;
			goto ending;
		}

		if(command == "htype")
		{
			sys_para[System::FSW1_HOLD_TYPE + fswNum] = value;
			goto ending;
		}

		if(command == "cpressnum")
		{
			sys_para[System::FSW1_CTRL_PRESS_CC + fswNum] = value;
			goto ending;
		}

		if(command == "choldnum")
		{
			sys_para[System::FSW1_CTRL_HOLD_CC + fswNum] = value;
			goto ending;
		}

		if(command == "ppressnum")
		{
			if(count > 4)
			{
				uint8_t presetNum = kgp_sdk_libc::strtol(args[4], &end, 16);

				sys_para[System::FSW1_PRESS_PR1 + fswNum*4 + value] = presetNum;
				msg_console("\r%d", presetNum);
			}
			else
			{
				msg_console("INCORRECT_ARGS");
			}
			goto ending;
		}

		if(command == "pholdnum")
		{
			if(count > 4)
			{
				uint8_t presetNum = kgp_sdk_libc::strtol(args[4], &end, 16);

				sys_para[System::FSW1_HOLD_PR1 + fswNum*4 + value] = presetNum;
				msg_console("\r%d", presetNum);
			}
			else
			{
				msg_console("INCORRECT_ARGS");
			}
			goto ending;
		}

		msg_console("\rundefined type");
	}
	else
	{
		msg_console("iINCORRECT_ARGS");
	}

ending:
	write_sys();
	msg_console("\n");
}

void set_syssettings_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("sys_settings", sys_settings_command_handler);

	rl->AddCommandHandler("sys_cab_mode", cab_mode_command_handler);
	rl->AddCommandHandler("sys_cab_num", cab_num_command_handler);
	rl->AddCommandHandler("sys_midi_ch", midi_ch_command_handler);
	rl->AddCommandHandler("sys_spdif", spdif_command_handler);
	rl->AddCommandHandler("sys_time_format", time_format_command_handler);
	rl->AddCommandHandler("sys_swap_conf", swap_conf_command_handler);
	rl->AddCommandHandler("sys_tempo", tempo_command_handler);
	rl->AddCommandHandler("sys_fsw_speed", fsw_speed_command_handler);

	rl->AddCommandHandler("sys_expr_on", expr_on_command_handler);
	rl->AddCommandHandler("sys_expr_type", expr_type_command_handler);
	rl->AddCommandHandler("sys_expr_cc", expr_cc_command_handler);
	rl->AddCommandHandler("sys_expr_slev", expr_slev_command_handler);

	rl->AddCommandHandler("sys_tuner_ctrl", tuner_ctrl_command_handler);
	rl->AddCommandHandler("sys_tuner_cc", tuner_cc_command_handler);
	rl->AddCommandHandler("sys_tuner_speed", tuner_speed_command_handler);

	rl->AddCommandHandler("fsw", fsw_command_handler);

	rl->AddCommandHandler("midi_map", midi_map_command_handler);

	rl->AddCommandHandler("vl_at", attenuator_command_handler);
	rl->AddCommandHandler("vl_ms", master_volume_command_handler);
	rl->AddCommandHandler("vl_ph", phones_volume_command_handler);

	rl->AddCommandHandler("meq_on", meq_on_command_handler);
	rl->AddCommandHandler("meq_lg", meq_low_command_handler);
	rl->AddCommandHandler("meq_mg", meq_mid_command_handler);
	rl->AddCommandHandler("meq_hg", meq_high_command_handler);
	rl->AddCommandHandler("meq_mf", meq_mid_freq_command_handler);
}
