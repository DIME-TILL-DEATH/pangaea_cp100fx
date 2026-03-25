#include "midi_task.h"

#include "eepr.h"

#include "system.h"
#include "preset.h"

#include "display_task.h"
#include "io_task.h"
#include "ui_task.h"
#include "controllers_task.h"

#include "tunermenu.h"
#include "tunerextmenu.h"
#include "controllersmenu.h"

TMidiTask *MidiTask = nullptr;

TMidiTask::TMidiTask() :
		TTask()
{
}

void TMidiTask::uartSend(uint8_t val)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData(USART1, val);
}

void TMidiTask::sendBank()
{
	uartSend(0xb0 | sys_para[System::MIDI_CHANNEL]);
	uartSend(0x0);
	uartSend(0x0);
	uartSend(0x20);
	uartSend(0x0);
}

void TMidiTask::Code()
{
	queue = new TQueue(32, sizeof(TMidiCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_MDTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	USART_Cmd(USART1, ENABLE);

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
						else uartSend(recievedByte);
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
								tunerExtMenu->showInputMidiCC(dataByte[1]);
							}

							if(currentMenu->menuType() == MENU_CONTROLLERS)
							{
								ControllersMenu* controllersMenu = static_cast<ControllersMenu*>(currentMenu);
								controllersMenu->showInputMidiCC(dataByte[1]);
							}
						}
						uartSend(statusByte | rcvChannel);
						uartSend(dataByte[0]);
						uartSend(dataByte[1]);
						midiState = WAIT_BYTE1;
						break;
					}
				}
				break;
			}

			case PROGRAMM_CHANGE:
			{
				if(cmd.pcCmd.pcType == PC_INTERNAL)
					sendBank();

				uartSend(Midi::MIDI_STATUS_PC | sys_para[System::MIDI_CHANNEL]);
				switch(currentPreset.pcOut)
				{
					case Preset::PcOutType::MidiIn:
						uartSend(cmd.pcCmd.presetNumber);
					break;

					case Preset::PcOutType::Map:
						uartSend(sys_para[System::MIDI_MAP_START + currentPresetNumber]);
					break;

					case Preset::PcOutType::Set:
						uartSend(currentPreset.set & 0x7f);
					break;
				}
				break;
			}

			case EXPRESSION:
			{
				uartSend(Midi::MIDI_STATUS_CC | sys_para[System::MIDI_CHANNEL]);
				uartSend(sys_para[System::EXPR_CCN] - 1);
				uartSend(cmd.exprCmd.data);
				break;
			}

			case FOOTSWITCH:
			{
				uartSend(0xb0 | sys_para[System::MIDI_CHANNEL]);
				uartSend(sys_para[cmd.fswCmd.fswType] - 1);
				uartSend(127 * cmd.fswCmd.state);
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
void ISR_midi_recieve()
{
	uint8_t uart_buf = USART1->DR; // auto drop USART_IT_RXNE
	if(!MidiTask) return;

	if(sysEx_fl) // retranslate while SysEx not end
	{
		if(uart_buf == Midi::MIDI_STATUS_SYSEX_STOP) sysEx_fl = 0;

		MidiTask->uartSend(uart_buf);
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

		MidiTask->uartSend(uart_buf);
		return;
	}


	if(uart_buf == Midi::MIDI_STATUS_SYSEX_START)
	{
		sysEx_fl = 1;
		MidiTask->uartSend(uart_buf);
	}
	else
	{
		MidiTask->uartRcv(uart_buf);
	}
}
