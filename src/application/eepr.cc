#include "eepr.h"

#include "system.h"
#include "controller.h"
#include "preset.h"
#include "modules.h"

#include "ff.h"

void EEPROM_Start()
{
	DIR dir;
	FATFS fs;
	FIL file;
	UINT file_size;
	FRESULT res;

	sys_para[System::MASTER_VOLUME] = 127;
	sys_para[System::TUNER_EXTERNAL] = 0x81;

	if(f_mount(&fs, "1:", 1)!=FR_OK)
	{
		f_mkfs("1:", 0, 0);
		f_setlabel("PANGEA");
		f_mount(&fs, "1:", 1);
	}

	res = f_opendir(&dir, "1:PRESETS");
	if(res==FR_OK)
		f_closedir(&dir);
	else
		f_mkdir("1:PRESETS");

	res = f_open(&file, "1:system.pan", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(f_size(&file) == 0)
	{
		f_write(&file, sys_para, 512, &file_size);
	}
	else
	{
		f_read(&file, sys_para, 512, &file_size);
	}
	f_close(&file);
	f_mount(0, "1:", 0);

	if(!sys_para[System::MIDI_PC_IND])
	{
		for(uint8_t i = 0; i<128; i++)
		{
			if(i<99)
				sys_para[System::MIDI_MAP_START + i] = i;
			else
				sys_para[System::MIDI_MAP_START + 1] = i-99;
		}
		sys_para[System::MIDI_PC_IND] = 1;
		EEPROM_WriteSys();
	}

	System::cab_type = sys_para[System::CAB_SIM_CONFIG];
}

void EEPROM_WriteSys(void)
{
	// Значения поумолчанию чтобы не решать квадратное уравнение
	// mstEqMidFreq = (mas_eq_fr * (20.0f/20000.0f) + 1) * mas_eq_fr;
	sys_para[System::MASTER_EQ_FREQ_VAL_LO] = 2;
	sys_para[System::MASTER_EQ_FREQ_VAL_HI] = 0x6a;

	sys_para[System::MASTER_EQ_FREQ_LO] = (mstEqMidFreq>>8) & 0xF;
	sys_para[System::MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xF;

	FATFS fs;
	FIL file;
	UINT f_size;
	f_mount(&fs, "1:", 1);
	f_open(&file, "1:system.pan", FA_WRITE);
	f_write(&file, sys_para, 512, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
}

void EEPROM_SavePreset(uint8_t numPreset, Preset::TPresetData* savePreset)
{
	char fna[_MAX_LFN];
	FATFS fs;
	FIL file;
	UINT f_size;

	numPreset++;
	if(numPreset<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)numPreset);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)numPreset);

	f_mount(&fs, "1:", 1);
	f_open(&file, fna, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

	savePreset->paramData.delay_time = savePreset->delayTime; // actual data
	f_write(&file, savePreset, sizeof(Preset::TPresetData), &f_size);

	f_close(&file);
	f_mount(0, "1:", 0);
}

bool EEPROM_LoadPreset(uint8_t numPreset, Preset::TPresetData* loadPreset)
{
	char fna[_MAX_LFN];
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;

	kgp_sdk_libc::memset(loadPreset, 0, sizeof(Preset::TPresetData));

	numPreset++;
	if(numPreset<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)numPreset);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)numPreset);

	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_read(&file, loadPreset, sizeof(Preset::TPresetData), &f_size);
		f_close(&file);
		return true;
	}
	else return false;
}

void EEPROM_LoadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData)
{
	char fna[_MAX_LFN];
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;

	kgp_sdk_libc::memset(presetData, 0, sizeof(Preset::TPresetBrief));

	presetNum++;
	if(presetNum<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)presetNum);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)presetNum);

	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_read(&file, presetData, 15 + 15 + sizeof(Preset::TModulesData), &f_size);

		f_lseek(&file, CAB1_NAME_OFFSET);
		f_read(&file, presetData->cab1Name, CAB_NAME_STRING_SIZE, &f_size);

		f_lseek(&file, f_tell(&file) + CAB_DATA_SIZE);
		f_read(&file, presetData->cab2Name, CAB_NAME_STRING_SIZE, &f_size);
	}
	else
	{
		kgp_sdk_libc::memcpy(presetData->name, "Preset", 6);
		kgp_sdk_libc::memcpy(presetData->comment, "Comment", 7);
	}
	f_close(&file);
}

void EEPROM_PresetErase(uint8_t presetNum)
{
	presetNum++;
	FATFS fs;

	f_mount(&fs, "1:", 1);
	char fna[_MAX_LFN];
	if(presetNum<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)presetNum);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)presetNum);

	f_unlink(fna);
	f_mount(0, "1:", 0);
}

void EEPROM_CopyPreset(uint8_t targetPresetNum, const Preset::TSelectionMask& selectionMask)
{
	if(!EEPROM_LoadPreset(targetPresetNum, (Preset::TPresetData*)&tempDataBuffer))
		Preset::SetDefaultValues((Preset::TPresetData*)&tempDataBuffer);

	Preset::TPresetData* copyPreset = (Preset::TPresetData*)&tempDataBuffer;

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

		copyPreset->cab1NameSize = currentPreset.cab1NameSize;
		kgp_sdk_libc::memcpy(copyPreset->cab1Name, currentPreset.cab1Name, CAB_NAME_STRING_SIZE - 1);
		kgp_sdk_libc::memcpy(copyPreset->cab1Data, currentPreset.cab1Data, CAB_DATA_SIZE);

		if(System::cab_type == CAB_CONFIG_STEREO)
		{
			copyPreset->cab2NameSize = currentPreset.cab2NameSize;
			kgp_sdk_libc::memcpy(copyPreset->cab2Name, currentPreset.cab2Name, CAB_NAME_STRING_SIZE - 1);
			kgp_sdk_libc::memcpy(copyPreset->cab2Data, currentPreset.cab2Data, CAB_DATA_SIZE);
		}
		else
			kgp_sdk_libc::memcpy(copyPreset->cabAuxData, currentPreset.cabAuxData, CAB_DATA_SIZE);

		kgp_sdk_libc::memcpy(copyPreset->currentImpulseName, currentPreset.currentImpulseName, 256);
		kgp_sdk_libc::memcpy(copyPreset->currentImpulsePath, currentPreset.currentImpulsePath, 256);
	}

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

	EEPROM_SavePreset(targetPresetNum, (Preset::TPresetData*)&tempDataBuffer);
}
