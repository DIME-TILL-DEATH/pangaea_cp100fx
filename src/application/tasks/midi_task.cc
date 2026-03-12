#include "display.h"
#include "enc.h"
#include "gui_task.h"
#include "cs.h"
#include "eepr.h"
#include "BF706_send.h"
#include "allFonts.h"
#include "controllers_task.h"

#include "tunermenu.h"
#include "tunerextmenu.h"
#include "controllersmenu.h"
#include "midi_task.h"
#include "system.h"

#include "preset.h"

TMidiTask *MidiTask;

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

TMidiTask::TMidiTask() :
		TTask()
{
}
volatile uint8_t pc_mute_fl;
volatile uint8_t midi_f1 = 0;

volatile uint8_t program_change_midi = 0;
volatile uint8_t pc_in_fl = 0;
volatile uint8_t pc_in = 0;

void TMidiTask::Code()
{
	queue = new TQueue(32, sizeof(TMidiCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();

	TMidiCmd cmd;

	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.type)
		{
			case UART_IN:
			{
				uint8_t recievedByte = cmd.uartCmd.rcvData;

				if(recievedByte & 0x80)
					midiState = PROCESS_STATUS_BYTE;

				switch(midiState)
				{
					case PROCESS_STATUS_BYTE:
					{
						statusByte = recievedByte & 0xF0;
						rcvChannel = recievedByte & 0x0F;
						if((statusByte == Midi::MIDI_STATUS_PC || statusByte == Midi::MIDI_STATUS_CC || statusByte == Midi::MIDI_STATUS_NOTE_ON)
								&& rcvChannel == sys_para[System::MIDI_CHANNEL])
						{
							midiState = WAIT_BYTE1;
						}
						else uart_send(recievedByte);
						break;
					}
					case WAIT_BYTE1:
					{
						dataByte[0] = recievedByte;

						if(statusByte == Midi::MIDI_STATUS_PC)
						{
							currentPresetNumber = sys_para[System::MIDI_MAP_START + dataByte[0]] % 100;
							sys_para[System::LAST_PRESET_NUM] = currentPresetNumber;
							Preset::Change();
							pcSend(TPcType::PC_EXTERNAL, dataByte[0]);

							currentMenu->show();
							currentMenu->refresh();
							break;
						}

						if(statusByte == Midi::MIDI_STATUS_CC || statusByte == Midi::MIDI_STATUS_NOTE_ON)
						{
							midiState = WAIT_BYTE2;
							break;
						}
						break;
					}

					case WAIT_BYTE2:
					{
						dataByte[1] = recievedByte;

						if(statusByte == Midi::MIDI_STATUS_CC)
							ControllersTask->midiCommand(dataByte[0] + Controller::Src::CC1, dataByte[1]);

						if(statusByte == Midi::MIDI_STATUS_NOTE_ON
							&& dataByte[0] <= (Controller::Src::LAST_CONTROLLER - Controller::Src::NOTE))
							ControllersTask->midiCommand(dataByte[0] + Controller::Src::NOTE, dataByte[1]);

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
						uart_send(statusByte | rcvChannel);
						uart_send(dataByte[0]);
						uart_send(dataByte[1]);
						midiState = WAIT_BYTE1;
						break;
					}
				}
				break;
			}

			case PROGRAMM_CHANGE:
			{
				if(cmd.pcCmd.pcType == PC_INTERNAL)
					send_bank();

				uart_send(Midi::MIDI_STATUS_PC | sys_para[System::MIDI_CHANNEL]);
				switch(currentPreset.pcOut)
				{
					case Preset::PcOutType::MidiIn:
						uart_send(cmd.pcCmd.presetNumber);
					break;

					case Preset::PcOutType::Map:
						uart_send(sys_para[System::MIDI_MAP_START + currentPresetNumber]);
					break;

					case Preset::PcOutType::Set:
						uart_send(currentPreset.set & 0x7f);
					break;
				}
				break;
			}

			case EXPRESSION:
			{
				uart_send(Midi::MIDI_STATUS_CC | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[System::EXPR_CCN] - 1);
				uart_send(cmd.exprCmd.data);
				break;
			}

			case FOOTSWITCH:
			{
				uart_send(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uart_send(sys_para[cmd.fswCmd.fswType] - 1);
				uart_send(127 * cmd.fswCmd.state);
				break;
			}
		}
	}
}

void TMidiTask::uartRcv(uint8_t data)
{
	TMidiCmd cmd;
	cmd.type = UART_IN;
	cmd.uartCmd.rcvData = data;
	Command(&cmd);
}

void TMidiTask::pcSend(TPcType pcType, uint8_t presetNumber)
{
	TMidiCmd cmd;
	cmd.type = PROGRAMM_CHANGE;
	cmd.pcCmd.pcType = pcType;
	cmd.pcCmd.presetNumber = presetNumber;
	Command(&cmd);
}

void TMidiTask::exprSend(uint8_t data)
{
	TMidiCmd cmd;
	cmd.type = EXPRESSION;
	cmd.exprCmd.data = data;
	Command(&cmd);
}

void TMidiTask::fswPressed(uint8_t fswType, uint8_t state)
{
	TMidiCmd cmd;
	cmd.type = FOOTSWITCH;
	cmd.fswCmd.fswType = fswType;
	cmd.fswCmd.state = state;
	Command(&cmd);
}

const uint8_t clk_data_size = 24;
uint16_t midi_clk_buf[32];
uint16_t midi_clk_data[clk_data_size];
uint8_t midi_clk_pos = 0;
volatile uint8_t sysEx_fl = 0;
extern "C" void USART1_IRQHandler()
{
	uint8_t uart_buf = USART1->DR; // auto drop USART_IT_RXNE

	if(sysEx_fl) // retranslate while SysEx not end
	{
		if(uart_buf == Midi::MIDI_STATUS_SYSEX_STOP) sysEx_fl = 0;

		uart_send(uart_buf);
		return;
	}

	if(uart_buf == Midi::MIDI_STATUS_TIMING_CLOCK)
	{
		if(sys_para[System::TAP_TYPE] == System::TAP_TYPE_GLOBAL_MIDI)
		{
			TIM14->CR1 &= (uint16_t)~TIM_CR1_CEN;

			if((TIM14->CNT < 60000 * 100 / System::minBpm / 24) &&
					(TIM14->CNT > 60000 * 100 / System::maxBpm / 24))
			{
				midi_clk_buf[midi_clk_pos++] = TIM14->CNT;
			}
			TIM14->CNT = 0;
			TIM14->CR1 |= TIM_CR1_CEN;

			if(midi_clk_pos == clk_data_size)
			{
				midi_clk_pos = 0;
				kgp_sdk_libc::memcpy(midi_clk_data, midi_clk_buf, clk_data_size * sizeof(uint16_t));
				ControllersTask->midiCalcTempo(midi_clk_data, clk_data_size);
			}
		}

		uart_send(uart_buf);
		return;
	}


	if(uart_buf == Midi::MIDI_STATUS_SYSEX_START)
	{
		sysEx_fl = 1;
		uart_send(uart_buf);
	}
	else
	{
		MidiTask->uartRcv(uart_buf);
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
		MidiTask->exprSend(adc_bu2);
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
