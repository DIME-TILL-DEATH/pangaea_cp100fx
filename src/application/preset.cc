#include "preset.h"

#include "adc.h"
#include "sharc.h"

#include "eepr.h"
#include "system.h"
#include "modules.h"

#include "io_task.h"
#include "midi_task.h"
#include "filesystem_task.h"


Preset::TPresetData __CCM_BSS__ currentPreset;

uint16_t __CCM_BSS__ Preset::moog_time;
uint16_t __CCM_BSS__ Preset::trem_time;

uint8_t __CCM_BSS__ tempCabBuffer[CAB_DATA_SIZE * 2];
uint8_t __CCM_BSS__ tempDataBuffer[512]; // sizeof(TModulesData)

volatile uint8_t pc_mute_fl;
void Preset::Change()
{
	DSP_GuiSendParameter(DSP_ADDRESS_MUTE, currentPresetNumber, 0);

	pc_mute_fl = 0;
	EEPROM_LoadPreset(currentPresetNumber);

	// DSP binary not set this params?
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_CENTER_POS, currentPreset.modulesBuf[PHASER_CENTER]);
	DSP_GuiSendParameter(DSP_ADDRESS_PHASER, PHASER_WIDTH_POS, currentPreset.modulesBuf[PHASER_WIDTH]);

	if(sys_para[System::EXPR_STORE_LEVEL])
		ADC_Routine();

	if(sys_para[System::ATTENUATOR_MODE])
		IOTask->potWrite();

	EEPROM_WriteSys(); // save current preset number
//-----------------------------------------------------------
	FileSystemTask->Object().name = emb_string((char*)&currentPreset.currentImpulsePath[0]);
	FileSystemTask->Object().startup = emb_string((char*)&currentPreset.currentImpulseName[0]);

	emb_string startup = FileSystemTask->Object().name + emb_string("/") + FileSystemTask->Object().startup;

	FILINFO fno;
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	lfn[0] = 0 ;
	fno.lfname = lfn;
	fno.lfsize = sizeof lfn;
#endif

	FRESULT fs_res = f_stat(startup.c_str(), &fno);
	if(fs_res == FR_OK)
	{
		// pack path here!
		FileSystemTask->SendCommand(TFsBrowser::bcStartup);
	}
//------------------------------------------------------------
	pc_mute_fl = 1;
}

void Preset::Erase()
{
	EEPROM_PresetErase(currentPresetNumber);
	kgp_sdk_libc::memcpy(currentPreset.modulesBuf, prog_data_init, 512);
	currentPreset.modulesBuf[delay_tim_lo] = 0xf4;
	currentPreset.modulesBuf[delay_tim_hi] = 1;

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
