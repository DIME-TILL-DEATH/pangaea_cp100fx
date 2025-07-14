#include "controllersmenu.h"

#include "cs.h"
#include "cc.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

#include "footswitch.h"
#include "controllers.h"
#include "preset.h"

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
		DisplayTask->StringOut(0, i, Font::fntSystem, 0, &strControllersMenu[i][0]);
		switch(i)
		{
			case ControllerMenuParams::Controller:
				DisplayTask->ParamIndicNum(45, 0, 1);
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[0].src == Controller::Src::Off)
				{
					DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "Off      ");
				}
				else if(currentPreset.controller[0].src >= Controller::Src::CC1)
				{
					DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
					DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[0].src - Controller::Src::CC1);
				}
				else
				{
					DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[0].src - 1][0]);
				}
			break;

			case ControllerMenuParams::Destination:
				DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[0].dst][0]);
			break;
		}
	}
	DisplayTask->Strel(58, 3, 0);
}

void ControllersMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < ControllerMenuParams::MinValue)
			DisplayTask->StringOut(0, m_parNum, Font::fntSystem, blinkFlag_fl * 2, &strControllersMenu[m_parNum][0]);
		else
			DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, blinkFlag_fl * 2, &strControllersMenu[m_parNum][0]);
	}
}

void ControllersMenu::encoderPressed()
{
	if(m_encoderKnobSelected == 0) m_encoderKnobSelected = 1;
	else m_encoderKnobSelected = 0;
}

void ControllersMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < paramsCount - 1)
		{
			if(m_parNum < ControllerMenuParams::MinValue)
				DisplayTask->StringOut(0, m_parNum, Font::fntSystem, 0, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, 0, &strControllersMenu[m_parNum][0]);

			m_parNum++;

			if(m_parNum == ControllerMenuParams::MinValue)
			{
				DisplayTask->Clear();
				DisplayTask->Icon_Strel(ICON_CC, STRELKA_UP);
				DisplayTask->StringOut(0, 1, Font::fntSystem, 0, &strControllersMenu[m_parNum + 1][0]);
				DisplayTask->StringOut(0, 2, Font::fntSystem, 0, &strControllersMenu[m_parNum + 2][0]);
				DisplayTask->StringOut(0, 3, Font::fntSystem, 0, &strControllersMenu[m_parNum + 3][0]);

				DisplayTask->ParamIndic(45, 0, currentPreset.controller[m_controllerNum].minVal);
				DisplayTask->ParamIndic(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strOutPcList[currentPreset.pcOut][0]);

				if(currentPreset.set & 0x80)
					DisplayTask->ParamIndicNum(45, 3, (currentPreset.set & 0x7f) + 1);
				else
					DisplayTask->ParamIndicNum(45, 3, currentPresetNumber + 1);
			}
			tim5_start(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum < controllersCount-1)
				{
					DisplayTask->ParamIndicNum(45, 0, ++m_controllerNum + 1);

					DisplayTask->Clear_str(45, 1, Font::fntSystem, 13);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Off)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "Off      ");
					}
					else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[m_controllerNum].src - 5);
					}
					else
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
					}

					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);

					m_controllerDst = dest_tabl_start[currentPreset.controller[m_controllerNum].dst];
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src < 132)
				{
					DisplayTask->Clear_str(45, 1, Font::fntSystem, 13);
					currentPreset.controller[m_controllerNum].src = enc_speed_inc(currentPreset.controller[m_controllerNum].src, 132);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Expression)
					{
						if(sys_para[EXPR_TYPE] < 3)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswDown)
					{
						if(sys_para[FSW1_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswConfirm)
					{
						if(sys_para[FSW2_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswDown)
					{
						if(sys_para[FSW3_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src++;

					}
					if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::CC1);
					}
					else
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
					}
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDst < 54)
				{
					currentPreset.controller[m_controllerNum].dst = dest_tabl[++m_controllerDst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal < 127)
				{
					currentPreset.controller[m_controllerNum].minVal = enc_speed_inc(currentPreset.controller[m_controllerNum].minVal, 127);
					DisplayTask->ParamIndic(45, 0, currentPreset.controller[m_controllerNum].minVal);
				}
			break;

			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal < 127)
				{
					currentPreset.controller[m_controllerNum].maxVal = enc_speed_inc(currentPreset.controller[m_controllerNum].maxVal, 127);
					DisplayTask->ParamIndic(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				}
			break;

			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut < 2)
				{
					currentPreset.pcOut++;
					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strOutPcList[currentPreset.pcOut][0]);
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
					a = enc_speed_inc(a, 127);
					DisplayTask->ParamIndicNum(45, 3, a + 1);
					currentPreset.set = a | 0x80;
				}
			break;
		}

		tap_del_fl = tap_trem_fl = tap_moog_fl = 0;

		for(uint8_t i = 0; i < controllersCount; i++)
		{
			if(currentPreset.controller[i].src != Controller::Src::Off)
			{
				if(currentPreset.controller[i].dst == Controller::Dst::DelayTap)
					tap_del_fl = 1;
				if(currentPreset.controller[i].dst == Controller::Dst::TremoloTap)
					tap_trem_fl = 1;
				if(currentPreset.controller[i].dst == Controller::Dst::RfLFOTAP)
					tap_moog_fl = 1;
			}
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
				DisplayTask->StringOut(0, m_parNum, Font::fntSystem, 0, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, Font::fntSystem, 0, &strControllersMenu[m_parNum][0]);

			m_parNum--;

			if(m_parNum == ControllerMenuParams::Destination)
			{
				DisplayTask->Clear();
				DisplayTask->Strel(58, 3, 0);
				DisplayTask->StringOut(0, 0, Font::fntSystem, 0, &strControllersMenu[0][0]);
				DisplayTask->StringOut(0, 1, Font::fntSystem, 0, &strControllersMenu[1][0]);
				for(uint8_t i = 0; i < 3; i++)
				{
					DisplayTask->StringOut(0, i, Font::fntSystem, 0, &strControllersMenu[i][0]);
					switch(i)
					{
						case 0:
							DisplayTask->ParamIndicNum(45, 0, m_controllerNum + 1);
						break;
						case 1:
							DisplayTask->Clear_str(45, 1, Font::fntSystem, 13);
							if(currentPreset.controller[m_controllerNum].src == Controller::Src::Off)
							{
								DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "Off      ");
							}
							else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1)
							{
								DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
								DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::CC1);
							}
							else
							{
								DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
							}
						break;
						case 2:
							DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
						break;
					}
				}
			}
			tim5_start(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case ControllerMenuParams::Controller:
				if(m_controllerNum > 0)
				{
					DisplayTask->Clear_str(45, 1, Font::fntSystem, 13);
					m_controllerNum--;
					DisplayTask->ParamIndicNum(45, 0, m_controllerNum + 1);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Off)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "Off      ");
					}
					else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[m_controllerNum].dst - Controller::Src::CC1);
					}
					else
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
					}
					m_controllerDst = dest_tabl_start[currentPreset.controller[m_controllerNum].dst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::Source:
				if(currentPreset.controller[m_controllerNum].src > 0)
				{
					DisplayTask->Clear_str(45, 1, Font::fntSystem, 13);
					currentPreset.controller[m_controllerNum].src = enc_speed_dec(currentPreset.controller[m_controllerNum].src, 0);

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswUp)
					{
						if(sys_para[FSW3_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswConfirm)
					{
						if(sys_para[FSW2_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::FswDown)
					{
						if(sys_para[FSW1_PRESS_TYPE] != Footswitch::Controller)
							currentPreset.controller[m_controllerNum].src--;
					}
					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Expression)
					{
						if(sys_para[EXPR_TYPE] < 3)
							currentPreset.controller[m_controllerNum].src--;
					}

					if(currentPreset.controller[m_controllerNum].src == Controller::Src::Off)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "Off      ");
					}
					else if(currentPreset.controller[m_controllerNum].src >= Controller::Src::CC1)
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, currentPreset.controller[m_controllerNum].src - Controller::Src::CC1);
					}
					else
					{
						DisplayTask->StringOut(45, 1, Font::fntSystem, 0, &strControllerExt[currentPreset.controller[m_controllerNum].src - 1][0]);
					}
				}
			break;

			case ControllerMenuParams::Destination:
				if(m_controllerDst)
				{
					currentPreset.controller[m_controllerNum].dst = dest_tabl[--m_controllerDst];
					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strMidiDstList[currentPreset.controller[m_controllerNum].dst][0]);
				}
			break;

			case ControllerMenuParams::MinValue:
				if(currentPreset.controller[m_controllerNum].minVal > 0)
				{
					currentPreset.controller[m_controllerNum].minVal = enc_speed_dec(currentPreset.controller[m_controllerNum].minVal, 0);
					DisplayTask->ParamIndic(45, 0, currentPreset.controller[m_controllerNum].minVal);
				}
			break;
			case ControllerMenuParams::MaxValue:
				if(currentPreset.controller[m_controllerNum].maxVal > 0)
				{
					currentPreset.controller[m_controllerNum].maxVal = enc_speed_dec(currentPreset.controller[m_controllerNum].maxVal, 0);
					DisplayTask->ParamIndic(45, 1, currentPreset.controller[m_controllerNum].maxVal);
				}
			break;
			case ControllerMenuParams::PcOut:
				if(currentPreset.pcOut > 0)
				{
					currentPreset.pcOut--;
					DisplayTask->StringOut(45, 2, Font::fntSystem, 0, &strOutPcList[currentPreset.pcOut][0]);
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
					a = enc_speed_dec(a, 0);
					DisplayTask->ParamIndicNum(45, 3, a + 1);
					currentPreset.set = a | 0x80;
				}
			break;
		}

		tap_del_fl = tap_trem_fl = tap_moog_fl = 0;

		for(uint8_t i = 0; i < controllersCount; i++)
		{
			if(currentPreset.controller[i].src != Controller::Src::Off)
			{
				if(currentPreset.controller[i].dst == Controller::Dst::DelayTap)
					tap_del_fl = 1;
				if(currentPreset.controller[i].dst == Controller::Dst::TremoloTap)
					tap_trem_fl = 1;
				if(currentPreset.controller[i].dst == Controller::Dst::RfLFOTAP)
					tap_moog_fl = 1;
			}
		}
	}
}
