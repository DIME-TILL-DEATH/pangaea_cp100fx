#include "midi_send.h"
#include "cc.h"
#include "display.h"
#include "enc.h"
#include "gui_task.h"
#include "cs.h"
#include "eepr.h"
#include "BF706_send.h"
#include "allFonts.h"

#include "system.h"

#include "preset.h"

TMidiSendTask *MidiSendTask;

inline void uart_send(uint8_t val)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, val);
}
inline void send_bank(void)
{
	uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
	uart_send(0x0);
	uart_send(0x0);
	uart_send(0x20);
	uart_send(0x0);
}

TMidiSendTask::TMidiSendTask() :
		TTask()
{
}
volatile uint8_t pc_mute_fl;
volatile uint8_t midi_f1 = 0;
volatile uint8_t midi_f2;
volatile uint8_t us_buf;
volatile uint8_t ex_fl = 0;
volatile uint8_t midi_in_buf[256];
volatile uint8_t midi_in_po = 0;
volatile uint8_t int_contr_buf[256];
volatile uint8_t int_contr_po = 0;
volatile uint8_t trans_midi_po = 0;
volatile uint8_t trans_contr_po = 0;
volatile uint8_t trans_po = 0;
volatile uint16_t trans_po1; //?
volatile uint8_t contr_hader = 0;
volatile uint8_t pc_hader = 0;
volatile uint8_t contr_cont = 0;
volatile uint8_t program_change_midi = 0;
volatile uint8_t key_midi[3];
volatile uint8_t key_midi1[3];
volatile uint8_t pc_in_fl = 0;
volatile uint8_t pc_in = 0;
volatile uint8_t old_pc = 0xff;

void TMidiSendTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 8, 0);

	volatile uint8_t data[2];

	while(1)
	{
		sem->Take(portMAX_DELAY);
		trans_po = 0;
		uint8_t tmp;

		while((trans_midi_po != midi_in_po) || (trans_contr_po != int_contr_po))
		{
//---------------------------------------------Midi event----------------------------------------
			if(trans_midi_po != midi_in_po)
			{
				trans_po++;
				if(trans_po1 < trans_po)
					trans_po1 = trans_po;

				tmp = midi_in_buf[trans_midi_po++];
				if(tmp & 0x80)
				{
					data[0] = tmp;
					if(tmp != (sys_para[System::MIDI_CHANNEL] | 0xc0))
						uart_send(tmp);
					else
						pc_in_fl = 1;
					if(((tmp & 0xf0) != 0xc0) && ((tmp & 0xf0) != 0xd0))
					{
						contr_hader = 1;
					}
					else
						contr_hader = 0;
				}
				else
				{
					if(data[0] != (sys_para[System::MIDI_CHANNEL] | 0xc0))
						uart_send(tmp);

					if(contr_hader)
					{
						if(!contr_cont)
						{
							data[1] = tmp;
							contr_cont = 1;
						}
						else
						{
							if(((data[0] & 0xf) == sys_para[System::MIDI_CHANNEL]) && ((data[0] & 0xf0) == 0xb0))
							{
								midi_b[1] = data[1];
								midi_b[2] = tmp;
								mid_fl = 1;
								if((midi_b[1] == (sys_para[System::TUNER_EXTERNAL] & 0x7f))
										&& (sys_para[System::TUNER_EXTERNAL] & 0x80))
								{
									if(currentMenu->menuType() == MENU_TUNER)
									{
										k_tuner = 1;
										CSTask->Give();
									}
								}
								else
								{
									CCTask->Give();
									//  Через menu->refresh()
//									if((current_menu_type==MENU_CONTROLLERS&&(par_num<3))||(current_menu_type==MENU_TUNER_EXT))
//									{
//										uint8_t midi_in_cc[] = "midi in CC#->";
//										DisplayTask->StringOut(20, 3, Font::fntSystem, 0, (uint8_t*)midi_in_cc);
//										DisplayTask->ParamIndicNum(100, 3, midi_b[1]);
//									}
								}
							}
							contr_cont = 0;
						}
					}
					else
					{
						if(((data[0] & 0xf) == sys_para[System::MIDI_CHANNEL]) && ((data[0] & 0xf0) == 0xc0))
						{
							pc_in = tmp;
							if(currentMenu->menuType() == MENU_MAIN)
							{
								MainMenu *mainMenu = static_cast<MainMenu*>(currentMenu);
								mainMenu->presetChoose(sys_para[tmp + 128] % 100);
								mainMenu->presetConfirm();

								midi_f1 = 1;
								CSTask->Give();
							}
						}
					}
				}
			}
//-------------------------------------------Int event------------------------------------------
			if(trans_contr_po != int_contr_po)
			{
				if(!contr_cont)
				{
					uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
					uart_send(sys_para[System::EXPR_CCN] - 1);
					uart_send(int_contr_buf[trans_contr_po++]);
					if((data[0] & 0xf0) != 0xb0)
						uart_send(data[0]);
				}
			}
		}
//-------------------------------------------Prog ch midi send------------------------------------
		if(program_change_midi)
		{
			if(!contr_cont)
			{
				if(pc_in_fl)
					pc_in_fl = 0;
				else
				{
					send_bank();
				}

				uart_send(0xc0 | sys_para[System::MIDI_CHANNEL]);
				switch(currentPreset.pcOut)
				{
					case Preset::PcOutType::MidiIn:
						if(midi_f1)
							uart_send(pc_in);
						else
							uart_send(currentPresetNumber);
					break;

					case Preset::PcOutType::Map:
						uart_send(sys_para[currentPresetNumber + 128]);
					break;

					case Preset::PcOutType::Set:
						uart_send(currentPreset.set & 0x7f);
					break;
				}

				if((data[0] & 0xf0) != 0xc0)
					uart_send(data[0]);

				program_change_midi = 0;
				midi_f1 = 0;
			}
		}
//-------------------------------------------Foot1-----------------------------------------------
		if(key_midi[0])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW1_CTRL_PRESS_CC] - 1);
				if(key_midi[0] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi[0] = 0;
			}
		}
//-------------------------------------------Foot2-----------------------------------------------
		if(key_midi[1])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW2_CTRL_PRESS_CC] - 1);
				if(key_midi[1] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi[1] = 0;
			}
		}
//-------------------------------------------Foot3-----------------------------------------------
		if(key_midi[2])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW3_CTRL_PRESS_CC] - 1);
				if(key_midi[2] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi[2] = 0;
			}
		}
//--------------------------------------------Foot11----------------------------------------------
		if(key_midi1[0])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW1_CTRL_HOLD_CC] - 1);
				if(key_midi1[0] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi1[0] = 0;
			}
		}
//--------------------------------------------Foot21----------------------------------------------
		if(key_midi1[1])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW2_CTRL_HOLD_CC] - 1);
				if(key_midi1[1] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi1[1] = 0;
			}
		}
//--------------------------------------------Foot31----------------------------------------------
		if(key_midi1[2])
		{
			if(!contr_cont)
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::FSW3_CTRL_HOLD_CC] - 1);
				if(key_midi1[2] == 2)
					uart_send(127);
				else
					uart_send(0);
				if((data[0] & 0xf0) != 0xb0)
					uart_send(data[0]);
				key_midi1[2] = 0;
			}
		}
	}
}

volatile uint16_t midi_clk_buf[64];
uint8_t midi_clk_po;

extern "C" void USART1_IRQHandler()
{
	us_buf = USART1->DR;
	if(us_buf == 0xf8)
	{
		TIM14->CR1 &= (uint16_t)~TIM_CR1_CEN;
		midi_clk_buf[midi_clk_po++] = TIM14->CNT;
		midi_clk_po &= 0x3f;
		TIM14->CNT = 0;
		TIM14->CR1 |= TIM_CR1_CEN;
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	if(!ex_fl)
	{
		if((us_buf & 0xf0) != 0xf0)
		{
			midi_in_buf[midi_in_po++] = us_buf;
			MidiSendTask->Give();
		}
		else
		{
			if(us_buf == 0xf0)
				ex_fl = 1;
			while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
			USART_SendData(USART1, us_buf);
		}
	}
	else
	{
		if(us_buf == 0xf7)
			ex_fl = 0;
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, us_buf);
	}
}
uint16_t mediann(uint16_t *array, int length)  // массив и его длина
{
	uint16_t slit = length / 2;
	for(uint16_t i = 0; i < length; i++)
	{
		uint16_t s1 = 0, s2 = 0;
		uint16_t val = array[i];
		for(int j = 0; j < length; j++)
		{
			if(array[j] < val)
			{
				if(++s1 > slit)
					goto aaa;
			}
			else if(array[j] > val)
			{
				if(++s2 > slit)
					goto aaa;
			}
		}
		return val;
aaa:	;
	}
	return 0;  // чистая формальность, досюда исполнение никогда не доходит
}
volatile uint16_t adc_bu;
volatile uint16_t adc_bu_cont;
volatile uint16_t adc_bu1[2];
volatile uint16_t adc_bu2;
uint16_t adc_buff[32];
volatile uint8_t adc_po;
volatile uint8_t adc_point = 0;
volatile uint8_t adc_inv_fl = 0;
void adc_proc(void)
{
	if(sys_para[System::EXPR_CCN])
	{
		int_contr_buf[int_contr_po++] = adc_bu2;
		MidiSendTask->Give();
	}
	if((sys_para[System::EXPR_TYPE] & 0x7f) < 3)
	{
		if(pc_mute_fl)
			ext_send(adc_bu2);
	}
	else
	{
		ext_data = adc_bu2;
		ext_sourc = 1;
		ext_fl = 1;
		CCTask->Give();
	}
	adc_bu1[1] = adc_bu1[0];
	adc_bu1[0] = adc_bu2;
}
extern "C" void ADC_IRQHandler()
{
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	adc_bu = ADC1->DR;
	adc_bu_cont = adc_bu;
	//----------------------------------------------------------------------------------
	int a = adc_bu_cont - adc_low;
	if(a < 0)
		a = 0;
	adc_bu_cont = a * adc_val1;
	if(adc_bu_cont > 127)
		adc_bu_cont = 127;
	if(adc_inv_fl)
		adc_bu_cont = 127 - adc_bu_cont;

	adc_buff[adc_po++] = adc_bu_cont;
	if(adc_po == 32)
		adc_po = 0;
	adc_bu2 = mediann(adc_buff, 32);

	if(adc_bu2 != adc_bu1[0])
	{
		if((adc_bu2 != adc_bu1[1]) && (sys_para[3] & 0x80))
		{
			if(adc_point++ == 100)
			{
				adc_point = 0;
				adc_proc();
			}
		}
	}
	if((sys_para[3] & 1))
		ADC_RegularChannelConfig(ADC1, 10, 1, ADC_SampleTime_480Cycles);
	else
		ADC_RegularChannelConfig(ADC1, 11, 1, ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv (ADC1);
}
void send_midi_temp(uint16_t *c)
{
	tap_global = *c * 48;
	delay_time = *c;

	DSP_ContrSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, *c >> 8);
	DSP_ContrSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, *c);

	DSP_ContrSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, *c >> 8);
	DSP_ContrSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, *c);
}
