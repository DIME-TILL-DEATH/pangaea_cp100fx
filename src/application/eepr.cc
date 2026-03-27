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
		EEPROM_SaveSystemData();
	}

	System::cab_type = sys_para[System::CAB_SIM_CONFIG];
}

void EEPROM_SaveSystemData(void)
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

bool EEPROM_LoadPresetBrief(uint8_t presetNum, Preset::TPresetBrief* presetData)
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
		f_close(&file);
		return true;
	}
	else
	{
		kgp_sdk_libc::memcpy(presetData->name, "Preset", 6);
		kgp_sdk_libc::memcpy(presetData->comment, "Comment", 7);
		return false;
	}
}

bool EEPROM_LoadPresetHeader(uint8_t presetNum, Preset::TPresetHeader* presetData)
{
	char fna[_MAX_LFN];
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;

	kgp_sdk_libc::memset(presetData, 0, sizeof(Preset::TPresetHeader));

	presetNum++;
	if(presetNum<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)presetNum);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)presetNum);

	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_read(&file, presetData, sizeof(Preset::TPresetHeader), &f_size);
		f_close(&file);
		return true;
	}
	else return false;
}

bool EEPROM_LoadCabData(uint8_t presetNum, uint8_t* buf)
{
	char fna[_MAX_LFN];
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;

	kgp_sdk_libc::memset(buf, 0, CAB_DATA_SIZE * 2);

	presetNum++;
	if(presetNum<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)presetNum);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)presetNum);

	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_lseek(&file, CAB1_DATA_OFFSET);
		f_read(&file, &buf[0], sizeof(Preset::TPresetData), &f_size);

		if(System::cab_type == CAB_CONFIG_STEREO)
		{
			f_lseek(&file, CAB2_DATA_OFFSET);
			f_read(&file, &buf[CAB_DATA_SIZE], sizeof(Preset::TPresetData), &f_size);
		}
		else
		{
			f_lseek(&file, CAB1_AUX_DATA_OFFSET);
			f_read(&file, &buf[CAB_DATA_SIZE], sizeof(Preset::TPresetData), &f_size);
		}
		f_close(&file);
		return true;
	}
	else return false;
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

void EEPROM_ErasePreset(uint8_t presetNum)
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
