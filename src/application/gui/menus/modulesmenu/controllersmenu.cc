#include <bitmaps.h>
#include <eeprom.h>
#include "controllersmenu.h"

#include "../../../tasks/controllers_task.h"
#include "../../../tasks/display_task.h"
#include "../../../tasks/io_task.h"
#include "../../../tasks/ui_task.h"
#include "footswitch.h"
#include "controller.h"
#include "preset.h"

#include "system.h"

const uint8_t ControllersMenu::strControllersMenu[][7];
const uint8_t ControllersMenu::strOutPcList[][8];
const uint8_t ControllersMenu::strControllerExt[][12];
const uint8_t ControllersMenu::strMidiDstList[][14];

const uint8_t ControllersMenu::dest_tabl[55];
const uint8_t ControllersMenu::dest_tabl_start[55];


ControllersMenu::ControllersMenu(AbstractMenu *parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_CONTROLLERS;
}

void ControllersMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	m_controllerDst = dest_tabl_start[currentPreset.controller[0].src];

	DisplayTask->Clear();
	for(uint8_t i = 0; i < 3; i++)
	{
		DisplayTask->StringOut(0, i, Font::fntSystem, Font::fnsNormal, &strControllersMenu[i][0]);
		switch(i)
		{
			case ControllerMenuParams::Controller:
				DisplayTask->ParamIndNum(45, 0, m_controllerNum + 1);
			break;

			case ControllerMenuParams::Source:
				printSources();
			break;

			case ControllerMenuParams::Destination:
				DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
			break;
		}
	}

	DisplayTask->Arrow(58, 3, 0);
}

void ControllersMenu::showInputMidiCC(uint8_t midiCC)
{
	DisplayTask->StringOut(0, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Midi in CC#->");
	DisplayTask->ParamIndNum(100, 3, midiCC);
}

void ControllersMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < ControllerMenuParams::MinValue)
			DisplayTask->StringOut(0, m_parNum, Font::fntSystem,
					(Font::TFontState)(blinkFlag * Font::fnsHighlight), &strControllersMenu[m_parNum][0]);
		else
			DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem,
					(Font::TFontState)(blinkFlag * Font::fnsHighlight), &strControllersMenu[m_parNum][0]);
	}
}

void ControllersMenu::encoderPressed()
{
	if(m_encoderKnobSelected == 0)
	{
		m_encoderKnobSelected = 1;

		if(m_parNum < ControllerMenuParams::MinValue)
			DisplayTask->StringOut(0, m_parNum, Font::fntSystem, Font::fnsHighlight, &strControllersMenu[m_parNum][0]);
		else
			DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, Font::fnsHighlight, &strControllersMenu[m_parNum][0]);
	}
	else m_encoderKnobSelected = 0;
}

void ControllersMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < paramsCount - 1)
		{
			if(m_parNum < ControllerMenuParams::MinValue)
				DisplayTask->StringOut(0, m_parNum, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum][0]);

			m_parNum++;

			if(m_parNum == ControllerMenuParams::MinValue)
			{
				DisplayTask->Clear();
				DisplayTask->IconAndArrows(ICON_CC, STRELKA_UP);
				DisplayTask->StringOut(0, 1, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum + 1][0]);
				DisplayTask->StringOut(0, 2, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum + 2][0]);
				DisplayTask->StringOut(0, 3, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum + 3][0]);

				DisplayTask->ParamInd(45, 0, currentPreset.controller[m_controllerNum].minVal);
				DisplayTask->ParamInd(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strOutPcList[currentPreset.pcOut][0]);

				if(currentPreset.set & 0x80)
					DisplayTask->ParamIndNum(45, 3, (currentPreset.set & 0x7f) + 1);
				else
					DisplayTask->ParamIndNum(45, 3, currentPresetNumber + 1);
			}
			restartBlinking(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum < Controller::controllersCount-1)
				{
					m_controllerNum++;
					DisplayTask->ParamIndNum(45, 0, m_controllerNum + 1);
					DisplayTask->ClearString(45, 1, Font::fntSystem, 13);

					printSources();

					m_controllerDst = dest_tabl_start[currentPreset.controller[m_controllerNum].dst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src < Controller::Src::LAST_CONTROLLER)
				{
					DisplayTask->ClearString(45, 1, Font::fntSystem, 13);
					currentPreset.controller[m_controllerNum].src = BaseParam::encSpeedInc(currentPreset.controller[m_controllerNum].src, Controller::Src::LAST_CONTROLLER);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Expression)
					{
						if(sys_para[System::EXPR_TYPE] < 3)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswDown)
					{
						if(sys_para[System::FSW1_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW1_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswConfirm)
					{
						if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW2_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswUp)
					{
						if(sys_para[System::FSW3_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW3_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}

					if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1
						&& currentPreset.controller[m_controllerNum].src < Controller::Src::NOTE)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, "CC#");
						DisplayTask->ParamIndNum(69, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::CC1);
					}
					else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::NOTE)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, "Note ");
						DisplayTask->ParamIndNote(71, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::NOTE);
					}
					else
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
					}
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDst < 54)
				{
					currentPreset.controller[m_controllerNum].dst = dest_tabl[++m_controllerDst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal < 127)
				{
					currentPreset.controller[m_controllerNum].minVal = BaseParam::encSpeedInc(currentPreset.controller[m_controllerNum].minVal, 127);
					DisplayTask->ParamInd(45, 0, currentPreset.controller[m_controllerNum].minVal);
				}
			break;

			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal < 127)
				{
					currentPreset.controller[m_controllerNum].maxVal = BaseParam::encSpeedInc(currentPreset.controller[m_controllerNum].maxVal, 127);
					DisplayTask->ParamInd(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				}
			break;

			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut < 2)
				{
					currentPreset.pcOut++;
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strOutPcList[currentPreset.pcOut][0]);
				}
			break;

			case ControllerMenuParams::Set:
				uint8_t a = 0;
				if(currentPreset.set & 0x80)
					a = currentPreset.set & 0x7f;
				else
					a = currentPresetNumber;

				if(a < 127)
				{
					a = BaseParam::encSpeedInc(a, 127);
					DisplayTask->ParamIndNum(45, 3, a + 1);
					currentPreset.set = a | 0x80;
				}
			break;
		}
	}
}

void ControllersMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum > 0)
		{
			if(m_parNum < ControllerMenuParams::MinValue)
				DisplayTask->StringOut(0, m_parNum, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, Font::fnsNormal, &strControllersMenu[m_parNum][0]);

			m_parNum--;

			if(m_parNum == ControllerMenuParams::Destination)
			{
				show(); // show first page
			}
			restartBlinking(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum > 0)
				{
					DisplayTask->ClearString(45, 1, Font::fntSystem, 13);
					m_controllerNum--;
					DisplayTask->ParamIndNum(45, 0, m_controllerNum + 1);

					printSources();

					m_controllerDst = dest_tabl_start[currentPreset.controller[m_controllerNum].dst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src > 0)
				{
					DisplayTask->ClearString(45, 1, Font::fntSystem, 13);
					currentPreset.controller[m_controllerNum].src = BaseParam::encSpeedDec(currentPreset.controller[m_controllerNum].src, 0);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswUp)
					{
						if(sys_para[System::FSW3_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW3_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswConfirm)
					{
						if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW2_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswDown)
					{
						if(sys_para[System::FSW1_PRESS_TYPE] != Footswitch::Controller && sys_para[System::FSW1_HOLD_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Expression)
					{
						if(sys_para[System::EXPR_TYPE] < 3)
							currentPreset.controller[m_controllerNum].src--;
					}

					printSources();
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDst)
				{
					currentPreset.controller[m_controllerNum].dst = dest_tabl[--m_controllerDst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal > 0)
				{
					currentPreset.controller[m_controllerNum].minVal = BaseParam::encSpeedDec(currentPreset.controller[m_controllerNum].minVal, 0);
					DisplayTask->ParamInd(45, 0, currentPreset.controller[m_controllerNum].minVal);
				}
			break;
			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal > 0)
				{
					currentPreset.controller[m_controllerNum].maxVal = BaseParam::encSpeedDec(currentPreset.controller[m_controllerNum].maxVal, 0);
					DisplayTask->ParamInd(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				}
			break;
			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut > 0)
				{
					currentPreset.pcOut--;
					DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strOutPcList[currentPreset.pcOut][0]);
				}
			break;
			case ControllerMenuParams::Set:
				uint8_t a = 0;
				if(currentPreset.set & 0x80)
					a = currentPreset.set & 0x7f;
				else
					a = currentPresetNumber;

				if(a > 0)
				{
					a = BaseParam::encSpeedDec(a, 0);
					DisplayTask->ParamIndNum(45, 3, a + 1);
					currentPreset.set = a | 0x80;
				}
			break;
		}
	}
}

void ControllersMenu::printSources()
{
	if(currentPreset.controller[m_controllerNum].src == Controller::Src::Off)
	{
		DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, "Off      ");
	}
	else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1
			&& currentPreset.controller[m_controllerNum].src < Controller::Src::NOTE)
	{
		DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, "CC#");
		DisplayTask->ParamIndNum(69, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::CC1);
	}
	else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::NOTE)
	{
		DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, "Note ");
		DisplayTask->ParamIndNote(71, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::NOTE);
	}
	else
	{
		DisplayTask->StringOut(45, 1, Font::fntSystem, Font::fnsNormal, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
	}
}

void ControllersMenu::key1()
{
	topLevelMenu->key1();
}

void ControllersMenu::key2()
{
	topLevelMenu->key2();
}

void ControllersMenu::key3()
{
	topLevelMenu->returnFromChildMenu();
}

void ControllersMenu::key4()
{
	topLevelMenu->key4();
}

void ControllersMenu::key5()
{
	topLevelMenu->key5();
}
