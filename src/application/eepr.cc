#include "eepr.h"

#include "sharc.h"

#include "system.h"
#include "controller.h"
#include "preset.h"
#include "modules.h"

#include "filesystem_task.h"

//volatile uint32_t flash_adr;


const uint8_t prog_data_init[512] =
{/*switch*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*empty*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*cab1*/82, 63, 0,
/*cab2*/0, 63, 0,
/*eq*/15, 15, 15, 15, 15,
/*hpf_lpf_pres*/0, 0, 0,
/*phas*/63, 49, 0, 55, 0, 0,
/*chor*/63, 31, 74, 4, 0,
/*del*/40, 63, 0, 0, 63, 0, 63, 63, 0, 0, 0,
/*flanger*/63, 0, 31, 74, 0, 0,
/*rev*/30, 63, 41, 25, 0, 0, 0,
/*trem*/63, 63, 0, 0,
/*pres_vol*/127,
/*amp_sim*/0, 0, 127,
/*early reflec*/63, 5,
/*eq_par*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/*preamp_par*/0, 127, 0, 64, 64, 64,
/*gate*/0, 0, 0,
/*compressor*/50, 127, 84, 0, 10,
/*HPF_mod*/0, 0, 0,
/*del tap*/0,/*trem tap*/0,
/*moog*/127, 0, 0, 47, 55, 52, 85, 80, 11, 66, 127,
/*rever type*/0,/*rever diffusion*/0,
/*moog gen type*/0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0,
		/*bpm del*/120};

const uint32_t del_tim_init = 500;
const uint8_t nameInit[] = {"Preset        "};
const uint8_t commentInit[] = {"Comment       "};



void EEPROM_Start()
{
	DIR dir;
	FATFS fs;
	FIL file;
	UINT file_size;
	FRESULT res;

	sys_para[System::MASTER_VOLUME] = 127;

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
	if(f_size(&file)==0)
	{
		sys_para[System::TUNER_EXTERNAL] = 0x81;
		f_write(&file, sys_para, 512, &file_size);
	}

	f_read(&file, sys_para, 512, &file_size);
	f_close(&file);
	f_mount(0, "1:", 0);

	if(!sys_para[System::MIDI_PC_IND])
	{
		for(uint8_t i = 0; i<128; i++)
		{
			if(i<99)
				sys_para[i+128] = i;
			else
				sys_para[i+128] = i-99;
		}
		sys_para[System::MIDI_PC_IND] = 1;
		EEPROM_WriteSys();
	}

	cab_type = sys_para[System::CAB_SIM_CONFIG];

	cab1.data = &currentPreset.cabinetDataBuf[0];
	cab2.data = &currentPreset.cabinetDataBuf[CAB_DATA_SIZE];
}

void EEPROM_WriteSys(void)
{
	// Значения поумолчанию чтобы не решать квадратное уравнение
	// mstEqMidFreq = (mas_eq_fr * (20.0f/20000.0f) + 1) * mas_eq_fr;
	//	mas_eq_fr = sys_para[508] << 8;
	//	mas_eq_fr |= sys_para[509];
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

void EEPROM_WritePreset(uint8_t nu)
{
	nu++;

	char fna[_MAX_LFN];
	FATFS fs;
	FIL file;
	UINT f_size;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	f_open(&file, fna, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

	currentPreset.modules.paramData.delay_time = currentPreset.delayTime; // actual data
	f_write(&file, &currentPreset, sizeof(Preset::TPresetData), &f_size);

	f_write(&file, &currentPreset.delayTime, 2, &f_size);
	f_write(&file, cab1.data, CAB_DATA_SIZE, &f_size);
	f_write(&file, &cab1.name, 64, &f_size);
	if(cab_type == CAB_CONFIG_STEREO)
	{
		f_write(&file, cab2.data, CAB_DATA_SIZE, &f_size);
		f_write(&file, &cab2.name, 64, &f_size);
		f_lseek(&file, sizeof(Preset::TPresetData) + 2 + CAB_DATA_SIZE * 3 + 64 * 2); // 38048
	}
	else
	{
		f_lseek(&file, sizeof(Preset::TPresetData) + 2 + CAB_DATA_SIZE * 2 + 64 * 2); //25760
		f_write(&file, cab1.data + CAB_DATA_SIZE, CAB_DATA_SIZE, &f_size);
	}
	f_write(&file, currentPreset.currentImpulsePath, 255, &f_size);
	f_write(&file, currentPreset.currentImpulseName, 255, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
}

bool EEPROM_LoadPreset(uint8_t numPreset)
{
	numPreset++;

	char fna[_MAX_LFN];
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;

	kgp_sdk_libc::memset(&currentPreset, 0, sizeof(Preset::TPresetData));

	if(numPreset<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)numPreset);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)numPreset);

	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_read(&file, &currentPreset, sizeof(Preset::TPresetData), &f_size);

		// Send data refactor.Not neded moving data
		cab1.data = &currentPreset.cabinetDataBuf[0];

		kgp_sdk_libc::memcpy(&cab1.name, &currentPreset.cabinetDataBuf[CAB_DATA_SIZE], CAB_NAME_STRING_SIZE);

		if(cab_type == CAB_CONFIG_STEREO)
		{
			cab2.data = &currentPreset.cabinetDataBuf[CAB_DATA_SIZE + CAB_NAME_STRING_SIZE];
			kgp_sdk_libc::memcpy(&cab2.name, &currentPreset.cabinetDataBuf[2 *CAB_DATA_SIZE + CAB_NAME_STRING_SIZE], CAB_NAME_STRING_SIZE);
		}
		else
		{
			kgp_sdk_libc::memcpy(&cab1.data[CAB_DATA_SIZE], &currentPreset.cabinetDataBuf[2 * CAB_DATA_SIZE + 2 * CAB_NAME_STRING_SIZE], CAB_DATA_SIZE);
		}

		f_close(&file);
		return true;
	}
	else
	{
		for(uint16_t i = 0; i<512; i++)
			currentPreset.modules.rawData[i] = prog_data_init[i];

		for(uint8_t i = 0; i<15; i++)
			currentPreset.name[i] = nameInit[i];
		for(uint8_t i = 0; i<15; i++)
			currentPreset.comment[i] = commentInit[i];

		for(uint8_t i=0; i<Controller::controllersCount; i++)
		{
			currentPreset.controller[i].src = 0;
			currentPreset.controller[i].dst = Controller::PresetLevel;
			currentPreset.controller[i].minVal = 0;
			currentPreset.controller[i].maxVal = 127;
		}

		currentPreset.delayTime = del_tim_init;

		cab1.data[0] = 0xff;
		cab1.data[1] = 0xff;
		cab1.data[2] = 0x7f;

		if(cab_type==2)
		{
			cab2.data[0] = 0xff;
			cab2.data[1] = 0xff;
			cab2.data[2] = 0x7f;
		}
		return false;
	}
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
		kgp_sdk_libc::memcpy(presetData->name, nameInit, 15);
		kgp_sdk_libc::memcpy(presetData->comment, nameInit, 15);
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

}
