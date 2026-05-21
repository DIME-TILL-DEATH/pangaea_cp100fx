#include "controllersmenu.h"

#include "footswitch.h"
#include "controller.h"
#include "preset.h"
#include "system.h"

#include "console_helpers.h"

#include "display_task.h"


const uint8_t ControllersMenu::strControllersMenu[][7];
const uint8_t ControllersMenu::strOutPcList[][8];
const uint8_t ControllersMenu::strControllerExt[][12];
const uint8_t ControllersMenu::strMidiDstList[][14];

const uint8_t ControllersMenu::dst_visual_to_model[55];
const uint8_t ControllersMenu::dst_model_to_visual[55];


ControllersMenu::ControllersMenu(AbstractMenu *parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_CONTROLLERS;
}

void ControllersMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	m_controllerDstVisual = dst_model_to_visual[currentPreset.controller[m_controllerNum].dst];

	DisplayTask->Clear();
	printPage(0);
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
	if(m_encoderKnobSelected == 0) m_encoderKnobSelected = 1;
	else m_encoderKnobSelected = 0;

	refresh();
}

void ControllersMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < paramsCount - 1) m_parNum++;
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum < Controller::controllersCount-1)
				{
					m_controllerNum++;
					m_controllerDstVisual = dst_model_to_visual[currentPreset.controller[m_controllerNum].dst];
					printSources();
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src < Controller::Src::LAST_CONTROLLER)
				{
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

					Controller::setSrc(m_controllerNum, currentPreset.controller[m_controllerNum].src);
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDstVisual < 54)
					Controller::setDst(m_controllerNum, dst_visual_to_model[++m_controllerDstVisual]);
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal < 127)
					Controller::setMinVal(m_controllerNum, BaseParam::encSpeedInc(currentPreset.controller[m_controllerNum].minVal, 127));
			break;

			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal < 127)
					Controller::setMaxVal(m_controllerNum, BaseParam::encSpeedInc(currentPreset.controller[m_controllerNum].maxVal, 127));
			break;

			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut < 2)
				{
					currentPreset.pcOut++;
					console_printf("%s\r%x\n", Controller::pcoutCommandString, currentPreset.pcOut);
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
					currentPreset.set = a | 0x80;
				}
				console_printf("%s\r%x\n", Controller::pcsetCommandString, currentPreset.set & 0x7F);
			break;
		}
	}
	refresh();
}

void ControllersMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum > 0)
		{
			m_parNum--;
			refresh();
		}
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum > 0)
				{
					m_controllerNum--;
					m_controllerDstVisual = dst_model_to_visual[currentPreset.controller[m_controllerNum].dst];
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src > 0)
				{
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

					Controller::setSrc(m_controllerNum, currentPreset.controller[m_controllerNum].src);
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDstVisual)
					Controller::setDst(m_controllerNum, dst_visual_to_model[--m_controllerDstVisual]);
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal > 0)
					Controller::setMinVal(m_controllerNum, BaseParam::encSpeedDec(currentPreset.controller[m_controllerNum].minVal, 0));

			break;
			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal > 0)
					Controller::setMaxVal(m_controllerNum, BaseParam::encSpeedDec(currentPreset.controller[m_controllerNum].maxVal, 0));
			break;
			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut > 0)
				{
					currentPreset.pcOut--;
					console_printf("%s\r%x\n", Controller::pcoutCommandString, currentPreset.pcOut);
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
					currentPreset.set = a | 0x80;
				}
				console_printf("%s\r%x\n", Controller::pcsetCommandString, currentPreset.set & 0x7F);
			break;
		}
	}
	refresh();
}

void ControllersMenu::printSources()
{
	DisplayTask->ClearString(45, 1, Font::fntSystem, 13);

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

void ControllersMenu::refresh()
{
	if(m_parNum >= ControllerMenuParams::MinValue)
		printPage(1);
	else
		printPage(0);

	restartBlinking(0);
}

void ControllersMenu::printPage(uint8_t pageNum)
{
	if(pageNum == 1)
	{
		if(pageNum != currentPageNum)
		{
			DisplayTask->Clear();
			DisplayTask->IconAndArrows(ICON_CC, STRELKA_UP);
		}

		DisplayTask->StringOut(0, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 3)), &strControllersMenu[3][0]);
		DisplayTask->StringOut(0, 1, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 4)), &strControllersMenu[4][0]);
		DisplayTask->StringOut(0, 2, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 5)), &strControllersMenu[5][0]);
		DisplayTask->StringOut(0, 3, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 6)), &strControllersMenu[6][0]);

		DisplayTask->ParamInd(45, 0, currentPreset.controller[m_controllerNum].minVal);
		DisplayTask->ParamInd(45, 1, currentPreset.controller[m_controllerNum].maxVal);
		DisplayTask->StringOut(45, 2, Font::fntSystem, Font::fnsNormal, &strOutPcList[currentPreset.pcOut][0]);

		if(currentPreset.set & 0x80)
		{
			DisplayTask->ParamIndNum(45, 3, (currentPreset.set & 0x7f) + 1);
		}
		else
			DisplayTask->ParamIndNum(45, 3, currentPresetNumber + 1);
	}
	else
	{
		if(pageNum != currentPageNum) DisplayTask->Clear();

		for(uint8_t i = 0; i < 3; i++)
		{
			DisplayTask->StringOut(0, i, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == i)), &strControllersMenu[i][0]);

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

	currentPageNum = pageNum;
}
