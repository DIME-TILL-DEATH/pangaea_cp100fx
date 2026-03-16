#include "ui_task.h"

#include "appdefs.h"
#include "init.h"
#include "eepr.h"

#include "BF706_send.h"

#include "mainmenu.h"
#include "usbmenu.h"

#include "preset.h"
#include "system.h"

#include "compressor.h"
#include "display_task.h"


#include "ER_OLEDM023-1B.h"
#include "io_task.h"


TUITask *UITask;

AbstractMenu *currentMenu = nullptr;
MainMenu *mainMenu = nullptr;
UsbMenu *usbMenu = nullptr;

TUITask::TUITask() :
		TTask()
{
	responseQueue = new TQueue(4, sizeof(TResponse));
	cmdQueue = new TQueue(8, sizeof(TUICmd));
}

TUITask::~TUITask()
{
	delete responseQueue;
	delete cmdQueue;
}

void TUITask::Code()
{
	Delay(100);
	sem = new TSemaphore(TSemaphore::fstCounting, 4, 0);
	GetCpuClock();

	GATE_ChangePreset();
	COMPR_Init();
	COMPR_ChangePreset(0, 0);

	UITask->DisplayAccess(false);

	currentPresetNumber = sys_para[System::LAST_PRESET_NUM];

	DisplayTask->StartScreen(1);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	while(!ind_en);

	USART_Cmd(USART1, ENABLE);

	if(sys_para[3] & 0x80)
	{
		adc_init(1);
	}

	UITask->DisplayAccess(true);

	DSP_GuiSendParameter(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[System::CAB_SIM_DISABLED], 0);
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	DSP_GuiSendParameter(DSP_ADDRESS_SPDIF, sys_para[System::SPDIF_OUT_TYPE], 0);
	DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[System::TAP_TYPE], sys_para[System::TAP_HIGH]);
	DSP_GuiSendParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[System::CAB_SIM_CONFIG], 0); // left cab bypass

	if(!sys_para[System::PHONES_VOLUME]) sys_para[System::PHONES_VOLUME] = 127;

	DisplayTask->Pot_Write(); // phones, attenuator

	DSP_GuiSendParameter(DSP_ADDRESS_MASTER, sys_para[System::MASTER_VOLUME], 0);

	if(!sys_para[System::MASTER_EQ_FREQ_LO] && !sys_para[System::MASTER_EQ_FREQ_HI])
	{
		mstEqMidFreq = 1000;
	}
	else
	{
		mstEqMidFreq = sys_para[System::MASTER_EQ_FREQ_LO] << 8;
		mstEqMidFreq |= sys_para[System::MASTER_EQ_FREQ_HI];
	}

	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_LOW] + 24);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_GAIN_POS, sys_para[System::MASTER_EQ_MID] + 24);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_LO]);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_HI]);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ, EQ_MASTER_HIGH_GAIN_POS, sys_para[System::MASTER_EQ_HIGH] + 24);

	DSP_GuiSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[System::MASTER_EQ_ON]);

	tun_del_val = (127 - sys_para[System::TUNER_SPEED]) * (90.0f / 127.0f) + 10.0f;
	Delay(500);
	Preset::Change();

	send_codec(0xa301);
	IOTask->SetEnc(1);

	sem->Take(portMAX_DELAY);
	if(DisplayAccess())
	{
		mainMenu = new MainMenu();
		usbMenu = new UsbMenu(mainMenu);

		currentMenu = mainMenu;
		mainMenu->show();

		//enable running string timer
		TIM_SetCounter(TIM4, 0);
		TIM_Cmd(TIM5, ENABLE);
		TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	}

	TUICmd cmd;
	while(1)
	{
		if(DisplayAccess())
		{
			cmdQueue->Receive(&cmd, portMAX_DELAY);

			switch(cmd.type)
			{
				case UI_REFRESH_MENU:
				{
					currentMenu->refresh();
					break;
				}

				case UI_RUNNING_STRING:
				{
					StringOutParam *runningString = currentMenu->getRunningString();
					if(runningString) runningString->task();
					break;
				}

				case UI_TASK:
				{
					if((GPIOA->IDR & GPIO_Pin_9) && !usbMenu->isConnected() && currentMenu->menuType() != MENU_USB_SELECT)
					{
						currentMenu->showChild(usbMenu);
					}

					currentMenu->task();
					break;
				}

				case UI_KEYS_EVENTS:
				{
					if(cmd.keysEvents.hold) break;

					if(cmd.keysEvents.keyUp)
					{
						currentMenu->keyUp();
						break;
					}

					if(cmd.keysEvents.keyDown)
					{
						currentMenu->keyDown();
						break;
					}

					if(cmd.keysEvents.key1)
					{
						currentMenu->key1();
						break;
					}

					if(cmd.keysEvents.key2)
					{
						currentMenu->key2();
						break;
					}

					if(cmd.keysEvents.key3)
					{
						currentMenu->key3();
						break;
					}

					if(cmd.keysEvents.key4)
					{
						currentMenu->key4();
						break;
					}

					if(cmd.keysEvents.key5)
					{
						currentMenu->key5();
						break;
					}

					break;
				}

				case UI_ENCODER_EVENTS:
				{
					if(cmd.encoderEvents.pressed)
						currentMenu->encoderPressed();

					if(cmd.encoderEvents.updated)
					{
						if(cmd.encoderEvents.state == ENC_COUNTERCLOCKWISE_STEP)
							currentMenu->encoderCounterClockwise();

						if(cmd.encoderEvents.state == ENC_CLOCKWISE_STEP)
							currentMenu->encoderClockwise();
					}
					break;
				}
			}
		}
	}
}
