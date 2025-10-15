#include "preset.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"
#include "init.h"

#include "system.h"

Preset::TPreset __CCM_BSS__ currentPreset;

Preset::TCabinet __CCM_BSS__ cab1;
Preset::TCabinet __CCM_BSS__ cab2;
uint8_t __CCM_BSS__ ccmCommonCabBuffer[4096 * 3 * 2];

uint8_t Preset::impulsePath[512];

uint8_t __CCM_BSS__ presetBuffer[38560];

uint16_t __CCM_BSS__ delay_time;
uint16_t __CCM_BSS__ moog_time;
uint16_t __CCM_BSS__ trem_time;

void Preset::Change()
{
	DSP_GuiSendParameter(DSP_ADDRESS_MUTE, currentPresetNumber, 0);

	moog_time = 0;
	trem_time = 0;

	EEPROM_loadPreset(currentPresetNumber);
	pc_mute_fl = 0;

	// DSP binary not set this params?
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_CENTER_POS, currentPreset.modules.rawData[PHASER_CENTER]);
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_WIDTH_POS, currentPreset.modules.rawData[PHASER_WIDTH]);

	MidiSendTask->prog_ch_midi_start();
	MidiSendTask->Give();

	for(uint8_t i = 0; i<3; i++)
	{
		contr_kn[i] = currentPreset.modules.rawData[fo1+i];
		contr_kn1[i] = currentPreset.modules.rawData[fo11+i];
	}

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

	DSP_ErasePrimaryCab(currentPresetNumber+1);

	if(cab_type == CAB_CONFIG_STEREO)
		DSP_EraseSecondaryCab(currentPresetNumber+1);
}

volatile uint8_t __CCM_BSS__ currentPresetNumber;
