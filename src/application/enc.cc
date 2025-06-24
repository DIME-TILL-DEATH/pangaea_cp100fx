#include "appdefs.h"
#include "enc.h"
#include "eepr.h"
#include "BF706_send.h"
#include "cs.h"
#include "cc.h"
#include "fs.h"
#include "display.h"
#include "sd_test.h"
#include "midi_send.h"
#include "fonts/allFonts.h"
#include "gui/gui_task.h"

void start_usb();

volatile uint8_t encoder_flag;
volatile uint8_t k_tuner;
volatile uint8_t contr_kn[3];
volatile uint8_t contr_kn1[3];
volatile uint8_t contr_pr[3];
volatile uint8_t usb_flag = 0;
extern uint8_t tim5_fl;
extern volatile uint8_t prog_sym_cur;

uint8_t k_up = 0;
uint8_t k_down = 0;
uint8_t k_att = 0;
uint8_t k_sys = 0;
uint8_t k_master = 0;
uint8_t k_master_eq = 0;
volatile uint16_t key_reg_in;
volatile uint8_t key_reg_out[2] =
{0, 0};
volatile uint16_t key_reg;
volatile uint8_t num_key_prog;

int16_t enc_speed_inc(int16_t data, int16_t max)
{
	TIM_Cmd(TIM6, DISABLE);
	if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update))
		data = data+1;
	else
	{
		if(TIM_GetCounter(TIM6)>0x3fff)
			data += 1;
		else
		{
			if(TIM_GetCounter(TIM6)>0x1fff)
			{
				if(data<(max-1))
					data += 2;
				else
					data += 1;
			}
			else
			{
				if(TIM_GetCounter(TIM6)>0xfff)
				{
					if(data<(max-3))
						data += 4;
					else
						data += 1;
				}
				else
				{
					if(TIM_GetCounter(TIM6)>0x7ff)
					{
						if(data<(max-7))
							data += 8;
						else
							data += 1;
					}
					else
					{
						if(data<(max-49))
							data += 50;
						else
							data += 1;
					}
				}
			}
		}
	}
	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
	return data;
}

uint16_t enc_speed_dec(int16_t data, int16_t min)
{
	TIM_Cmd(TIM6, DISABLE);
	if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update))
		data -= 1;
	else
	{
		if(TIM_GetCounter(TIM6)>0x3fff)
			data -= 1;
		else
		{
			if(TIM_GetCounter(TIM6)>0x1fff)
			{
				if(data>(min+1))
					data -= 2;
				else
					data -= 1;
			}
			else
			{
				if(TIM_GetCounter(TIM6)>0xfff)
				{
					if(data>(min+3))
						data -= 4;
					else
						data -= 1;
				}
				else
				{
					if(TIM_GetCounter(TIM6)>0x7ff)
					{
						if(data>(min+7))
							data -= 8;
						else
							data -= 1;
					}
					else
					{
						if(data>(min+49))
							data -= 50;
						else
							data -= 1;
					}
				}
			}
		}
	}
	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
	return data;
}

TENCTask *ENCTask;
//------------------------------------------------------------------------------
TENCTask::TENCTask() :
		TTask()
{

}
void tim_start(uint16_t del)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_SetCounter(TIM3, del);
	TIM_Cmd(TIM3, ENABLE);
}
void foot_run(uint8_t num)
{
	if(tuner_use)
	{
		k_up = 1;
		CSTask->Give();
	}
	else
	{
		switch(sys_para[fs1+num])
		{
			case 0:
				if(currentMenu->menuType()==MENU_MAIN)
				{
					switch(num)
					{
						case 0:
							if((sys_para[fs2]&&!sys_para[SWAP_SWITCH])||(sys_para[fs3]&&sys_para[SWAP_SWITCH]))
							{
								if(prog1==0)
									prog1 = 98;
								else
									prog1 -= 1;
								encoder_knob_pressed = 1;
								prog_old = prog = prog1;
							}
							else
							{
								encoder_state_updated = 1;
								encoder_state = 1;
							}
							CSTask->Give();
						break;
						case 1:
							if(!sys_para[SWAP_SWITCH])
							{
								encoder_knob_pressed = 1;
								CSTask->Give();
							}
							else
							{
								if((sys_para[fs2]&&!sys_para[SWAP_SWITCH])||(sys_para[fs3]&&sys_para[SWAP_SWITCH]))
								{
									if(prog1==98)
										prog1 = 0;
									else
										prog1 += 1;
									encoder_knob_pressed = 1;
									prog_old = prog = prog1;
								}
								else
								{
									encoder_state_updated = 1;
									encoder_state = 2;
								}
								CSTask->Give();
							}
						break;
						case 2:
							if(!sys_para[SWAP_SWITCH])
							{
								if((sys_para[fs2]&&!sys_para[SWAP_SWITCH])||(sys_para[fs3]&&sys_para[SWAP_SWITCH]))
								{
									if(prog1==98)
										prog1 = 0;
									else
										prog1 += 1;
									encoder_knob_pressed = 1;
									prog_old = prog = prog1;
								}
								else
								{
									encoder_state_updated = 1;
									encoder_state = 2;
								}
								CSTask->Give();
							}
							else
							{
								encoder_knob_pressed = 1;
								CSTask->Give();
							}
						break;
					}
				}
			break;
			case 1:
				if(!contr_kn[num])
				{
					contr_kn[num] = presetData[fo1+num] = 1;
					ext_data = 127;
				}
				else
					ext_data = contr_kn[num] = presetData[fo1+num] = 0;
				if(currentMenu->menuType()==MENU_MAIN)
					DisplayTask->IndFoot(num, contr_kn[num]);
				if(sys_para[k1_cc+num])
				{
					MidiSendTask->key_midi_start(num, contr_kn[num]+1);
					MidiSendTask->Give();
				}
				ext_sourc = num+2;
				ext_fl = 1;
				CCTask->Give();
			break;
			case 2:
				k_tuner = 1;
				CSTask->Give();
			break;
			default:
				if(currentMenu->menuType()==MENU_MAIN)
				{
					if(num_key_prog==num)
						contr_pr[num]++;
					if(contr_pr[num]>(sys_para[fs1+num]-3))
						contr_pr[num] = 0;
					prog1 = sys_para[pr11+num*4+contr_pr[num]];
					num_key_prog = num;
					if(sys_para[fs2])
					{
						if(!sys_para[fsm2])
						{
							prog_old = prog = prog1;
							encoder_knob_pressed = 1;
						}
						else
						{
							if(sys_para[fs21])
							{
								prog_old = prog = prog1;
								encoder_knob_pressed = 1;
							}
							else
							{
								prog_old = prog = --prog1;
								encoder_state_updated = 1;
								encoder_state = 2;
							}
						}
					}
					else
					{
						prog_old = prog = --prog1;
						encoder_state_updated = 1;
						encoder_state = 2;
					}
					CSTask->Give();
				}
			break;
		}
	}
}
void foot_run1(uint8_t num)
{
	if(tuner_use)
	{
		k_up = 1;
		CSTask->Give();
	}
	else
	{
		switch(sys_para[fs11+num])
		{
			case 0:
				if(currentMenu->menuType()==MENU_MAIN)
				{
					switch(num)
					{
						case 0:
							if((sys_para[fs21]&&!sys_para[SWAP_SWITCH])||(sys_para[fs31]&&sys_para[SWAP_SWITCH]))
							{
								if(prog1==0)
									prog1 = 98;
								else
									prog1 -= 1;
								encoder_knob_pressed = 1;
								prog_old = prog = prog1;
							}
							else
							{
								encoder_state_updated = 1;
								encoder_state = 1;
							}
							CSTask->Give();
						break;
						case 1:
							if(!sys_para[SWAP_SWITCH])
							{
								encoder_knob_pressed = 1;
								CSTask->Give();
							}
							else
							{
								if((sys_para[fs21]&&!sys_para[SWAP_SWITCH])||(sys_para[fs31]&&sys_para[SWAP_SWITCH]))
								{
									if(prog1==98)
										prog1 = 0;
									else
										prog1 += 1;
									encoder_knob_pressed = 1;
									prog_old = prog = prog1;
								}
								else
								{
									encoder_state_updated = 1;
									encoder_state = 2;
								}
								CSTask->Give();
							}
						break;
						case 2:
							if(!sys_para[SWAP_SWITCH])
							{
								if((sys_para[fs21]&&!sys_para[SWAP_SWITCH])||(sys_para[fs31]&&sys_para[SWAP_SWITCH]))
								{
									if(prog1==98)
										prog1 = 0;
									else
										prog1 += 1;
									encoder_knob_pressed = 1;
									prog_old = prog = prog1;
								}
								else
								{
									encoder_state_updated = 1;
									encoder_state = 2;
								}
								CSTask->Give();
							}
							else
							{
								encoder_knob_pressed = 1;
								CSTask->Give();
							}
						break;
					}
				}
			break;
			case 1:
				if(!contr_kn1[num])
				{
					contr_kn1[num] = presetData[fo11+num] = 1;
					ext_data = 127;
				}
				else
					ext_data = contr_kn1[num] = presetData[fo11+num] = 0;
				if(currentMenu->menuType()==MENU_MAIN)
					DisplayTask->IndFoot(num, contr_kn1[num]);
				if(sys_para[k11_cc+num])
				{
					MidiSendTask->key_midi_start1(num, contr_kn1[num]+1);
					MidiSendTask->Give();
				}
				ext_sourc = sys_para[k11_cc+num]+4;
				ext_fl = 1;
				CCTask->Give();
			break;
			case 2:
				k_tuner = 1;
				CSTask->Give();
			break;
			default:
				if(currentMenu->menuType()==MENU_MAIN)
				{
					if(contr_pr[num]>(sys_para[fs11+num]-3))
						contr_pr[num] = 0;
					prog1 = sys_para[pr111+num*4+contr_pr[num]++];
					if(sys_para[fs21]) // && !sys_para[fsm2]
					{
						if(!sys_para[fsm2])
						{
							prog_old = prog = prog1;
							encoder_knob_pressed = 1;
						}
						else
						{
							if(sys_para[fs2])
							{
								prog_old = prog = prog1;
								encoder_knob_pressed = 1;
							}
							else
							{
								prog_old = prog = --prog1;
								encoder_state_updated = 1;
								encoder_state = 2;
							}
						}
					}
					else
					{
						prog_old = prog = --prog1;
						encoder_state_updated = 1;
						encoder_state = 2;
					}
					CSTask->Give();
				}
			break;
		}
	}
}
uint8_t kn2_in_fl;
uint8_t fsw1_in_fl;
uint8_t fsw2_in_fl;
uint8_t fsw3_in_fl;
uint8_t fsw1_in_fl1;
uint8_t fsw2_in_fl1;
uint8_t fsw3_in_fl1;
uint8_t tim3_end_fl = 0;
uint8_t name_temp_buf[22];
uint8_t name_temp_po = 0;
uint32_t name_temp_po_time = 0;
uint8_t name_run_fl = 0;
volatile uint8_t enc_run = 0;
volatile uint32_t reset_disp_count = 0;

//volatile uint32_t name_temp_po_time1 = 150000;
void ADC_process();

void TENCTask::Code()
{
	while(!enc_run);
	TIM_Cmd(TIM3, ENABLE);
	while(1)
	{
		if((key_reg!=31212)&&(!kn2_in_fl)&&(!fsw1_in_fl)&&(!fsw2_in_fl)&&(!fsw3_in_fl))
		{
			tim_start(0xf700);
			switch(key_reg)
			{
				case 14828:
					k_up = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31204:
					k_down = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 23020:
					k_att = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31148:
					k_sys = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31180:
					k_tuner = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 27116:
					k_master = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 29164:
					k_master_eq = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 30956:
					fsw1_in_fl = 1;
					if(!sys_para[fsm1])
						foot_run(0);
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[foot_sp]*(8000.0f/127.0f)+55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //--------------------DOWN-----------------
				case 31084:
					fsw2_in_fl = 1;
					if(!sys_para[fsm2])
						foot_run(1);
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[foot_sp]*(8000.0f/127.0f)+55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //----------------CONFIRM----------
				case 31208:
					fsw3_in_fl = 1;
					if(!sys_para[fsm3])
						foot_run(2);
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[foot_sp]*(8000.0f/127.0f)+55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //--------------------UP-----------------
			}
			tim5_fl = 1;
		}
		if(key_reg==31212)
		{
			if(sys_para[fsm1])
			{
				if(fsw1_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						foot_run(0);
					tim_start(0xf700);
					fsw1_in_fl = 0;
				}
			}
			if(sys_para[fsm2])
			{
				if(fsw2_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						foot_run(1);
					tim_start(0xf700);
					fsw2_in_fl = 0;
				}
			}
			if(sys_para[fsm3])
			{
				if(fsw3_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						foot_run(2);
					tim_start(0xf700);
					fsw3_in_fl = 0;
				}
			}
		}
		if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
		{
			//TIM_ClearFlag(TIM3,TIM_FLAG_Update);
			if(key_reg==31212)
				kn2_in_fl = fsw1_in_fl = fsw2_in_fl = fsw3_in_fl = tim3_end_fl = 0;
		}
//---------------------------------------------Run string-------------------------------------
		StringOutParam* runningString = currentMenu->getRunningString();
		if(runningString) runningString->task();

//----------------------------------------------------LED FX-----------------------------------------------
		uint8_t a = 0;
		for(uint8_t i = 0; i<14; i++)
			if(i!=0&&i!=1&&i!=10)
				a += presetData[i];
		if(a)
			GPIO_SetBits(GPIOB, GPIO_Pin_14);
		else
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
//----------------------------------------------------Test SD------------------------------------------------
		if(EXTI_GetITStatus (EXTI_Line8))
		{
			EXTI_ClearITPendingBit(EXTI_Line8);
			SD_TESTTask->Give();
		}
//----------------------------------------------------Test USB-----------------------------------
		if(usb_flag==0)
		{
			if(GPIOA->IDR&GPIO_Pin_9)
			{
				void send_codec(uint16_t data);
				if(usb_connect_type==usb_connect_type_t::msc)
				{
					send_codec(0xa102);
					gui_send(13, 0);
				}
				usb_flag = 1;
				start_usb();
			}
		}
		else
		{
			if(!(GPIOA->IDR&GPIO_Pin_9))
			{
				if(usb_connect_type==usb_connect_type_t::msc)
					NVIC_SystemReset();
				else
					usb_flag = 2;
			}

		}
//--------------------------------------------------------------------------------------------
	}
}
extern "C" void DMA1_Stream5_IRQHandler()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	key_reg = key_reg_in&0x7fff;
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

extern "C" void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	tim3_end_fl = 1;
	if(fsw1_in_fl||fsw2_in_fl||fsw3_in_fl)
	{
		if(fsw1_in_fl)
		{
			fsw1_in_fl1 = 1;
			fsw1_in_fl = 0;
			foot_run1(0);
			fsw1_in_fl1 = 0;
		}
		if(fsw2_in_fl)
		{
			fsw2_in_fl1 = 1;
			fsw2_in_fl = 0;
			foot_run1(1);
			fsw2_in_fl1 = 0;
		}
		if(fsw3_in_fl)
		{
			fsw3_in_fl1 = 1;
			fsw3_in_fl = 0;
			foot_run1(2);
			fsw3_in_fl1 = 0;
		}
	}
}
uint16_t mediann_tap(uint16_t *array, int length)  // массив и его длина
{
	uint16_t slit = length/2;
	for(uint16_t i = 0; i<length; i++)
	{
		uint16_t s1 = 0, s2 = 0;
		uint16_t val = array[i];
		for(int j = 0; j<length; j++)
		{
			if(array[j]<val)
			{
				if(++s1>slit)
					goto aaa;
			}
			else if(array[j]>val)
			{
				if(++s2>slit)
					goto aaa;
			}
		}
		return val;
aaa:	;
	}
	return 0;  // чистая формальность, досюда исполнение никогда не доходит
}
