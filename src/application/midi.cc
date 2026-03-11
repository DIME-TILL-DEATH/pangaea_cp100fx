#include "midi.h"

#include "cc.h"
#include "display.h"
#include "enc.h"
#include "gui_task.h"
#include "cs.h"
#include "eepr.h"
#include "BF706_send.h"
#include "allFonts.h"

#include "tunermenu.h"
#include "tunerextmenu.h"
#include "controllersmenu.h"

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
//volatile uint8_t midi_f2;
volatile uint8_t us_buf;

volatile uint8_t midi_in_buf[TMidiSendTask::midiInBufferSize];
volatile uint8_t midi_buf_wr_pos = 0;
volatile uint8_t midi_buf_rd_pos = 0;

volatile uint8_t int_contr_buf[256];
volatile uint8_t int_contr_po = 0;

volatile uint8_t trans_contr_po = 0;
volatile uint8_t contr_cont = 0;
volatile uint8_t program_change_midi = 0;
volatile uint8_t key_midi[3];
volatile uint8_t key_midi1[3];
volatile uint8_t pc_in_fl = 0;
volatile uint8_t pc_in = 0;

void TMidiSendTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 8, 0);

//	volatile uint8_t data[2] = {0, 0};

	while(1)
	{
		sem->Take(portMAX_DELAY);

		while(midi_buf_rd_pos != midi_buf_wr_pos)
		{
//---------------------------------------------Midi event----------------------------------------
			if(midi_buf_rd_pos != midi_buf_wr_pos)
			{
				uint8_t recievedByte = midi_in_buf[midi_buf_rd_pos];
				midi_buf_rd_pos++;
				if(midi_buf_rd_pos == midiInBufferSize) midi_buf_rd_pos = 0;
				uart_send(recievedByte);

				if(recievedByte & 0x80)
					midiState = PROCESS_STATUS_BYTE;

				switch(midiState)
				{
					case PROCESS_STATUS_BYTE:
					{
						statusByte = recievedByte & 0xF0;
						rcvChannel = recievedByte & 0x0F;
						if((statusByte == Midi::MIDI_STATUS_PC || statusByte == Midi::MIDI_STATUS_CC)
								&& rcvChannel == sys_para[System::MIDI_CHANNEL])
						{
							midiState = WAIT_BYTE1;
						}
						break;
					}
					case WAIT_BYTE1:
					{
						dataByte[0] = recievedByte;

						if(statusByte == Midi::MIDI_STATUS_PC)
						{
							pc_in = dataByte[0];
							if(currentMenu->menuType() == MENU_MAIN)
							{
								MainMenu *mainMenu = static_cast<MainMenu*>(currentMenu);
								mainMenu->presetChoose(sys_para[dataByte[0] + 128] % 100);
								mainMenu->presetConfirm();

								midi_f1 = 1;
//								CSTask->Give();
							}
							pc_in_fl = 1;
							break;
						}

						if(statusByte == Midi::MIDI_STATUS_CC)
						{
							midiState = WAIT_BYTE2;
							break;
						}
						break;
					}

					case WAIT_BYTE2:
					{
						dataByte[1] = recievedByte;

						ControllersTask->midiCommand(dataByte[0], dataByte[1]);

						if((dataByte[0] == (sys_para[System::TUNER_EXTERNAL] & 0x7f)) && (sys_para[System::TUNER_EXTERNAL] & 0x80))
						{
							if(currentMenu->menuType() != MENU_TUNER)
							{
								currentMenu->showChild(new TunerMenu(currentMenu));
							}
							else
							{
								currentMenu->keyUp();
							}
						}
						else
						{
							if(currentMenu->menuType() == MENU_TUNER_EXT)
							{
								TunerExtMenu* tunerExtMenu = static_cast<TunerExtMenu*>(currentMenu);
								tunerExtMenu->showInputMidiCC(midi_b[1]);
							}

							if(currentMenu->menuType() == MENU_CONTROLLERS)
							{
								ControllersMenu* controllersMenu = static_cast<ControllersMenu*>(currentMenu);
								controllersMenu->showInputMidiCC(midi_b[1]);
							}
						}
						midiState = WAIT_BYTE1;
						break;
					}
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
					send_bank();

				uart_send(Midi::MIDI_STATUS_PC | sys_para[System::MIDI_CHANNEL]);
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

				program_change_midi = 0;
				midi_f1 = 0;
			}
		}

//-------------------------------------------Int event------------------------------------------
		while(trans_contr_po != int_contr_po)
		{
			if(trans_contr_po != int_contr_po)
			{
				if(!contr_cont)
				{
					uart_send(Midi::MIDI_STATUS_CC | sys_para[System::MIDI_CHANNEL]);
					uart_send(sys_para[System::EXPR_CCN] - 1);
					uart_send(int_contr_buf[trans_contr_po++]);
				}
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

				key_midi1[2] = 0;
			}
		}
	}
}

const uint8_t clk_data_size = 24;
uint16_t midi_clk_buf[32];
uint16_t midi_clk_data[clk_data_size];
uint8_t midi_clk_pos = 0;
volatile uint8_t sysEx_fl = 0;
extern "C" void USART1_IRQHandler()
{
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	us_buf = USART1->DR;

	if(sysEx_fl) // retranslate while SysEx not end
	{
		if(us_buf == Midi::MIDI_STATUS_SYSEX_STOP) sysEx_fl = 0;

		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, us_buf);
		return;
	}

	if(us_buf == Midi::MIDI_STATUS_TIMING_CLOCK)
	{
		if(sys_para[System::TAP_TYPE] == System::TAP_TYPE_GLOBAL_MIDI)
		{
			TIM14->CR1 &= (uint16_t)~TIM_CR1_CEN;

			if((TIM14->CNT < 60000 * 100 / System::minBpm / 24) &&
					(TIM14->CNT > 60000 * 100 / System::maxBpm / 24))
			midi_clk_buf[midi_clk_pos++] = TIM14->CNT;
			TIM14->CNT = 0;
			TIM14->CR1 |= TIM_CR1_CEN;

			if(midi_clk_pos == clk_data_size)
			{
				midi_clk_pos = 0;
				kgp_sdk_libc::memcpy(midi_clk_data, midi_clk_buf, clk_data_size * sizeof(uint16_t));
				ControllersTask->midiCalcTempo(midi_clk_data, clk_data_size);
			}
		}

		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, us_buf);
		return;
	}


	if(us_buf == Midi::MIDI_STATUS_SYSEX_START)
	{
		sysEx_fl = 1;

		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, us_buf);
	}
	else
	{
		midi_in_buf[midi_buf_wr_pos] = us_buf;
		midi_buf_wr_pos++;
		if(midi_buf_wr_pos == TMidiSendTask::midiInBufferSize) midi_buf_wr_pos = 0;

		MidiSendTask->Give();
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
		ControllersTask->extCommand(1, adc_bu2);
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
