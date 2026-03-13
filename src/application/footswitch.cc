#include "footswitch.h"

#include "appdefs.h"
#include "eepr.h"
#include "BF706_send.h"
#include "allFonts.h"
#include "controllers_task.h"
#include "system.h"

#include "preset.h"
#include "footswitch.h"
#include "midi_task.h"
#include "tunermenu.h"

#include "system.h"
#include "tasks/display_task.h"
#include "tasks/filesystem_task.h"
#include "tasks/io_task.h"
#include "tasks/io_task.h"
#include "tasks/sdtest_task.h"
#include "tasks/ui_task.h"
#include "tasks/usb_task.h"

volatile uint8_t num_key_prog;
volatile uint8_t contr_pr[3];

void Footswitch::press_execute(uint8_t num)
{
	TEncoderEvents encEvents;
	encEvents.pressed = 0;
	encEvents.updated = 0;

	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->keyUp();
		UITask->Give();
	}
	else
	{
		switch(sys_para[System::FSW1_PRESS_TYPE + num])
		{
			case Footswitch::Default:
				if(currentMenu->menuType() == MENU_MAIN)
				{
					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);

					switch(num)
					{
						case 0:
						{
							mainMenu->presetDown();

							if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
									|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							break;
						}
						case 1:
						{
							if(!sys_para[System::SWAP_SWITCH])
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							else
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encEvents.pressed = 1;
									UITask->encoderEvents(encEvents);
								}
							}
						}
						break;
						case 2:
							if(!sys_para[System::SWAP_SWITCH])
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encEvents.pressed = 1;
									UITask->encoderEvents(encEvents);
								}
							}
							else
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
						break;
					}
				}
			break;
			case Footswitch::Controller:
				if(!currentPreset.modules.paramData.foot_ind_press[num])
				{
					currentPreset.modules.paramData.foot_ind_press[num] = 1;
					ControllersTask->extCommand(num + 2, 127);
				}
				else
				{
					currentPreset.modules.paramData.foot_ind_press[num] = 0;
					ControllersTask->extCommand(num + 2, 0);
				}

				if(currentMenu->menuType() == MENU_MAIN)
					DisplayTask->IndFoot(num); // refresh в конце

				if(sys_para[System::FSW1_CTRL_PRESS_CC + num])
					MidiTask->fswPressed(System::FSW1_CTRL_PRESS_CC + num, currentPreset.modules.paramData.foot_ind_press[num]);

			break;

			case Footswitch::Tuner:
				currentMenu->showChild(new TunerMenu(currentMenu));
				UITask->task();
			break;

			default: // Preset maps
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(num_key_prog == num)
						contr_pr[num]++;
					if(contr_pr[num] > (sys_para[System::FSW1_PRESS_TYPE + num] - 3))
						contr_pr[num] = 0;

					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					mainMenu->presetChoose(sys_para[System::FSW1_PRESS_PR1 + num * 4 + contr_pr[num]]);
					num_key_prog = num;

					if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
						{
							encEvents.pressed = 1;
							UITask->encoderEvents(encEvents);
						}
						else
						{
							if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default)
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							else UITask->refreshMenu();
						}
					}
					else
					{
						UITask->refreshMenu();
					}
					UITask->Give();
				}
			break;
		}
	}
}

void Footswitch::hold_execute(uint8_t num)
{
	TEncoderEvents encEvents;
	encEvents.pressed = 0;
	encEvents.updated = 0;

	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->keyUp();
		UITask->task();
	}
	else
	{
		switch(sys_para[System::FSW1_HOLD_TYPE + num])
		{
			case Footswitch::Default:
			{
				if(currentMenu->menuType() == MENU_MAIN)
				{
					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);

					switch(num)
					{
						case 0:
						{
							mainMenu->presetDown();

							if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
									|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							break;
						}
						case 1:
						{
							if(!sys_para[System::SWAP_SWITCH])
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							else
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encEvents.pressed = 1;
									UITask->encoderEvents(encEvents);
								}
							}
						}
						break;
						case 2:
							if(!sys_para[System::SWAP_SWITCH])
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encEvents.pressed = 1;
									UITask->encoderEvents(encEvents);
								}
							}
							else
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
						break;
					}
				}
				break;
			}
			case Footswitch:: Controller:
			{
				if(!currentPreset.modules.paramData.foot_ind_hold[num])
				{

					currentPreset.modules.paramData.foot_ind_hold[num] = 1;
					ControllersTask->extCommand(sys_para[System::FSW1_CTRL_HOLD_CC + num] + 4, 127);
				}
				else
				{
					currentPreset.modules.paramData.foot_ind_hold[num] = 0;
					ControllersTask->extCommand(sys_para[System::FSW1_CTRL_HOLD_CC + num] + 4, 0);
				}

				if(currentMenu->menuType() == MENU_MAIN)
					DisplayTask->IndFoot(num);

				if(sys_para[System::FSW1_CTRL_HOLD_CC + num])
					MidiTask->fswPressed(System::FSW1_CTRL_HOLD_CC + num, currentPreset.modules.paramData.foot_ind_hold[num]);

			break;
			}

			case Footswitch::Tuner:
				currentMenu->showChild(new TunerMenu(currentMenu));
				UITask->Give();
			break;

			default:
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(contr_pr[num] > (sys_para[System::FSW1_HOLD_TYPE + num] - 3))
						contr_pr[num] = 0;

					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					mainMenu->presetChoose(sys_para[System::FSW1_HOLD_PR1 + num * 4 + contr_pr[num]++]);

					if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::Single)
						{
							encEvents.pressed = 1;
							UITask->encoderEvents(encEvents);
						}
						else
						{
							if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default)
							{
								encEvents.pressed = 1;
								UITask->encoderEvents(encEvents);
							}
							else UITask->refreshMenu();
						}
					}
					else
					{
						UITask->refreshMenu();
					}
				}
			break;
		}
	}
}
