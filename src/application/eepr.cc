#include "appdefs.h"
#include "eepr.h"
#include "fs.h"

uint16_t delay_time;

volatile uint8_t prog;
volatile uint8_t prog1;
volatile uint32_t flash_adr;
volatile uint16_t adc_low;
volatile uint16_t adc_high;
volatile uint16_t adc_val;
volatile float adc_val1;
extern uint8_t cab_type;

const uint8_t stas[] =
{"STAS"};
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
uint8_t __CCM_BSS__ preset_temp[38560];
const uint32_t del_tim_init = 500;
const uint8_t imya_init[]
{"Preset        "};
const uint8_t imya_init1[]
{"Name          "};

uint8_t __CCM_BSS__ sys_para[512] =
{/*mode*/0,/*midi_ch*/0,/*cab num*/0,/*exp_type*/1,/*foot1*/0,/*foot2*/0,
/*foot3*/0,/*calibrate*/0, 0, 0xff, 0xf};

uint8_t impulse_path[512];

uint8_t __CCM_BSS__ presetData[512];
uint8_t __CCM_BSS__ presetControllers[512];
uint8_t __CCM_BSS__ presetName[15];
uint8_t __CCM_BSS__ presetComment[15];

uint8_t __CCM_BSS__ imya_t[15];
uint8_t __CCM_BSS__ imya1_t[15];
volatile uint32_t fl_st;
const uint8_t no_loaded[] = "No loaded";
char __CCM_BSS__ name_buf [64];
char __CCM_BSS__ name_buf1[64];
char __CCM_BSS__ name_buf_temp[64];
uint8_t cab_num = 0;
extern volatile uint8_t prog_sym_cur;
extern uint8_t name_run_fl;
extern emb_string full_curr_dir_path;

char fna[_MAX_LFN];

void write_sys(void)
{
	FATFS fs;
	FIL file;
	UINT f_size;
	f_mount(&fs, "1:", 1);
	f_open(&file, "1:system.pan", FA_WRITE);
	f_write(&file, sys_para, 512, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
}

void eepr_write(uint8_t nu)
{
	nu++;
	FATFS fs;
	FIL file;
	UINT f_size;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	f_open(&file, fna, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	f_write(&file, presetName, 15, &f_size);
	f_write(&file, presetComment, 15, &f_size);
	f_write(&file, presetData, 512, &f_size);
	f_write(&file, presetControllers, 512, &f_size);
	uint8_t del_t_b[2];
	del_t_b[0] = delay_time;
	del_t_b[1] = delay_time>>8;
	f_write(&file, del_t_b, 2, &f_size);
	f_write(&file, cab_data, 12288, &f_size);
	f_write(&file, name_buf, 64, &f_size);
	if(cab_type==2)
	{
		f_write(&file, cab_data1, 12288, &f_size);
		f_write(&file, name_buf1, 64, &f_size);
		f_lseek(&file, 38048);
	}
	else
	{
		f_lseek(&file, 25760);
		f_write(&file, cab_data1, 12288, &f_size);
	}
	f_write(&file, impulse_path, 512, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
}
void eepr_read_prog_data(uint8_t nu)
{
	nu++;
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_lseek(&file, 30);
		f_read(&file, presetData, 512, &f_size);
	}
	else
		for(uint16_t i = 0; i<512; i++)
			presetData[i] = prog_data_init[i];
	f_close(&file);
}

void eepr_read_prog(uint8_t nu)
{
	FSTask->Suspend();
	nu++;
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		f_read(&file, presetName, 15, &f_size);
		f_read(&file, presetComment, 15, &f_size);
		f_lseek(&file, 542);
		f_read(&file, presetControllers, 512, &f_size);
		uint8_t del_t_b[2];
		f_read(&file, del_t_b, 2, &f_size);

		extern uint8_t tempo_fl;
//		if(!tempo_fl || !sys_para[tap_typ])
//	    {
		delay_time = del_t_b[0];
		delay_time |= del_t_b[1]<<8;
//	    }
		f_read(&file, cab_data, 12288, &f_size);
		f_read(&file, name_buf, 64, &f_size);
		if(cab_type==2)
		{
			f_read(&file, cab_data1, 12288, &f_size);
			f_read(&file, name_buf1, 64, &f_size);
		}
		else
		{
			f_lseek(&file, 25760);
			if(f_size(&file)<=25760)
				kgp_sdk_libc::memset(cab_data1, 0, 12288);
			else
				f_read(&file, cab_data1, 12288, &f_size);
		}
		f_lseek(&file, 38048);
		kgp_sdk_libc::memset(impulse_path, 0, 512);
		char *tmp = new char[_MAX_LFN];
		if(!tmp)
			throw_exeption_catcher("not enother memory");
		f_read(&file, tmp, _MAX_LFN, &f_size);
		if(f_size)
		{
			kgp_sdk_libc::memcpy(impulse_path, tmp, _MAX_LFN);
			FSTask->Object().name = emb_string(tmp);
			f_read(&file, tmp, _MAX_LFN, &f_size);
			kgp_sdk_libc::memcpy(impulse_path+255, tmp, _MAX_LFN);
			FSTask->Object().startup = emb_string(tmp);

			emb_string startup = FSTask->Object().name+emb_string("/")+FSTask->Object().startup;
			if(FR_OK==f_open(&file, startup.c_str(), FA_READ))
			{
				f_close(&file);
				FSTask->SendCommand(TFsBrowser::bcStartup);
			}
		}
		delete tmp;
		prog_sym_cur = 0;
	}
	else
	{
		for(uint16_t i = 0; i<512; i++)
			presetData[i] = prog_data_init[i];
		for(uint8_t i = 0; i<15; i++)
			presetName[i] = imya_init[i];
		for(uint8_t i = 0; i<15; i++)
			presetComment[i] = imya_init1[i];
		kgp_sdk_libc::memset(cab_data, 0, 12288);
		kgp_sdk_libc::memset(cab_data1, 0, 12288);
		kgp_sdk_libc::memset(presetControllers, 0, 512);
		for(uint8_t i = 0; i<128; i++)
			presetControllers[i*4+3] = 127;
		delay_time = del_tim_init;
		name_buf[0] = name_buf1[0] = 0;
		cab_data[0] = 0xff;
		cab_data[1] = 0xff;
		cab_data[2] = 0x7f;
		if(cab_type==2)
		{
			cab_data1[0] = 0xff;
			cab_data1[1] = 0xff;
			cab_data1[2] = 0x7f;
		}
		prog_sym_cur = 1;
	}

	f_close(&file);
	f_open(&file, "1:system.pan", FA_WRITE);
	f_write(&file, sys_para, 512, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
	presetName[14] = 0;
	presetComment[14] = 0;
	name_buf[63] = name_buf1[63] = 0;
	FSTask->Resume();
}
void read_prog_temp(uint8_t nu)
{
	nu++;
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;
	uint16_t pres_po = 0;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	fs_res = f_open(&file, fna, FA_READ);
	if(fs_res==FR_OK)
	{
		if(f_size(&file)<=25760)
		{
			f_read(&file, preset_temp, 25760, &f_size);
			for(uint16_t i = 0; i<12800; i++)
				preset_temp[i+25760] = 0;
		}
		else
		{
			if(f_size(&file)<=38048)
			{
				f_read(&file, preset_temp, 38048, &f_size);
				for(uint16_t i = 0; i<512; i++)
					preset_temp[i+38048] = 0;
			}
			else
				f_read(&file, preset_temp, 38560, &f_size);
		}
		prog_sym_cur = 0;
	}
	else
	{
		for(uint8_t i = 0; i<15; i++)
			preset_temp[pres_po++] = imya_init[i];
		for(uint8_t i = 0; i<15; i++)
			preset_temp[pres_po++] = imya_init1[i];
		for(uint16_t i = 0; i<512; i++)
			preset_temp[pres_po++] = prog_data_init[i];
		for(uint16_t i = 0; i<512; i++)
			preset_temp[pres_po++] = 0;
		preset_temp[pres_po++] = del_tim_init;
		preset_temp[pres_po++] = del_tim_init>>8;
		preset_temp[pres_po++] = 0xff;
		preset_temp[pres_po++] = 0xff;
		preset_temp[pres_po++] = 0x7f;
		for(uint16_t i = 0; i<12285; i++)
			preset_temp[pres_po++] = 0;
		for(uint16_t i = 0; i<64; i++)
			preset_temp[pres_po++] = 0;
		preset_temp[pres_po++] = 0xff;
		preset_temp[pres_po++] = 0xff;
		preset_temp[pres_po++] = 0x7f;
		for(uint16_t i = 0; i<12285; i++)
			preset_temp[pres_po++] = 0;
		for(uint16_t i = 0; i<64; i++)
			preset_temp[pres_po++] = 0;
		for(uint16_t i = 0; i<12288; i++)
			preset_temp[pres_po++] = 0;
		for(uint16_t i = 0; i<512; i++)
			preset_temp[pres_po++] = 0;
		prog_sym_cur = 1;
	}
	f_close(&file);
	f_mount(0, "1:", 0);
}
void write_prog_temp(uint8_t nu)
{
	nu++;
	FATFS fs;
	FIL file;
	UINT f_size;
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_mount(&fs, "1:", 1);
	f_open(&file, fna, FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	f_write(&file, preset_temp, 19024, &f_size);
	f_write(&file, preset_temp+19024, 19536, &f_size);
	f_close(&file);
	f_mount(0, "1:", 0);
}
// read a  header
void eepr_read_imya(uint8_t nu)
{
	nu++;
	FRESULT fs_res;
	FATFS fs;
	FIL file;
	UINT f_size;
	f_mount(&fs, "1:", 1);
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	fs_res = f_open(&file, fna, FA_READ);
	//name_run_fl = 0;
	if(fs_res==FR_OK)
	{
		f_lseek(&file, 13344);
		f_read(&file, imya_t, 1, &f_size);
		f_lseek(&file, 25696);
		f_read(&file, imya1_t, 1, &f_size);
		if((imya_t[0])||(imya1_t[0]))
			prog_sym_cur = 0;
		else
			prog_sym_cur = 1;
		f_lseek(&file, 0);
		f_read(&file, imya_t, 15, &f_size);
		f_read(&file, imya1_t, 15, &f_size);
		prog_sym_cur = 0;
	}
	else
	{
		for(uint8_t i = 0; i<15; i++)
			imya_t[i] = imya_init[i];
		for(uint8_t i = 0; i<15; i++)
			imya1_t[i] = imya_init1[i];
		prog_sym_cur = 1;
	}
	f_close(&file);
	f_mount(0, "1:", 0);
}
void preset_erase(uint8_t nu)
{
	nu++;
	FATFS fs;
	f_mount(&fs, "1:", 1);
	char *fna = new char[_MAX_LFN];
	if(nu<10)
		ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)nu);
	else
		ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)nu);
	f_unlink(fna);
	f_mount(0, "1:", 0);
}
void load_mass_imp(void)
{
	uint32_t buf;
	FATFS fs;
	FIL file;
	UINT f_size;
	f_mount(&fs, "1:", 1);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	for(uint8_t i = 1; i<100; i++)
	{
		if(i<10)
			ksprintf(fna, "1:PRESETS/0%d_preset.pan", (uint32_t)i);
		else
			ksprintf(fna, "1:PRESETS/%d_preset.pan", (uint32_t)i);
		if(f_open(&file, fna, FA_READ)==FR_OK)
		{
			while(EXTI_GetITStatus(EXTI_Line9)==RESET);
			EXTI_ClearITPendingBit (EXTI_Line9);
			while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
			{
			}
			;
			SPI_I2S_SendData(SPI2, i-1);
			while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
			{
			}
			;
			SPI_I2S_SendData(SPI2, i-1);
			f_lseek(&file, 1056);
			f_read(&file, cab_data, 12288, &f_size);
			for(uint32_t ii = 0; ii<4096; ii++)
			{
				while(EXTI_GetITStatus(EXTI_Line9)==RESET);
				EXTI_ClearITPendingBit(EXTI_Line9);
				buf = cab_data[ii*3]<<8;
				buf |= cab_data[ii*3+1]<<16;
				buf |= cab_data[ii*3+2]<<24;
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf>>16);
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf);
			}
			if(cab_type!=2)
			{
				if(f_size(&file)<=25760)
				{
					kgp_sdk_libc::memset(cab_data1, 0, 12288);
				}
				else
				{
					f_lseek(&file, 25760);
					f_read(&file, cab_data1, 12288, &f_size);
				}
			}
			else
			{
				f_lseek(&file, 13408);
				f_read(&file, cab_data1, 12288, &f_size);
			}
			for(uint32_t ii = 0; ii<4096; ii++)
			{
				while(EXTI_GetITStatus(EXTI_Line9)==RESET);
				EXTI_ClearITPendingBit(EXTI_Line9);
				buf = cab_data1[ii*3]<<8;
				buf |= cab_data1[ii*3+1]<<16;
				buf |= cab_data1[ii*3+2]<<24;
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf>>16);
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf);
			}
			f_lseek(&file, 30);
			f_read(&file, presetData, 512, &f_size);
			f_lseek(&file, 1054);
			f_read(&file, presetData+147, 2, &f_size);
			for(uint32_t ii = 0; ii<512; ii++)
			{
				while(EXTI_GetITStatus(EXTI_Line9)==RESET);
				EXTI_ClearITPendingBit(EXTI_Line9);
				buf = presetData[ii];
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf>>16);
				while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
				{
				}
				;
				SPI_I2S_SendData(SPI2, buf);
			}
			f_close(&file);
		}
	}
	while(EXTI_GetITStatus(EXTI_Line9)==RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
	{
	}
	;
	SPI_I2S_SendData(SPI2, 100);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, 100);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
	f_mount(0, "1:", 0);
}
