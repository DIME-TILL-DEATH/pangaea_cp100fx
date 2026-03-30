#include "ui_task.h"

#include "adc.h"
#include "lcd.h"
#include "pot.h"
#include "codec.h"

#include "eepr.h"
#include "footswitch.h"
#include "preset.h"
#include "system.h"
#include "dsp.h"

#include "display_task.h"
#include "io_task.h"

#include "mainmenu.h"
#include "usbmenu.h"

TUITask *UITask = nullptr;

AbstractMenu *currentMenu = nullptr;
MainMenu *mainMenu = nullptr;
UsbMenu *usbMenu = nullptr;

TUITask::TUITask() :
		TTask()
{
	responseQueue = new TQueue(4, sizeof(TResponse));
	cmdQueue = new TQueue(32, sizeof(TUICmd));
}

TUITask::~TUITask()
{
	delete responseQueue;
	delete cmdQueue;
}

void TUITask::Code()
{
	mainMenu = new MainMenu();
	currentMenu = mainMenu;

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_UITASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	Preset::Change(currentPresetNumber);
	CODEC_Send(0xa301);

	mainMenu->show();

	// enable blink timer
	TIM_SetCounter(TIM4, 0);
	TIM_Cmd(TIM4, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	//enable running string timer
	TIM_Cmd(TIM5, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	TUICmd cmd;
	usbMenu = new UsbMenu(mainMenu);

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

			case UI_SHOW_MENU:
			{
				if(cmd.showMenuParams.menu)
				{
					currentMenu = cmd.showMenuParams.menu;
					cmd.showMenuParams.menu->show();
				}
				break;
			}

			case UI_RETURN_FROM_MENU:
			{
				currentMenu->returnToParent();
				break;
			}

			case UI_TASK:
			{
				if((GPIOA->IDR & GPIO_Pin_9) && !usbMenu->isConnected() && currentMenu->menuType() != MENU_USB_SELECT)
				{
					if(usbMenu) currentMenu->showChild(usbMenu);
				}
				currentMenu->task();
				break;
			}

			case UI_RUNNING_STRING:
			{
				StringOutParam *runningString = currentMenu->getRunningString();
				if(runningString) runningString->task();
				break;
			}

			case UI_KEYS_EVENTS:
			{
				currentMenu->keyEvent(cmd.keysEvents);
				break;
			}

			case UI_ENCODER_EVENTS:
			{
				currentMenu->encoderEvent(cmd.encoderEvents);
				break;
			}

			case UI_FSW_SINGLE_MODE_PRESS:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Single)
					Footswitch::press_execute(cmd.fswEvents.fsw);
				break;
			}

			case UI_FSW_DUAL_MODE_PRESS:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Double)
					Footswitch::press_execute(cmd.fswEvents.fsw);
				break;
			}

			case UI_FSW_DUAL_MODE_HOLD:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Double)
					Footswitch::hold_execute(cmd.fswEvents.fsw);
				break;
			}

			case UI_CHANGE_PRESET:
			{
				mainMenu->presetChoose(cmd.changePresetParams.presetNumber);
				mainMenu->presetConfirm();
				break;
			}

			case UI_SET_PARAMETER:
			{
				cmd.setParamParams.funcHandler(cmd.setParamParams.value);
			}
		}
	}
}
