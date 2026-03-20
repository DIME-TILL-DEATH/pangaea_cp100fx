#include "ui_task.h"

#include "eepr.h"

#include "adc.h"
#include "ER_OLEDM023-1B.h"
#include "pot.h"
#include "codec.h"

#include "mainmenu.h"
#include "usbmenu.h"

#include "preset.h"
#include "system.h"
#include "dsp.h"

#include "display_task.h"
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
	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_UITASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	Preset::Change();
	CODEC_Send(0xa301);

	mainMenu = new MainMenu();
	usbMenu = new UsbMenu(mainMenu);

	currentMenu = mainMenu;
	mainMenu->show();

	// enable blink timer
	TIM_SetCounter(TIM4, 0);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	//enable running string timer
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	TUICmd cmd;

	while(1)
	{
		cmdQueue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.type)
		{
			case UI_REFRESH_MENU:
			{
				currentMenu->refresh();
				break;
			}

			case UI_RETURN_FROM_MENU:
			{
				currentMenu->keyUp();
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
