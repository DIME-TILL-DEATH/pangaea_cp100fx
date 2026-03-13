#include "preset.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"
#include "init.h"

#include "system.h"
#include "tasks/controllers_task.h"
#include "tasks/midi_task.h"

Preset::TPreset __CCM_BSS__ currentPreset;

Preset::TCabinet __CCM_BSS__ cab1;
Preset::TCabinet __CCM_BSS__ cab2;
uint8_t __CCM_BSS__ ccmCommonCabBuffer[4096 * 3 * 2];

uint8_t Preset::impulsePath[512];

uint8_t __CCM_BSS__ presetBuffer[38560];

uint16_t __CCM_BSS__ Preset::delay_time;
uint16_t __CCM_BSS__ Preset::moog_time;
uint16_t __CCM_BSS__ Preset::trem_time;

bool Preset::cab_data_ready;

void Preset::Change()
{
	DSP_GuiSendParameter(DSP_ADDRESS_MUTE, currentPresetNumber, 0);


	EEPROM_loadPreset(currentPresetNumber);
	pc_mute_fl = 0;

	// DSP binary not set this params?
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_CENTER_POS, currentPreset.modules.rawData[PHASER_CENTER]);
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_WIDTH_POS, currentPreset.modules.rawData[PHASER_WIDTH]);

	if(sys_para[System::EXPR_STORE_LEVEL])
		adc_proc();

	if(sys_para[System::ATTENUATOR_MODE])
		DisplayTask->Pot_Write();

	pc_mute_fl = 1;
}

void Preset::Erase()
{
	preset_erase(currentPresetNumber);
	kgp_sdk_libc::memcpy(currentPreset.modules.rawData, prog_data_init, 512);
	currentPreset.modules.rawData[delay_tim_lo] = 0xf4;
	currentPreset.modules.rawData[delay_tim_hi] = 1;

	for(uint8_t i=0; i<Controller::controllersCount; i++)
	{
		currentPreset.controller[i].src = 0;
		currentPreset.controller[i].dst = Controller::PresetLevel;
		currentPreset.controller[i].minVal = 0;
		currentPreset.controller[i].maxVal = 127;
	}

	DSP_ErasePrimaryCab(currentPresetNumber+1);

	if(cab_type == CAB_CONFIG_STEREO)
		DSP_EraseSecondaryCab(currentPresetNumber+1);
}

volatile uint8_t __CCM_BSS__ currentPresetNumber;
