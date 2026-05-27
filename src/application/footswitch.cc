#include "footswitch.h"

#include "led.h"
#include "system.h"
#include "preset.h"
#include "footswitch.h"
#include "eeprom.h"

#include "controllers_task.h"
#include "ui_task.h"
#include "midi_task.h"

#include "console_helpers.h"
#include "preset_accessors.h"

#include "tunermenu.h"

const char* Footswitch::fswCommandString = "fsw";
const char* Footswitch::fswModeStringHandler = "mode";
const char* Footswitch::fswPressTypeStringHandler = "ptype";
const char* Footswitch::fswHoldTypeStringHandler = "htype";
const char* Footswitch::fswPressCcStringHandler = "cpressnum";
const char* Footswitch::fswHoldCcStringHandler = "choldnum";
const char* Footswitch::fswPressMapStringHandler = "ppressnum";
const char* Footswitch::fswHoldMapStringHandler = "pholdnum";

volatile uint8_t num_key_prog;
volatile uint8_t contr_pr[3];

bool confirmPressDisabled()
{
	return (sys_para[System::FSW2_PRESS_TYPE] != Footswitch::Default && sys_para[System::SWAP_SWITCH] == 0)
			|| (sys_para[System::FSW3_PRESS_TYPE] != Footswitch::Default && sys_para[System::SWAP_SWITCH] == 1);
}

bool confirmHoldDisabled()
{
	return (sys_para[System::FSW2_HOLD_TYPE] != Footswitch::Default && sys_para[System::SWAP_SWITCH] == 0)
			|| (sys_para[System::FSW3_HOLD_TYPE] != Footswitch::Default && sys_para[System::SWAP_SWITCH] == 1);
}

void Footswitch::press_execute(uint8_t num)
{
	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->returnToParent();
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
							if(confirmPressDisabled())
								preset_change_handler(mainMenu->preselectedPresetNum());
							break;
						}
						case 1:
						{
							if(sys_para[System::SWAP_SWITCH] == 0)
							{
								preset_change_handler(mainMenu->preselectedPresetNum());
							}
							else
							{
								mainMenu->presetUp();
								if(confirmPressDisabled())
									preset_change_handler(mainMenu->preselectedPresetNum());
							}
							break;
						}

						case 2:
						{
							if(sys_para[System::SWAP_SWITCH] == 0)
							{
								mainMenu->presetUp();
								if(confirmPressDisabled())
									preset_change_handler(mainMenu->preselectedPresetNum());
							}
							else
							{
								preset_change_handler(mainMenu->preselectedPresetNum());
							}
							break;
						}
					}

					currentMenu->refresh();
				}
			break;
			case Footswitch::Controller:
				if(!currentPreset.paramData.foot_ind_press[num])
				{
					currentPreset.paramData.foot_ind_press[num] = 1;
					ControllersTask->extCommand(num + 2, 127);
					LED_SetState((TLedType)(TLedType::LED_FSW_DOWN_RED + num), TLedState::ENABLED);
				}
				else
				{
					currentPreset.paramData.foot_ind_press[num] = 0;
					ControllersTask->extCommand(num + 2, 0);
					LED_SetState((TLedType)(TLedType::LED_FSW_DOWN_RED + num), TLedState::DISABLED);
				}

				if(sys_para[System::FSW1_CTRL_PRESS_CC + num])
					MidiTask->fswPressed(System::FSW1_CTRL_PRESS_CC + num, currentPreset.paramData.foot_ind_press[num]);

			break;

			case Footswitch::Tuner:
				currentMenu = new TunerMenu(currentMenu);
				currentMenu->show();
			break;

			default: // Preset maps
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(num_key_prog == num)
						contr_pr[num]++;
					if(contr_pr[num] > (sys_para[System::FSW1_PRESS_TYPE + num] - 3))
						contr_pr[num] = 0;

					num_key_prog = num;

					uint8_t presetToSwitch = sys_para[System::FSW1_PRESS_PR1 + num * 4 + contr_pr[num]];
					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					mainMenu->presetChoose(presetToSwitch);

					if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
							preset_change_handler(mainMenu->preselectedPresetNum());
						else if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default)
							preset_change_handler(mainMenu->preselectedPresetNum());

					}
					currentMenu->refresh();
				}
			break;
		}
	}
}

void Footswitch::hold_execute(uint8_t num)
{
	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->returnToParent();
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
							if(confirmHoldDisabled())
								preset_change_handler(mainMenu->preselectedPresetNum());
							break;
						}
						case 1:
						{
							if(sys_para[System::SWAP_SWITCH] == 0)
							{
								preset_change_handler(mainMenu->preselectedPresetNum());
							}
							else
							{
								mainMenu->presetUp();
								if(confirmHoldDisabled())
									preset_change_handler(mainMenu->preselectedPresetNum());
							}
							break;
						}
						case 2:
						{
							if(sys_para[System::SWAP_SWITCH] == 0)
							{
								mainMenu->presetUp();
								if(confirmHoldDisabled())
									preset_change_handler(mainMenu->preselectedPresetNum());
							}
							else
							{
								preset_change_handler(mainMenu->preselectedPresetNum());
							}
							break;
						}
					}
					currentMenu->refresh();
				}
				break;
			}
			case Footswitch:: Controller:
			{
				if(!currentPreset.paramData.foot_ind_hold[num])
				{
					currentPreset.paramData.foot_ind_hold[num] = 1;
					ControllersTask->extCommand(sys_para[System::FSW1_CTRL_HOLD_CC + num] + 4, 127);
					LED_SetState((TLedType)(TLedType::LED_FSW_DOWN_GREEN + num), TLedState::ENABLED);
				}
				else
				{
					currentPreset.paramData.foot_ind_hold[num] = 0;
					ControllersTask->extCommand(sys_para[System::FSW1_CTRL_HOLD_CC + num] + 4, 0);
					LED_SetState((TLedType)(TLedType::LED_FSW_DOWN_GREEN + num), TLedState::DISABLED);
				}

				if(sys_para[System::FSW1_CTRL_HOLD_CC + num])
					MidiTask->fswPressed(System::FSW1_CTRL_HOLD_CC + num, currentPreset.paramData.foot_ind_hold[num]);

			break;
			}

			case Footswitch::Tuner:
				currentMenu = new TunerMenu(currentMenu);
				currentMenu->show();
			break;

			default:
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(contr_pr[num] > (sys_para[System::FSW1_HOLD_TYPE + num] - 3))
						contr_pr[num] = 0;

					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					uint8_t presetToSwitch = sys_para[System::FSW1_HOLD_PR1 + num * 4 + contr_pr[num]++];
					mainMenu->presetChoose(presetToSwitch);

					if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
							preset_change_handler(mainMenu->preselectedPresetNum());
						else if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default)
							preset_change_handler(mainMenu->preselectedPresetNum());

					}
					currentMenu->refresh();
				}
			break;
		}
	}
}

void Footswitch::setMode(Footswitch::FswButton button, Footswitch::FswMode mode)
{
	sys_para[System::FSW1_MODE + button] = mode;
	console_printf("%s %d %s\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswModeStringHandler, mode);
}

void Footswitch::setPressType(Footswitch::FswButton button, Footswitch::FswType type)
{
	sys_para[System::FSW1_PRESS_TYPE + button] = type;
	console_printf("%s %d %s\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswPressTypeStringHandler, type);
}

void Footswitch::setHoldType(Footswitch::FswButton button, Footswitch::FswType type)
{
	sys_para[System::FSW1_HOLD_TYPE + button] = type;
	console_printf("%s %d %s\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswHoldTypeStringHandler, type);
}

void Footswitch::setPressCc(Footswitch::FswButton button, uint8_t ccValue)
{
	sys_para[System::FSW1_CTRL_PRESS_CC + button] = ccValue;
	console_printf("%s %d %s\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswPressCcStringHandler, ccValue);
}

void Footswitch::setHoldCc(Footswitch::FswButton button, uint8_t ccValue)
{
	sys_para[System::FSW1_CTRL_HOLD_CC + button] = ccValue;
	console_printf("%s %d %s\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswHoldCcStringHandler, ccValue);
}

void Footswitch::setPressMap(Footswitch::FswButton button, uint8_t mapNum, uint8_t presetNum)
{
	sys_para[System::FSW1_PRESS_PR1 + button*4 + mapNum] = presetNum;
	console_printf("%s %d %s %d\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswPressMapStringHandler, mapNum, presetNum);
}

void Footswitch::setHoldMap(Footswitch::FswButton button, uint8_t mapNum, uint8_t presetNum)
{
	sys_para[System::FSW1_HOLD_PR1 + button*4 + mapNum] = presetNum;
	console_printf("%s %d %s %d\r%x\n", Footswitch::fswCommandString, button, Footswitch::fswHoldMapStringHandler, mapNum, presetNum);
}
