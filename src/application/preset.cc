#include "preset.h"

#include "adc.h"

#include "eepr.h"
#include "system.h"
#include "modules.h"

#include "io_task.h"
#include "midi_task.h"
#include "filesystem_task.h"
#include "sharc_task.h"


Preset::TPresetData __CCM_BSS__ currentPreset;

uint16_t __CCM_BSS__ Preset::moog_time;
uint16_t __CCM_BSS__ Preset::trem_time;

uint8_t __CCM_BSS__ tempCabBuffer[CAB_DATA_SIZE * 2];
uint8_t __CCM_BSS__ tempDataBuffer[512]; // sizeof(TModulesData)

volatile uint8_t __CCM_BSS__ currentPresetNumber;

void Preset::Change(uint8_t presetNumber)
{
	currentPresetNumber = presetNumber;

	SharcTask->setParameter(DSP_ADDRESS_MUTE, currentPresetNumber, 0);

	if(!EEPROM_LoadPreset(currentPresetNumber, &currentPreset))
		Preset::SetDefaultValues(&currentPreset);

	// DSP binary not set this params in full data send?
	SharcTask->setParameter(DSP_ADDRESS_PHASER, PHASER_CENTER_POS, currentPreset.modulesBuf[PHASER_CENTER]);
	SharcTask->setParameter(DSP_ADDRESS_PHASER, PHASER_WIDTH_POS, currentPreset.modulesBuf[PHASER_WIDTH]);

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

	if(f_stat(startup.c_str(), &fno) == FR_OK)
	{
		// pack path here!
		FileSystemTask->SendCommand(TFsBrowser::bcStartup);
	}

	EXPR_StoreLevel();
}

void Preset::Erase()
{
	EEPROM_PresetErase(currentPresetNumber);
	Preset::SetDefaultValues(&currentPreset);

	SharcTask->eraseCab1(currentPresetNumber+1);
	if(System::cab_type == CAB_CONFIG_STEREO)
		SharcTask->eraseCab2(currentPresetNumber+1);
}

void Preset::SetDefaultValues(Preset::TPresetData* preset)
{
	for(uint16_t i = 0; i<512; i++)
		preset->modulesBuf[i] = prog_data_init[i];

	preset->paramData.cab1.volume = 82;
	preset->paramData.cab1.pan = 63;

	preset->paramData.cab2.pan = 63;

	for(uint8_t i=0; i<5; i++)
		preset->paramData.eq_gain[i] = 15;

	preset->paramData.resonance_filter.mix = 127;
	preset->paramData.resonance_filter.lfo_rate = 47;
	preset->paramData.resonance_filter.lpf = 55;
	preset->paramData.resonance_filter.hpf = 52;
	preset->paramData.resonance_filter.resonance = 85;
	preset->paramData.resonance_filter.dyn_threshold = 80;
	preset->paramData.resonance_filter.dyn_attack = 11;
	preset->paramData.resonance_filter.dyn_release = 66;
	preset->paramData.resonance_filter.volume = 127;

	preset->paramData.compressor.threshold = 50;
	preset->paramData.compressor.ratio = 127;
	preset->paramData.compressor.volume = 84;
	preset->paramData.compressor.knee = 10;

	preset->paramData.preamp.volume = 127;
	preset->paramData.preamp.low = 64;
	preset->paramData.preamp.mid = 64;
	preset->paramData.preamp.high = 64;

	preset->paramData.pa.level = 127;

	preset->paramData.phaser.mix = 63;
	preset->paramData.phaser.rate = 49;
	preset->paramData.phaser.center = 55;

	preset->paramData.chorus.mix = 63;
	preset->paramData.chorus.rate = 31;
	preset->paramData.chorus.width = 74;
	preset->paramData.chorus.delay = 4;

	preset->paramData.flanger.mix = 63;
	preset->paramData.flanger.rate = 31;
	preset->paramData.flanger.width = 74;

	preset->paramData.delay.volumeFirst = 40;
	preset->paramData.delay.feedback = 63;
	preset->paramData.delay.panFirst = 63;
	preset->paramData.delay.panSecond = 63;
	preset->paramData.delay.offset = 63;

	preset->paramData.early_reflections.volume = 63;
	preset->paramData.early_reflections.type = 5;

	preset->paramData.reverb.mix = 30;
	preset->paramData.reverb.time = 63;
	preset->paramData.reverb.size = 41;
	preset->paramData.reverb.damping = 25;

	preset->paramData.tremolo.depth = 63;
	preset->paramData.tremolo.rate = 63;

	preset->paramData.preset_volume = 127;

	kgp_sdk_libc::memcpy(preset->name, "Preset", 6);
	kgp_sdk_libc::memcpy(preset->comment, "Comment", 7);

	for(uint8_t i=0; i<Controller::controllersCount; i++)
	{
		preset->controller[i].src = 0;
		preset->controller[i].dst = Controller::PresetLevel;
		preset->controller[i].minVal = 0;
		preset->controller[i].maxVal = 127;
	}

	preset->delayTime = 500;

	preset->paramData.bpm_delay = 120;

	preset->cab1Data[0] = 0xff;
	preset->cab1Data[1] = 0xff;
	preset->cab1Data[2] = 0x7f;

	if(System::cab_type == CAB_CONFIG_STEREO)
	{
		preset->cab2Data[0] = 0xff;
		preset->cab2Data[1] = 0xff;
		preset->cab2Data[2] = 0x7f;
	}
}
