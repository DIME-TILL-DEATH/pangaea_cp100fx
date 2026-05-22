#include "syssettings_handlers.h"

#include "adc.h"
#include "gpio.h"

#include "system.h"
#include "controller.h"
#include "modules.h"
#include "eeprom.h"

#include "console_helpers.h"

#include "display_task.h"
#include "ui_task.h"
#include "sharc_task.h"

static TParamDescriptor cabModeParamDesc = {
	.ptr = &sys_para[System::CAB_SIM_DISABLED],
	.handlerStr = "sys_cab_mode",
	.dspAddress = DSP_ADDRESS_CAB_DRY_MUTE,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Mode",
	.setterHandler = cab_mode_setter
};

static TParamDescriptor cabNumParamDesc = {
	.ptr = &sys_para[System::CAB_SIM_CONFIG],
	.handlerStr = "sys_cab_num",
	.dspAddress = DSP_ADDRESS_CAB_CONFIG,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Cab num",
	.setterHandler = cab_num_setter
};

static TParamDescriptor midiChParamDesc = {
	.ptr = &sys_para[System::MIDI_CHANNEL],
	.handlerStr = "sys_midi_ch",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "MIDI Ch",
	.setterHandler = midi_ch_setter
};

static TParamDescriptor exprOnParamDesc = {
	.ptr = &sys_para[System::EXPR_TYPE],
	.handlerStr = "sys_expr_on",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Expression",
	.setterHandler = expr_on_setter
};

static TParamDescriptor exprTypeParamDesc = {
	.ptr = &sys_para[System::EXPR_TYPE],
	.handlerStr = "sys_expr_type",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Type",
	.setterHandler = expr_type_setter
};

static TParamDescriptor exprCcParamDesc = {
	.ptr = &sys_para[System::EXPR_CC_NUM],
	.handlerStr = "sys_expr_cc",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "CC#",
	.setterHandler = expr_cc_setter
};

static TParamDescriptor exprSlevParamDesc = {
	.ptr = &sys_para[System::EXPR_STORE_LEVEL],
	.handlerStr = "sys_expr_slev",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Store Lev",
	.setterHandler = expr_slev_setter
};

static TParamDescriptor exprCalibrateParamDesc = {
	.ptr = nullptr,
	.handlerStr = "",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Calibrate",
	.setterHandler = nullptr
};

static TParamDescriptor spdifParamDesc = {
	.ptr = &sys_para[System::SPDIF_OUT_TYPE],
	.handlerStr = "sys_spdif",
	.dspAddress = DSP_ADDRESS_SPDIF,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "S/PDIF",
	.setterHandler = spdif_setter
};

static TParamDescriptor tempoTypeParamDesc = {
	.ptr = &sys_para[System::TAP_TYPE],
	.handlerStr = "sys_tempo",
	.dspAddress = DSP_ADDRESS_GLOBAL_TEMPO,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Tempo",
	.setterHandler = tempo_setter
};

static TParamDescriptor timeFormatParamDesc = {
	.ptr = &sys_para[System::TIME_FORMAT],
	.handlerStr = "sys_time_format",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Time",
	.setterHandler = time_format_setter
};

static TParamDescriptor swapConfParamDesc = {
	.ptr = &sys_para[System::SWAP_SWITCH],
	.handlerStr = "sys_swap_conf",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Swap UpConf",
	.setterHandler = swap_conf_setter
};

static TParamDescriptor tunerSpeedParamDesc = {
	.ptr = &sys_para[System::TUNER_SPEED],
	.handlerStr = "sys_tuner_speed",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Speed tun",
	.setterHandler = tuner_speed_setter
};

static TParamDescriptor tunerCtrlParamDesc = {
	.ptr = &sys_para[System::TUNER_EXTERNAL],
	.handlerStr = "sys_tuner_ctrl",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Tuner Contr",
	.setterHandler = tuner_ctrl_setter
};

static TParamDescriptor tunerCcParamDesc = {
	.ptr = &sys_para[System::TUNER_EXTERNAL],
	.handlerStr = "sys_tuner_cc",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "    CC#",
	.setterHandler = tuner_cc_setter
};

static TParamDescriptor fswSpeedParamDesc = {
	.ptr = &sys_para[System::FSW_SPEED],
	.handlerStr = "sys_fsw_speed",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "Speed FS",
	.setterHandler = fsw_speed_setter
};

TSysSettingsDesc SysSettingsDesc = {
	.cabMode = cabModeParamDesc,
	.cabNum = cabNumParamDesc,
	.midiChannel = midiChParamDesc,
	.exprOn = exprOnParamDesc,
	.exprType = exprTypeParamDesc,
	.exprCc = exprCcParamDesc,
	.exprLev = exprSlevParamDesc,
	.exprCalibrate = exprCalibrateParamDesc,
	.spdif = spdifParamDesc,
	.tempo = tempoTypeParamDesc,
	.timeFormat = timeFormatParamDesc,
	.swapConfig = swapConfParamDesc,
	.tunerSpeed = tunerSpeedParamDesc,
	.tunerCtrl = tunerCtrlParamDesc,
	.tunerCc = tunerCcParamDesc,
	.fswSpeed = fswSpeedParamDesc
};

void cab_mode_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)cabModeParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", cabModeParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(cabModeParamDesc.dspAddress, *param_ptr, 0);
	EEPROM_DelayedSaveSystemData();
}

void cab_num_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)cabNumParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", cabNumParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(cabNumParamDesc.dspAddress, *param_ptr, 0);
	EEPROM_DelayedSaveSystemData();
}

void midi_ch_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)midiChParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", midiChParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void expr_on_setter(uint32_t value)
{
	if(!(value & 0x80))
	{
		ADC_SetState(0);
		sys_para[System::EXPR_TYPE] &= 0x7f;
		SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, 127, 0);
	}
	else
	{
		sys_para[System::EXPR_TYPE] |= 0x80;
		ADC_SetState(1);
	}

	console_printf("%s\r%02x\n", exprOnParamDesc.handlerStr, sys_para[System::EXPR_TYPE]);
	EEPROM_DelayedSaveSystemData();
}

void expr_type_setter(uint32_t value)
{
	uint8_t val = value & 0x7F;

	sys_para[System::EXPR_TYPE] = val | (sys_para[System::EXPR_TYPE] & 0x80);

	HW_AdcPinInit();

	if((sys_para[System::EXPR_TYPE] & 0x7f) > EXPR_TYPE_VOL_ALT)
		SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, 127, 0);

	console_printf("%s\r%02x\n", exprTypeParamDesc.handlerStr, sys_para[System::EXPR_TYPE]);
	EEPROM_DelayedSaveSystemData();
}

void expr_cc_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)exprCcParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", exprCcParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void expr_slev_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)exprSlevParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", exprSlevParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void spdif_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)spdifParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", spdifParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(spdifParamDesc.dspAddress, *param_ptr, 0);
	EEPROM_DelayedSaveSystemData();
}

void tempo_setter(uint32_t value)
{
	sys_para[System::GLOBAL_TAP_TIME] = 0;

	uint8_t* param_ptr = (uint8_t*)tempoTypeParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", tempoTypeParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(tempoTypeParamDesc.dspAddress, *param_ptr, sys_para[System::GLOBAL_TAP_TIME]);
	EEPROM_DelayedSaveSystemData();
}

void time_format_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)timeFormatParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", timeFormatParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void swap_conf_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)swapConfParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", swapConfParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void tuner_speed_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)tunerSpeedParamDesc.ptr;
	*param_ptr = value;
	System::tun_del_val = (127 - *param_ptr) * (90.0f / 127.0f) + 10.0f;
	console_printf("%s\r%02x\n", tunerSpeedParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void tuner_ctrl_setter(uint32_t value)
{
	if(!(value & 0x80))
	{
		sys_para[System::TUNER_EXTERNAL] &= 0x7F;
	}
	else
	{
		sys_para[System::TUNER_EXTERNAL] |= 0x80;
	}

	console_printf("%s\r%02x\n", tunerCtrlParamDesc.handlerStr, sys_para[System::TUNER_EXTERNAL]);
	EEPROM_DelayedSaveSystemData();
}

void tuner_cc_setter(uint32_t value)
{
	uint8_t val = value & 0x7F;

	sys_para[System::TUNER_EXTERNAL] = val | (sys_para[System::TUNER_EXTERNAL] & 0x80);

	console_printf("%s\r%02x\n", tunerCcParamDesc.handlerStr, sys_para[System::TUNER_EXTERNAL]);
	EEPROM_DelayedSaveSystemData();
}

void fsw_speed_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)fswSpeedParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", fswSpeedParamDesc.handlerStr, *param_ptr);
	EEPROM_DelayedSaveSystemData();
}

void midi_map_handler(uint32_t mapPos, uint32_t mapVal)
{
	sys_para[System::MIDI_MAP_START + mapPos] = mapVal;
	console_printf("midi_map\r%d\r%d\n", mapPos, mapVal);
	EEPROM_DelayedSaveSystemData();
}

void set_syssettings_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(cabModeParamDesc.handlerStr, cab_mode_setter);
	translator->AddAccessorHandler(cabNumParamDesc.handlerStr, cab_num_setter);
	translator->AddAccessorHandler(midiChParamDesc.handlerStr, midi_ch_setter);
	translator->AddAccessorHandler(spdifParamDesc.handlerStr, spdif_setter);
	translator->AddAccessorHandler(timeFormatParamDesc.handlerStr, time_format_setter);
	translator->AddAccessorHandler(swapConfParamDesc.handlerStr, swap_conf_setter);
	translator->AddAccessorHandler(tempoTypeParamDesc.handlerStr, tempo_setter);
	translator->AddAccessorHandler(fswSpeedParamDesc.handlerStr, fsw_speed_setter);

	translator->AddAccessorHandler(exprOnParamDesc.handlerStr, expr_on_setter);
	translator->AddAccessorHandler(exprTypeParamDesc.handlerStr, expr_type_setter);
	translator->AddAccessorHandler(exprCcParamDesc.handlerStr, expr_cc_setter);
	translator->AddAccessorHandler(exprSlevParamDesc.handlerStr, expr_slev_setter);

	translator->AddAccessorHandler(tunerCtrlParamDesc.handlerStr, tuner_ctrl_setter);
	translator->AddAccessorHandler(tunerCcParamDesc.handlerStr, tuner_cc_setter);
	translator->AddAccessorHandler(tunerSpeedParamDesc.handlerStr, tuner_speed_setter);
}
