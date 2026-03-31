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
uint8_t __CCM_BSS__ tempDataBuffer[sizeof(Preset::TPresetHeader)]; // sizeof(TModulesData)

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

	EEPROM_SaveSystemData(); // save current preset number
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

void Preset::SetDefaultValues(Preset::TPresetData* preset)
{
	SetDefaultValues(&preset->presetHeader);

	for(uint8_t i=0; i<Controller::controllersCount; i++)
	{
		preset->controller[i].src = 0;
		preset->controller[i].dst = Controller::PresetLevel;
		preset->controller[i].minVal = 0;
		preset->controller[i].maxVal = 127;
	}

	preset->cab1Data[0] = 0xff;
	preset->cab1Data[1] = 0xff;
	preset->cab1Data[2] = 0x7f;

	if(System::cab_type == CAB_CONFIG_STEREO)
	{
		preset->cab2Data[0] = 0xff;
		preset->cab2Data[1] = 0xff;
		preset->cab2Data[2] = 0x7f;
	}

	preset->delayTime = 500;
	preset->paramData.bpm_delay = 120;
}

void Preset::SetDefaultValues(Preset::TPresetHeader* preset)
{
	kgp_sdk_libc::memcpy(preset->name, "Preset", 6);
	kgp_sdk_libc::memcpy(preset->comment, "Comment", 7);

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
}

void Preset::Copy(uint8_t targetPresetNum, const Preset::TSelectionMask& selectionMask)
{
	uint8_t cab1Name[CAB_NAME_STRING_SIZE];
	uint8_t cab2Name[CAB_NAME_STRING_SIZE];

	Preset::TPresetHeader* copyPreset = (Preset::TPresetHeader*)tempDataBuffer;

	if(!EEPROM_LoadPresetHeader(targetPresetNum, copyPreset))
		Preset::SetDefaultValues(copyPreset);

	if(selectionMask.name)
		kgp_sdk_libc::memcpy(copyPreset->name, currentPreset.name, PRESET_NAME_STRING_SIZE);

	if(selectionMask.comment)
		kgp_sdk_libc::memcpy(copyPreset->comment, currentPreset.comment, PRESET_COMMENT_STRING_SIZE);

	if(selectionMask.rf)
	{
		copyPreset->paramData.resonance_filter = currentPreset.paramData.resonance_filter;
		copyPreset->paramData.switches.resonance_filter = currentPreset.paramData.switches.resonance_filter;
		copyPreset->paramData.resonance_filter_gen_type = currentPreset.paramData.resonance_filter_gen_type;
	}

	if(selectionMask.gt)
	{
		copyPreset->paramData.gate = currentPreset.paramData.gate;
		copyPreset->paramData.switches.gate = currentPreset.paramData.switches.gate;
	}

	if(selectionMask.cm)
	{
		copyPreset->paramData.compressor = currentPreset.paramData.compressor;
		copyPreset->paramData.switches.compressor = currentPreset.paramData.switches.compressor;
	}

	if(selectionMask.pr)
	{
		copyPreset->paramData.preamp = currentPreset.paramData.preamp;
		copyPreset->paramData.switches.preamp = currentPreset.paramData.switches.preamp;
	}

	if(selectionMask.pa)
	{
		copyPreset->paramData.pa = currentPreset.paramData.pa;
		copyPreset->paramData.switches.amp = currentPreset.paramData.switches.amp;
		copyPreset->paramData.presence = currentPreset.paramData.presence;
	}

	if(selectionMask.ir)
	{
		copyPreset->paramData.cab1 = currentPreset.paramData.cab1;
		copyPreset->paramData.cab2 = currentPreset.paramData.cab2;
		copyPreset->paramData.switches.cab = currentPreset.paramData.switches.cab;

		kgp_sdk_libc::memcpy(&tempCabBuffer[0], currentPreset.cab1Data, CAB_DATA_SIZE);
		kgp_sdk_libc::memcpy(cab1Name, &currentPreset.cab1NameSize, CAB_NAME_STRING_SIZE);
#ifdef __MONO_MOD__
		if(System::cab_type == CAB_CONFIG_STEREO)
		{
			kgp_sdk_libc::memcpy(&tempCabBuffer[CAB_DATA_SIZE], currentPreset.cab2Data, CAB_DATA_SIZE);
			kgp_sdk_libc::memcpy(cab2Name, &currentPreset.cab2NameSize, CAB_NAME_STRING_SIZE);
		}
		else
			kgp_sdk_libc::memcpy(&tempCabBuffer[CAB_DATA_SIZE], currentPreset.cabAuxData, CAB_DATA_SIZE);
#endif

#ifdef __STEREO_MOD__
		kgp_sdk_libc::memcpy(&tempCabBuffer[CAB_DATA_SIZE], currentPreset.cab2Data, CAB_DATA_SIZE);
#endif
	}
	else
	{
		if(!EEPROM_LoadCabData(targetPresetNum, tempCabBuffer, cab1Name, cab2Name))
		{
			kgp_sdk_libc::memset(tempCabBuffer, 0, CAB_DATA_SIZE * 2);
			tempCabBuffer[0] = 0x7f;
			tempCabBuffer[1] = 0xff;
			tempCabBuffer[2] = 0xff;

			if(System::cab_type == CAB_CONFIG_STEREO)
			{
				tempCabBuffer[CAB_DATA_SIZE + 0] = 0x7f;
				tempCabBuffer[CAB_DATA_SIZE + 1] = 0xff;
				tempCabBuffer[CAB_DATA_SIZE + 2] = 0xff;
			}
			kgp_sdk_libc::memset(cab1Name, 0, CAB_NAME_STRING_SIZE);
			kgp_sdk_libc::memset(cab2Name, 0, CAB_NAME_STRING_SIZE);
		}
	}
	EEPROM_SaveCabData(targetPresetNum, tempCabBuffer, cab1Name, cab2Name);


	if(selectionMask.eq)
	{
		for(uint8_t i = 0; i<5; i++)
		{
			copyPreset->paramData.eq_freq[i] = currentPreset.paramData.eq_freq[i];
			copyPreset->paramData.eq_gain[i] = currentPreset.paramData.eq_gain[i];
			copyPreset->paramData.eq_q[i] = currentPreset.paramData.eq_q[i];
		}
		copyPreset->paramData.switches.eq = currentPreset.paramData.switches.eq;
		copyPreset->paramData.eq_pre_post = currentPreset.paramData.eq_pre_post;
		copyPreset->paramData.lpf = currentPreset.paramData.lpf;
		copyPreset->paramData.hpf = currentPreset.paramData.hpf;
	}

	if(selectionMask.ph)
	{
		copyPreset->paramData.phaser = currentPreset.paramData.phaser;
		copyPreset->paramData.switches.phaser = currentPreset.paramData.switches.phaser;
		copyPreset->paramData.hpf_phaser = currentPreset.paramData.hpf_phaser;
		copyPreset->paramData.phaser_pre_post = currentPreset.paramData.phaser_pre_post;
	}

	if(selectionMask.fl)
	{
		copyPreset->paramData.flanger = currentPreset.paramData.flanger;
		copyPreset->paramData.switches.flanger = currentPreset.paramData.switches.flanger;
		copyPreset->paramData.hpf_flanger = currentPreset.paramData.hpf_flanger;
		copyPreset->paramData.flanger_pre_post = currentPreset.paramData.flanger_pre_post;
	}

	if(selectionMask.ch)
	{
		copyPreset->paramData.chorus = currentPreset.paramData.chorus;
		copyPreset->paramData.switches.chorus = currentPreset.paramData.switches.chorus;
		copyPreset->paramData.hpf_chorus = currentPreset.paramData.hpf_chorus;
	}

	if(selectionMask.dl)
	{
		copyPreset->paramData.delay = currentPreset.paramData.delay;
		copyPreset->paramData.switches.delay = currentPreset.paramData.switches.delay;
		copyPreset->paramData.delay_tail = currentPreset.paramData.delay_tail;
		copyPreset->paramData.delay_tap = currentPreset.paramData.delay_tap;
		copyPreset->paramData.delay_time = currentPreset.paramData.delay_time;

		copyPreset->delayTime = currentPreset.delayTime;
		copyPreset->paramData.bpm_delay = currentPreset.paramData.bpm_delay;
	}

	if(selectionMask.er)
	{
		copyPreset->paramData.early_reflections = currentPreset.paramData.early_reflections;
		copyPreset->paramData.switches.early_reflections = currentPreset.paramData.switches.early_reflections;
	}

	if(selectionMask.rv)
	{
		copyPreset->paramData.reverb = currentPreset.paramData.reverb;
		copyPreset->paramData.switches.reverb = currentPreset.paramData.switches.reverb;
		copyPreset->paramData.reverb_diffusion = currentPreset.paramData.reverb_diffusion;
		copyPreset->paramData.reverb_predelay = currentPreset.paramData.reverb_predelay;
		copyPreset->paramData.reverb_tail = currentPreset.paramData.reverb_tail;
		copyPreset->paramData.reverb_type = currentPreset.paramData.reverb_type;
	}

	if(selectionMask.tr)
	{
		copyPreset->paramData.tremolo = currentPreset.paramData.tremolo;
		copyPreset->paramData.switches.tremolo = currentPreset.paramData.switches.tremolo;
		copyPreset->paramData.tremolo_tap = currentPreset.paramData.tremolo_tap;
		copyPreset->paramData.tremolo_lfo_type = currentPreset.paramData.tremolo_lfo_type;
	}

	if(selectionMask.pv)
		copyPreset->paramData.preset_volume = currentPreset.paramData.preset_volume;

	if(selectionMask.att)
		copyPreset->paramData.attenuator = currentPreset.paramData.attenuator;

	if(selectionMask.controllers)
	{
		for(uint16_t i = 0 ; i < Controller::controllersCount ; i++)
		{
			copyPreset->controller[i].src = currentPreset.controller[i].src;
			copyPreset->controller[i].dst = currentPreset.controller[i].dst;
			copyPreset->controller[i].minVal = currentPreset.controller[i].minVal;
			copyPreset->controller[i].maxVal = currentPreset.controller[i].maxVal;
		}
	}

#ifdef __STEREO_MOD__
    copyPreset->in_left_en = currentPreset.in_left_en;
	copyPreset->in_right_en = currentPreset.in_right_en;
	copyPreset->left_pan = currentPreset.left_pan;
	copyPreset->right_pan = currentPreset.right_pan;
#endif

	copyPreset->pcOut = currentPreset.pcOut;
	copyPreset->set = currentPreset.set;

	EEPROM_SavePresetHeader(targetPresetNum, (Preset::TPresetHeader*)tempDataBuffer);

	SharcTask->sendPrimaryData(&tempCabBuffer[0], &tempCabBuffer[CAB_DATA_SIZE], &copyPreset->paramData, targetPresetNum+1);
	if(System::cab_type==CAB_CONFIG_STEREO)
			SharcTask->sendCab2Data(&tempCabBuffer[CAB_DATA_SIZE], currentPresetNumber+1);
}

void Preset::Erase()
{
	EEPROM_ErasePreset(currentPresetNumber);
	Preset::SetDefaultValues(&currentPreset);

	SharcTask->eraseCab1(currentPresetNumber+1);
	if(System::cab_type == CAB_CONFIG_STEREO)
		SharcTask->eraseCab2(currentPresetNumber+1);
}
