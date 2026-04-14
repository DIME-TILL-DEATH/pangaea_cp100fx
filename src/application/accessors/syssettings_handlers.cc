#include "syssettings_handlers.h"
#include "console_helpers.h"

#include "adc.h"

#include "eeprom.h"
#include "system.h"
#include "modules.h"

#include "display_task.h"
#include "ui_task.h"
#include "sharc_task.h"

static void cab_mode_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::CAB_SIM_DISABLED], args, count);
	SharcTask->setParameter(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[System::CAB_SIM_DISABLED], 0);
	EEPROM_DelayedSaveSystemData();
}

static void cab_num_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::CAB_SIM_CONFIG], args, count);
	SharcTask->setParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[System::CAB_SIM_CONFIG], 0);
	EEPROM_DelayedSaveSystemData();
	//Reset?????
}

static void midi_ch_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::MIDI_CHANNEL], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void expr_on_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, args, count);

	if(!val)
	{
		ADC_SetState(0);
		sys_para[System::EXPR_TYPE] &= 0x7f;
		SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, 127);
	}
	else
	{
		sys_para[System::EXPR_TYPE] |= 0x80;
		ADC_SetState(1);
	}

	EEPROM_DelayedSaveSystemData();
}

static void expr_type_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, args, count);

	sys_para[System::EXPR_TYPE] = (val & 0x7F) | (sys_para[System::EXPR_TYPE] & 0x80);

	if((sys_para[System::EXPR_TYPE] & 0x7f) > 2)
		SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, 127);

	EEPROM_DelayedSaveSystemData();
}

static void expr_cc_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::EXPR_CC_NUM], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void expr_slev_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::EXPR_STORE_LEVEL], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void spdif_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::SPDIF_OUT_TYPE], args, count);
	SharcTask->setParameter(DSP_ADDRESS_SPDIF, sys_para[System::SPDIF_OUT_TYPE], 0);
	EEPROM_DelayedSaveSystemData();
}

static void tempo_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TAP_TYPE], args, count);
	SharcTask->setParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[System::TAP_TYPE], sys_para[System::GLOBAL_TAP_TIME]);
	EEPROM_DelayedSaveSystemData();
}

static void time_format_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TIME_FORMAT], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void swap_conf_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::SWAP_SWITCH], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void tuner_speed_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::TUNER_SPEED], args, count);
	System::tun_del_val = (127-sys_para[System::TUNER_SPEED])*(90.0f/127.0f)+10.0f;
	EEPROM_DelayedSaveSystemData();
}

static void tuner_ctrl_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, args, count);

	if(!val)
	{
		if(sys_para[System::TUNER_EXTERNAL] & 0x80) sys_para[System::TUNER_EXTERNAL] &= ~0x80;
	}
	else
	{
		if(!(sys_para[System::TUNER_EXTERNAL] & 0x80)) sys_para[System::TUNER_EXTERNAL] |= 0x80;
	}
	EEPROM_DelayedSaveSystemData();
}

static void tuner_cc_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t val = 0;
	default_param_handler(&val, args, count);

	sys_para[System::TUNER_EXTERNAL] = (val & 0x7F) | (sys_para[System::TUNER_EXTERNAL] & 0x80);
	EEPROM_DelayedSaveSystemData();
}

static void fsw_speed_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[System::FSW_SPEED], args, count);
	EEPROM_DelayedSaveSystemData();
}

static void midi_map_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	if(count>2)
	{
		char *end;
		uint8_t mapPos = kgp_sdk_libc::strtol(args[1], &end, 16);
		uint8_t mapVal = kgp_sdk_libc::strtol(args[2], &end, 16);

		sys_para[System::MIDI_MAP_START + mapPos] = mapVal;
		EEPROM_DelayedSaveSystemData();

		msg_console("%s\r%d\r%d\n", args[0], mapPos, mapVal);
	}
}

void set_syssettings_handlers(TTranslator *translator)
{
	translator->AddCommandHandler("sys_cab_mode", cab_mode_command_handler);
	translator->AddCommandHandler("sys_cab_num", cab_num_command_handler);
	translator->AddCommandHandler("sys_midi_ch", midi_ch_command_handler);
	translator->AddCommandHandler("sys_spdif", spdif_command_handler);
	translator->AddCommandHandler("sys_time_format", time_format_command_handler);
	translator->AddCommandHandler("sys_swap_conf", swap_conf_command_handler);
	translator->AddCommandHandler("sys_tempo", tempo_command_handler);
	translator->AddCommandHandler("sys_fsw_speed", fsw_speed_command_handler);

	translator->AddCommandHandler("sys_expr_on", expr_on_command_handler);
	translator->AddCommandHandler("sys_expr_type", expr_type_command_handler);
	translator->AddCommandHandler("sys_expr_cc", expr_cc_command_handler);
	translator->AddCommandHandler("sys_expr_slev", expr_slev_command_handler);

	translator->AddCommandHandler("sys_tuner_ctrl", tuner_ctrl_command_handler);
	translator->AddCommandHandler("sys_tuner_cc", tuner_cc_command_handler);
	translator->AddCommandHandler("sys_tuner_speed", tuner_speed_command_handler);

	translator->AddCommandHandler("midi_map", midi_map_command_handler);
}
