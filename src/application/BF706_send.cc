#include "appdefs.h"
#include "BF706_send.h"
#include "eepr.h"
#include "gui/gui_task.h"

extern uint32_t send_buf;
extern uint8_t presetData[];
extern uint8_t sys_para[];
extern uint16_t delay_time;
extern uint8_t cab_type;
uint8_t spi_stat = 0;

volatile uint8_t gui_fl;
volatile uint8_t contr_fl;
volatile uint8_t ext_send_fl;

void DSP_gui_set_parameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value)
{
	while((contr_fl)||(ext_send_fl));

	gui_fl = 1;
	dsp_send(module_address, parameter_address|value<<8);
	gui_fl = 0;
}

void DSP_contr_set_parameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value)
{
	while((gui_fl)||(ext_send_fl));

	contr_fl = 1;
	dsp_send(module_address, parameter_address|value<<8);
	contr_fl = 0;
}

void master_volum(uint8_t val)
{
	dsp_send(DSP_ADDRESS_PRESET_VOLUME, val);
}

void pres_volum_cont(uint8_t val)
{
	dsp_send(DSP_ADDRESS_PRESET_VOLUME, val);
}

void cab_volume(uint8_t num)
{
	dsp_send(DSP_ADDRESS_CAB, num|(presetData[vol+num]<<8));
}

void chor_par(uint8_t num)
{
	if(num==5)
		dsp_send(DSP_ADDRESS_CHORUS, num|(presetData[hpf_ch]<<8));
	else
		dsp_send(DSP_ADDRESS_CHORUS, num|(presetData[chor_volum+num]<<8));
}

void Phaser_par(uint8_t num)
{
	if(num==6)
		dsp_send(DSP_ADDRESS_PHASER, num|(presetData[hpf_ph]<<8));
	else
	{
		if(num==7)
			dsp_send(DSP_ADDRESS_PHASER, num|(presetData[phas_pos]<<8));
		else
			dsp_send(DSP_ADDRESS_PHASER, num|(presetData[phaser_vol+num]<<8));
	}
}

void DSP_reverb_param(uint8_t num)
{
	switch(num)
	{
		case 0:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[r_vol]<<8));
		break;
		case 1:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[rev_t]<<8));
		break;
		case 8:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[rev_di]<<8));
		break;
		case 9:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[r_pre]<<8));
		break;
		case 10:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[r_tail]<<8));
		break;
		default:
			dsp_send(DSP_ADDRESS_REVERB, num|(presetData[r_vol+num-1]<<8));
		break;
	}
}

void del_par(uint8_t num)
{
	switch(num)
	{
		case 0:
			dsp_send(DSP_ADDRESS_DELAY, 0|(presetData[d_vol]<<8));
		break;
		case 1:
			dsp_send(DSP_ADDRESS_DELAY, 11|((delay_time>>8)<<8));
			dsp_send(DSP_ADDRESS_DELAY, 12|((delay_time)<<8));
		break;
		case 13:
			dsp_send(DSP_ADDRESS_DELAY, 13|(presetData[d_tail]<<8));
		break;
		default:
			num--;
			dsp_send(DSP_ADDRESS_DELAY, num|(presetData[d_vol+num]<<8));
		break;
	}
}

void eq_par(uint8_t num)
{
	if(num!=12)
		dsp_send(DSP_ADDRESS_EQ, num|(presetData[eq1+num]<<8));
	else
		dsp_send(DSP_ADDRESS_EQ, num|(presetData[eq_pr_po]<<8));
}
void eq_mas_par(uint8_t num)
{
	volatile int8_t a = sys_para[121+num];
	if(num!=2)
	{
		a += 24;
		dsp_send(DSP_ADDRESS_EQ, (num+8)|a<<8);
	}
	else
	{
		dsp_send(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS|(sys_para[MASTER_EQ_FREQ_LO]<<8));
		dsp_send(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS|(sys_para[MASTER_EQ_FREQ_HI]<<8));
	}
}

void tr_param(uint8_t num)
{
	if(num<5)
	{
		switch(num)
		{
			case 0:
			case 1:
				dsp_send(DSP_ADDRESS_TREMOLO, num|(presetData[tr_vol+num]<<8));
			break;
			case 2:
				dsp_send(DSP_ADDRESS_TREMOLO, 6|(presetData[tr_lfo_t]<<8));
			break;
			default:
				num--;
				dsp_send(DSP_ADDRESS_TREMOLO, num|(presetData[tr_vol+num]<<8));
			break;
		}
	}
	else
	{
		dsp_send(DSP_ADDRESS_TREMOLO, 4|((trem_time>>8)<<8));
		dsp_send(DSP_ADDRESS_TREMOLO, 5|(trem_time<<8));
	}
}

void amp_volume(uint8_t num)
{
	dsp_send(DSP_ADDRESS_AMP, num|(presetData[am_v+num]<<8));
}

void program_ch(void)
{
	dsp_send(DSP_ADDRESS_PROGRAMM_CHANGE, 0);
}

void flanger_par(uint8_t num)
{
	if(num==6)
		dsp_send(DSP_ADDRESS_FLANGER, num|(presetData[hpf_fl]<<8));
	else
	{
		if(num==7)
			dsp_send(DSP_ADDRESS_FLANGER, num|(presetData[flan_pos]<<8));
		else
			dsp_send(DSP_ADDRESS_FLANGER, num|(presetData[fl_v+num]<<8));
	}
}

void tun_proc(uint8_t num)
{
	dsp_send(DSP_ADDRESS_TUN_PROC, num);
}

void cab_dry_mute(void)
{
	dsp_send(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[0]);
}

void Early_par(uint8_t num)
{
	dsp_send(DSP_ADDRESS_EARLY_REFLECTIONS, num|(presetData[early_vol+num]<<8));
}

void master_volu(uint8_t val)
{
	dsp_send(DSP_ADDRESS_MASTER, val);
}

void master_volum_contr(uint8_t val)
{
	dsp_send(21, val);
}

void eq_band_par(uint8_t num)
{
	dsp_send(DSP_ADDRESS_EQ_BAND, num|(presetData[f1+num]<<8));
}

void amp_imp_load(void)
{
	dsp_send(DSP_ADDRESS_AMP_TYPE, presetData[am_mode]);
}

void dsp_run(uint8_t val)
{
	dsp_send(DSP_ADDRESS_DSP_RUN, val);
}

void spdi_t(uint8_t val)
{
	dsp_send(DSP_ADDRESS_SPDIF, val);
}

void pr_par(uint8_t num)
{
	dsp_send(DSP_ADDRESS_PREAMP, num|(presetData[pre_gain+num]<<8));
}

void cab_r_by(uint8_t val)
{
	dsp_send(DSP_ADDRESS_CAB_CONFIG, val);
}

void gate_param(uint8_t val)
{
	dsp_send(DSP_ADDRESS_GATE, val|(presetData[gate_thr+val]<<8));
}

void comp_par(uint8_t val)
{
	dsp_send(DSP_ADDRESS_COMPRESSOR, val|(presetData[comp_thr+val]<<8));
}

void moog_par(uint8_t val)
{
	if(val<11)
		dsp_send(DSP_ADDRESS_RESONANCE_FILTER, val|(presetData[mog_mix+val]<<8));
	else
	{
		if(val==11)
			dsp_send(DSP_ADDRESS_RESONANCE_FILTER, 12|(presetData[mog_gen_t]<<8));
		else
		{
			if(val==12)
				dsp_send(DSP_ADDRESS_RESONANCE_FILTER, 11|(presetData[mog_ext]<<8));
			else
			{
				dsp_send(DSP_ADDRESS_RESONANCE_FILTER, 13|((moog_time>>8)<<8));
				dsp_send(DSP_ADDRESS_RESONANCE_FILTER, 14|(moog_time<<8));
			}
		}
	}
}

void dsp_mute(uint8_t val)
{
	dsp_send(DSP_ADDRESS_MUTE, val);
}

void global_temp(uint8_t val)
{
	dsp_send(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE]|(sys_para[TAP_HIGH]<<8));
}

extern uint8_t cab1_data[];
extern uint8_t cab2_data[];

void send_cab_data(uint8_t val, uint8_t num, uint8_t menu_fl)
{
	uint32_t a;
	extern bool cab_data_ready;
	if(cab_data_ready!=true&&current_menu_type!=MENU_COPY)
	{
		kgp_sdk_libc::memset(preset_temp, 0, 24576);
		preset_temp[0] = 0xff;
		preset_temp[1] = 0xff;
		preset_temp[2] = 0x7f;
	}
	while(EXTI_GetITStatus(EXTI_Line9)==RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
	{
	}
	;
	SPI_I2S_SendData(SPI2, num);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, 2);
	uint32_t cab_count;
	if(cab_type!=2)
		cab_count = 8192;
	else
		cab_count = 4096;
	for(uint32_t i = 0; i<cab_count; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9)==RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);
		if(val)
		{
			if(!menu_fl)
			{
				send_buf = preset_temp[i*3]<<8;
				send_buf |= preset_temp[i*3+1]<<16;
				send_buf |= preset_temp[i*3+2]<<24;
			}
			else
			{
				if(i<4096)
				{
					send_buf = preset_temp[i*3+1056]<<8;
					send_buf |= preset_temp[i*3+1+1056]<<16;
					send_buf |= preset_temp[i*3+2+1056]<<24;
				}
				else
				{
					a = i-4096;
					if(cab_type==2)
					{
						send_buf = preset_temp[a*3+13408]<<8;
						send_buf |= preset_temp[a*3+1+13408]<<16;
						send_buf |= preset_temp[a*3+2+13408]<<24;
					}
					else
					{
						send_buf = preset_temp[a*3+25760]<<8;
						send_buf |= preset_temp[a*3+1+25760]<<16;
						send_buf |= preset_temp[a*3+2+25760]<<24;
					}
				}
			}
		}
		else
		{
			if(i<4096)
			{
				send_buf = cab1_data[i*3]<<8;
				send_buf |= cab1_data[i*3+1]<<16;
				send_buf |= cab1_data[i*3+2]<<24;
			}
			else
			{
				a = i-4096;
				send_buf = cab2_data[a*3]<<8;
				send_buf |= cab2_data[a*3+1]<<16;
				send_buf |= cab2_data[a*3+2]<<24;
			}
		}
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf>>16);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf);
	}
	for(uint32_t i = 0; i<512; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9)==RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, 0);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		if(!menu_fl)
			SPI_I2S_SendData(SPI2, presetData[i]);
		else
			SPI_I2S_SendData(SPI2, preset_temp[i+30]);
	}
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void send_cab_data1(uint8_t val, uint8_t num)
{
	extern bool cab_data_ready;
	if(cab_data_ready!=true&&current_menu_type!=MENU_COPY)
	{
		kgp_sdk_libc::memset(preset_temp, 0, 12288);
		preset_temp[0] = 0xff;
		preset_temp[1] = 0xff;
		preset_temp[2] = 0x7f;
	}
	while(EXTI_GetITStatus(EXTI_Line9)==RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
	{
	}
	;
	SPI_I2S_SendData(SPI2, num);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, 18);
	for(int i = 0; i<4096; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9)==RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);
		if(val)
		{
			send_buf = preset_temp[i*3]<<8;
			send_buf |= preset_temp[i*3+1]<<16;
			send_buf |= preset_temp[i*3+2]<<24;
		}
		else
		{
			send_buf = cab2_data[i*3]<<8;
			send_buf |= cab2_data[i*3+1]<<16;
			send_buf |= cab2_data[i*3+2]<<24;
		}
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf>>16);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf);
	}
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void gui_send(uint8_t num, uint16_t val)
{
	while((contr_fl)||(ext_send_fl));
	gui_fl = 1;
	switch(num)
	{
		case 0:
			master_volum(val);
		break;
		case 1:
			master_volu(val);
		break;
		case 2:
			DSP_reverb_param(val);
		break;
		case 3:
			del_par(val);
		break;
		case 4:
			eq_par(val);
		break;
		case 5:
			Phaser_par(val);
		break;
		case 6:
			Early_par(val);
		break;
		case 7:
			cab_volume(val);
		break;
		case 8:
			flanger_par(val);
		break;
		case 9:
			chor_par(val);
		break;
		case 10:
			tr_param(val);
		break;
		case 11:
			amp_volume(val);
		break;
		case 12:
			program_ch();
		break;
		case 13:
			tun_proc(val);
		break;
		case 14:
			cab_dry_mute();
		break;
		case 15:/*ind_source(val);*/
		break;
		case 16:
			send_cab_data(val, 0, 0);
		break;
		case 17:
			send_cab_data1(val, 0);
		break;
		case 18:
			dsp_send(DSP_ADDRESS_MODULES_ENABLE, val);
		break;
		case 19:
			pres_volum_cont(val);
		break;
		case 20:
			master_volum_contr(val);
		break;
		case 22:
			eq_band_par(val);
		break;
		case 23:
			amp_imp_load();
		break;
		case 24:
			dsp_run(val);
		break;
		case 25:
			eq_mas_par(val);
		break;
		case 26:
			spdi_t(val);
		break;
		case 27:
			pr_par(val);
		break;
		case 28:
			cab_r_by(val);
		break;
		case 29:
			gate_param(val);
		break;
		case 30:
			comp_par(val);
		break;
		case 31:
			moog_par(val);
		break;
		case 32:
			dsp_mute(val);
		break;
		case 33:
			global_temp(val);
		break;
	}
	gui_fl = 0;
}

void contr_send(uint8_t num, uint16_t val)
{
	while((gui_fl)||(ext_send_fl));
	contr_fl = 1;
	switch(num)
	{
		case 0:
			master_volum(val);
		break;
		case 1:
			master_volu(val);
		break;
		case 2:
			DSP_reverb_param(val);
		break;
		case 3:
			del_par(val);
		break;
		case 4:
			eq_par(val);
		break;
		case 5:
			Phaser_par(val);
		break;
		case 6:
			Early_par(val);
		break;
		case 7:
			cab_volume(val);
		break;
		case 8:
			flanger_par(val);
		break;
		case 9:
			chor_par(val);
		break;
		case 10:
			tr_param(val);
		break;
		case 11:
			amp_volume(val);
		break;
		case 12:
			program_ch();
		break;
		case 13:
			tun_proc(val);
		break;
		case 14:
			cab_dry_mute();
		break;
		case 15:/*ind_source(val);*/
		break;
		case 18:
			dsp_send(DSP_ADDRESS_MODULES_ENABLE, val);
		break;
		case 19:
			pres_volum_cont(val);
		break;
		case 20:
			master_volum_contr(val);
		break;
		case 27:
			pr_par(val);
		break;
		case 29:
			gate_param(val);
		break;
		case 30:
			comp_par(val);
		break;
		case 31:
			moog_par(val);
		break;
	}
	contr_fl = 0;
}

volatile uint8_t master_volume_controller = 127;
void ext_send(uint8_t data)
{
	while((gui_fl)||(contr_fl));
	ext_send_fl = 1;
	master_volum_contr(data);
	master_volume_controller = data;
	ext_send_fl = 0;
}

