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

void EEPROM_WritePreset(uint8_t numPreset)
{
	numPreset++;

	char fna[_MAX_LFN];
	FATFS fs;
	FIL file;
	UINT f_size;
	if(numPreset<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)numPreset);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)numPreset);
	f_mount(&fs, "1:", 1);
	f_open(&file, fna, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);

	currentPreset.paramData.delay_time = currentPreset.delayTime; // actual data
	f_write(&file, &currentPreset, sizeof(Preset::TPresetData), &f_size);

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
		f_close(&file);
		return true;
	}
	else
	{
		for(uint16_t i = 0; i<512; i++)
			currentPreset.modulesBuf[i] = prog_data_init[i];

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

		currentPreset.cab1Data[0] = 0xff;
		currentPreset.cab1Data[1] = 0xff;
		currentPreset.cab1Data[2] = 0x7f;

		if(System::cab_type == CAB_CONFIG_STEREO)
		{
			currentPreset.cab2Data[0] = 0xff;
			currentPreset.cab2Data[1] = 0xff;
			currentPreset.cab2Data[2] = 0x7f;
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
	//	EEPROM_LoadPresetToBuffer(targetPresetNum, presetBuffer);
	//
	//	if(selectionMask.name)
	//	{
	//		kgp_sdk_libc::memcpy(presetBuffer, currentPreset.name, 15);
	//	}
	//
	//	if(selectionMask.comment)
	//	{
	//		kgp_sdk_libc::memcpy(presetBuffer + 15, currentPreset.comment, 15);
	//	}
	//
	//	if(selectionMask.rf)
	//	{
	//			kgp_sdk_libc::memcpy(&presetBuffer[PRESET_DATA_OFFSET + RFILTER_MIX], &currentPreset.modulesBuf[RFILTER_MIX], sizeof(Preset::TRfData));
	//			presetBuffer[PRESET_DATA_OFFSET + ENABLE_RESONANCE_FILTER] = currentPreset.modulesBuf[ENABLE_RESONANCE_FILTER];
	//			presetBuffer[PRESET_DATA_OFFSET + RFILTER_LFO_TYPE] = currentPreset.modulesBuf[RFILTER_LFO_TYPE];
	//	}
	//
	//	if(selectionMask.gt)
	//	{
	//		for(uint8_t i = 0; i<3; i++)
	//			presetBuffer[30+gate_thr+i] = currentPreset.modulesBuf[gate_thr+i];
	//
	//		presetBuffer[PRESET_DATA_OFFSET + ENABLE_GATE] = currentPreset.modulesBuf[ENABLE_GATE];
	//	}
	//
	//	if(selectionMask.cm)
	//	{
	//		for(uint8_t i = 0; i<5; i++)
	//			presetBuffer[30+comp_thr+i] = currentPreset.modulesBuf[comp_thr+i];
	//		presetBuffer[30+compr] = currentPreset.modulesBuf[compr];
	//	}
	//
	//	if(selectionMask.pr)
	//	{
	//		for(uint8_t i = 0; i<6; i++)
	//			presetBuffer[30+pre_gain+i] = currentPreset.modulesBuf[pre_gain+i];
	//		presetBuffer[30+pream] = currentPreset.modulesBuf[pream];
	//	}
	//
	//	if(selectionMask.pa)
	//	{
	//		for(uint8_t i = 0; i<3; i++)
	//			presetBuffer[30+am_v+i] = currentPreset.modulesBuf[am_v+i];
	//		presetBuffer[30+pre_v] = currentPreset.modulesBuf[pre_v];
	//		presetBuffer[30+amp] = currentPreset.modulesBuf[amp];
	//	}
	//
	//	if(selectionMask.ir)
	//	{
	//		for(uint8_t i = 0; i<6; i++)
	//			presetBuffer[30+vol+i] = currentPreset.modulesBuf[vol+i];
	//
	//		kgp_sdk_libc::memcpy(&presetBuffer[13344], cab1.name.string, 64);
	//		kgp_sdk_libc::memcpy(&presetBuffer[25696], cab2.name.string, 64);
	//
	//		for(uint16_t i = 0; i<12288; i++)
	//			presetBuffer[1056+i] = cab1.data[i];
	//
	//		if(cab_type==CAB_CONFIG_STEREO)
	//			for(uint16_t i = 0; i<4096*3; i++)
	//				presetBuffer[13408+i] = cab2.data[i];
	//		else
	//			for(uint16_t i = 0; i<4096*3; i++)
	//				presetBuffer[25760+i] = cab1.data[i + 4096 * 3];
	//
	////		for(uint16_t i = 0; i<512; i++)
	////			presetBuffer[38048+i] = Preset::impulsePath[i];
	//
	//		Preset::cab_data_ready = true; // Kostyl
	//		send_cab_data(0, targetPresetNum+1, 0);
	//
	//		if(cab_type==CAB_CONFIG_STEREO)
	//			send_cab_data1(0, targetPresetNum+1, cab2.data);
	//
	//		Preset::cab_data_ready = false;
	//
	//		presetBuffer[30+cab] = currentPreset.modulesBuf[cab];
	//	}
	//
	//	if(selectionMask.eq)
	//	{
	//		for(uint8_t i = 0; i<8; i++)
	//			presetBuffer[30+eq1+i] = currentPreset.modulesBuf[eq1+i];
	//		for(uint8_t i = 0; i<10; i++)
	//			presetBuffer[30+f1+i] = currentPreset.modulesBuf[f1+i];
	//		presetBuffer[30+eq] = currentPreset.modulesBuf[eq];
	//		presetBuffer[30+eq_pr_po] = currentPreset.modulesBuf[eq_pr_po];
	//	}
	//
	//	if(selectionMask.ph)
	//	{
	//		for(uint8_t i = 0; i<6; i++)
	//			presetBuffer[30+phaser_vol+i] = currentPreset.modulesBuf[phaser_vol+i];
	//		presetBuffer[30+phas] = currentPreset.modulesBuf[phas];
	//		presetBuffer[30+hpf_ph] = currentPreset.modulesBuf[hpf_ph];
	//		presetBuffer[30+phas_pos] = currentPreset.modulesBuf[phas_pos];
	//	}
	//
	//	if(selectionMask.fl)
	//	{
	//		for(uint8_t i = 0; i<6; i++)
	//			presetBuffer[30+fl_v+i] = currentPreset.modulesBuf[fl_v+i];
	//		presetBuffer[30+fl] = currentPreset.modulesBuf[fl];
	//		presetBuffer[30+hpf_fl] = currentPreset.modulesBuf[hpf_fl];
	//		presetBuffer[30+flan_pos] = currentPreset.modulesBuf[flan_pos];
	//	}
	//
	//	if(selectionMask.ch)
	//	{
	//		for(uint8_t i = 0; i<5; i++)
	//			presetBuffer[30+chor_volum+i] = currentPreset.modulesBuf[chor_volum+i];
	//		presetBuffer[30+chor] = currentPreset.modulesBuf[chor];
	//		presetBuffer[30+hpf_ch] = currentPreset.modulesBuf[hpf_ch];
	//	}
	//
	//	if(selectionMask.dl)
	//	{
	//		for(uint8_t i = 0; i<11; i++)
	//			presetBuffer[30+d_vol+i] = currentPreset.modulesBuf[d_vol+i];
	//
	//		presetBuffer[1054] = Preset::delay_time;
	//		presetBuffer[1055] = Preset::delay_time>>8;
	//
	//		presetBuffer[30+delay] = currentPreset.modulesBuf[delay];
	//		presetBuffer[30+d_tap_t] = currentPreset.modulesBuf[d_tap_t];
	//		presetBuffer[30+d_tail] = currentPreset.modulesBuf[d_tail];
	//	}
	//
	//	if(selectionMask.er)
	//	{
	//		for(uint8_t i = 0; i<2; i++)
	//			presetBuffer[30+early_vol+i] = currentPreset.modulesBuf[early_vol+i];
	//		presetBuffer[30+early] = currentPreset.modulesBuf[early];
	//	}
	//
	//	if(selectionMask.rv)
	//	{
	//		for(uint8_t i = 0; i<7; i++)
	//			presetBuffer[30+r_vol+i] = currentPreset.modulesBuf[r_vol+i];
	//		presetBuffer[30+reve] = currentPreset.modulesBuf[reve];
	//		presetBuffer[30+rev_t] = currentPreset.modulesBuf[rev_t];
	//		presetBuffer[30+rev_di] = currentPreset.modulesBuf[rev_di];
	//		presetBuffer[30+r_pre] = currentPreset.modulesBuf[r_pre];
	//		presetBuffer[30+r_tail] = currentPreset.modulesBuf[r_tail];
	//	}
	//
	//	if(selectionMask.tr)
	//	{
	//		for(uint8_t i = 0; i<4; i++)
	//			presetBuffer[30+tr_vol+i] = currentPreset.modulesBuf[tr_vol+i];
	//
	//		presetBuffer[30+trem] = currentPreset.modulesBuf[trem];
	//		presetBuffer[30+t_tap_t] = currentPreset.modulesBuf[t_tap_t];
	//		presetBuffer[30+tr_lfo_t] = currentPreset.modulesBuf[tr_lfo_t];
	//	}
	//
	//	if(selectionMask.pv)
	//	{
	//		presetBuffer[30 + pres_lev] = currentPreset.paramData.preset_volume;
	//	}
	//
	//	if(selectionMask.att)
	//	{
	//		presetBuffer[PRESET_DATA_OFFSET + preset_att] = currentPreset.paramData.attenuator;
	//	}
	//
	//	if(selectionMask.controllers)
	//	{
	//		for(uint16_t i = 0 ; i < Controller::controllersCount ; i++)
	//		{
	//			presetBuffer[542 + i*sizeof(Controller::TController)] = currentPreset.controller[i].src;
	//			presetBuffer[542 + i*sizeof(Controller::TController) + 1] = currentPreset.controller[i].dst;
	//			presetBuffer[542 + i*sizeof(Controller::TController) + 2] = currentPreset.controller[i].minVal;
	//			presetBuffer[542 + i*sizeof(Controller::TController) + 3] = currentPreset.controller[i].maxVal;
	//		}
	//	}
	//
	//	EEPROM_WriteTempPreset(targetPresetNum);
}
