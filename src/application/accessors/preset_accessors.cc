#include "preset_accessors.h"

#include "system.h"
#include "controller.h"
#include "modules.h"
#include "eeprom.h"

#include "console_helpers.h"

#include "ui_task.h"
#include "tuner_task.h"
#include "console_task.h"
#include "midi_task.h"
#include "sharc_task.h"

#include "copyselectmenu.h"

static TParamDescriptor vlevelParamDesc = {
	.ptr = &currentPreset.paramData.preset_volume,
	.handlerStr = "vl_pr",
	.dspAddress = DSP_ADDRESS_PRESET_VOLUME,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Level",
	.setterHandler = preset_volume_setter
};

static TParamDescriptor vcontrolParamDesc = {
	.ptr = &currentPreset.paramData.volume_control,
	.handlerStr = "vl_pr_cntrl",
	.dspAddress = DSP_ADDRESS_PRESET_FULL,
	.dspPosition = NOT_SEND_POS,
	.name = "Control",
	.setterHandler = preset_volume_control_setter
};

TPresetDesc PresetDesc = {
	.level = vlevelParamDesc,
	.control = vcontrolParamDesc
};

void preset_volume_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)vlevelParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", vlevelParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(vlevelParamDesc.dspAddress, *param_ptr, 0);
}

void preset_volume_control_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)vcontrolParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", vcontrolParamDesc.handlerStr, *param_ptr);
}

void pnum_getter(uint32_t value)
{
	console_printf("pnum\r%02x\n", currentPresetNumber);
}

void psave_handler(uint32_t value)
{
	currentPreset.modulesBuf[147] = currentPreset.delayTime;
	currentPreset.modulesBuf[148] = currentPreset.delayTime >> 8;

	EEPROM_SavePreset(currentPresetNumber, &currentPreset);

	// need to do it in the same thread
	DSP_SendPrimaryData(currentPreset.cab1Data, currentPreset.cabAuxData, &currentPreset.paramData, currentPresetNumber+1);
	if(System::cab_type==CAB_CONFIG_STEREO)
		DSP_SendCab2Data(currentPreset.cab2Data, currentPresetNumber+1);

	console_printf("psave\r\n");
}

void erase_preset_handler(uint32_t value)
{
	Preset::Erase();
	UITask->setParam(preset_change_handler, currentPresetNumber);
	console_printf("erase_preset\r\n");
}

void preset_change_handler(uint32_t value)
{
	currentPresetNumber = value % 99;

	sys_para[System::LAST_PRESET_NUM] = currentPresetNumber;
	Preset::Change(currentPresetNumber);
	MidiTask->pcSend(TMidiTask::TPcType::PC_INTERNAL, currentPresetNumber);

	mainMenu->presetConfirm(); //update data in main menu
}

void pbrief_getter(uint32_t value)
{
	uint8_t presetNum = value % 99;

	console_printf("pbrief");

	Preset::TPresetBrief presetData;
	EEPROM_LoadPresetBrief(presetNum, &presetData);

	char *cab1NameSrc = presetData.cab1Name + 1;
	char *cab2NameSrc = presetData.cab2Name + 1;
	char cab1NameDst[64];
	char cab2NameDst[64];

	kgp_sdk_libc::memset(cab1NameDst, 0, 64);
	kgp_sdk_libc::memset(cab2NameDst, 0, 64);

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

	console_printf("\r%d|%s|%s|%s|%s|", presetNum, presetData.name, presetData.comment, cab1NameDst, cab2NameDst);

	uint8_t enabled[14];
	kgp_sdk_libc::memcpy(enabled, &presetData.paramData.switches, 14);
	for(uint8_t i=0; i<14; i++) console_printf("%d", enabled[i]);

	console_printf("\n");
}

void set_preset_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler("erase_preset", erase_preset_handler);
	translator->AddAccessorHandler("pchange", preset_change_handler);
	translator->AddAccessorHandler("psave", psave_handler);

	translator->AddAccessorHandler("pbrief", pbrief_getter);
	translator->AddAccessorHandler("pnum", pnum_getter);

	translator->AddAccessorHandler("vl_pr", preset_volume_setter);
	translator->AddAccessorHandler("vl_pr_cntrl", preset_volume_control_setter);
}
