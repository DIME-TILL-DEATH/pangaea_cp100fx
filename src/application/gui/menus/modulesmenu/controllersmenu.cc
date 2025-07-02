#include "controllersmenu.h"

#include "cs.h"
#include "cc.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

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

	m_controllerDst = dest_tabl_start[presetControllers[1]];

	DisplayTask->Clear();
	for(uint8_t i = 0; i < 3; i++)
	{
		DisplayTask->StringOut(0, i, TDisplayTask::fntSystem, 0, &strControllersMenu[i][0]);
		switch(i)
		{
			case 0:
				DisplayTask->ParamIndicNum(45, 0, 1);
			break;
			case 1:
				if(presetControllers[0] == 0)
					DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "Off      ");
				else
				{
					if(presetControllers[0] > 4)
					{
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, presetControllers[0] - 5);
					}
					else
					{
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, &strControllerExt[presetControllers[0] - 1][0]);
					}
				}
			break;
			case 2:
				DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
						(uint8_t*)strMidiDstList + presetControllers[1] * 14);
			break;
		}
	}
	DisplayTask->Strel(58, 3, 0);
}

void ControllersMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum < 3)
			DisplayTask->StringOut(0, m_parNum, TDisplayTask::fntSystem, blinkFlag_fl * 2, &strControllersMenu[m_parNum][0]);
		else
			DisplayTask->StringOut(0, m_parNum - 3, TDisplayTask::fntSystem, blinkFlag_fl * 2, &strControllersMenu[m_parNum][0]);
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
		if(m_parNum < 6)
		{
			if(m_parNum < 3)
				DisplayTask->StringOut(0, m_parNum, TDisplayTask::fntSystem, 0, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, TDisplayTask::fntSystem, 0, &strControllersMenu[m_parNum][0]);

			m_parNum++;

			if(m_parNum == 3)
			{
				DisplayTask->Clear();
				DisplayTask->Icon_Strel(ICON_CC, STRELKA_UP);
				DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0,
						(uint8_t*)strControllersMenu + (m_parNum + 1) * 7);
				DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0,
						(uint8_t*)strControllersMenu + (m_parNum + 2) * 7);
				DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0,
						(uint8_t*)strControllersMenu + (m_parNum + 3) * 7);

				DisplayTask->ParamIndic(45, 0, presetControllers[2 + m_controllerNum * 4]);
				DisplayTask->ParamIndic(45, 1, presetControllers[3 + m_controllerNum * 4]);
				DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
						(uint8_t*)strOutPcList + presetControllers[510] * 8);

				if(presetControllers[511] & 0x80)
					DisplayTask->ParamIndicNum(45, 3, (presetControllers[511] & 0x7f) + 1);
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
			case 0:
				if(m_controllerNum < 31)
				{
					DisplayTask->ParamIndicNum(45, 0, ++m_controllerNum + 1);

					DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);

					if(!presetControllers[m_controllerNum * 4])
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "Off      ");
					else
					{
						if(presetControllers[m_controllerNum * 4] > 4)
						{
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
							DisplayTask->ParamIndicNum(69, 1, presetControllers[m_controllerNum * 4] - 5);
						}
						else
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
									(uint8_t*)strControllerExt + (presetControllers[m_controllerNum * 4] - 1) * 12);
					}
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
							(uint8_t*)strMidiDstList + (presetControllers[m_controllerNum * 4 + 1]) * 14);

					m_controllerDst = dest_tabl_start[presetControllers[m_controllerNum * 4 + 1]];
				}
			break;
			case 1:
				if(presetControllers[m_controllerNum * 4] < 132)
				{
					DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
					presetControllers[m_controllerNum * 4] = enc_speed_inc(presetControllers[m_controllerNum * 4], 132);
					if(presetControllers[m_controllerNum * 4] == 1)
					{
						if(sys_para[EXPRESSION_TYPE] < 3)
							presetControllers[m_controllerNum * 4]++;

					}
					if(presetControllers[m_controllerNum * 4] == 2)
					{
						if(sys_para[fs1] != 1)
							presetControllers[m_controllerNum * 4]++;

					}
					if(presetControllers[m_controllerNum * 4] == 3)
					{
						if(sys_para[fs2] != 1)
							presetControllers[m_controllerNum * 4]++;

					}
					if(presetControllers[m_controllerNum * 4] == 4)
					{
						if(sys_para[fs3] != 1)
							presetControllers[m_controllerNum * 4]++;

					}
					if(presetControllers[m_controllerNum * 4] > 4)
					{
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
						DisplayTask->ParamIndicNum(69, 1, presetControllers[m_controllerNum * 4] - 5);
					}
					else
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
								(uint8_t*)strControllerExt + (presetControllers[m_controllerNum * 4] - 1) * 12);
				}
			break;
			case 2:
				if(m_controllerDst < 54)
				{
					presetControllers[m_controllerNum * 4 + 1] = dest_tabl[++m_controllerDst];
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
							(uint8_t*)strMidiDstList + (presetControllers[m_controllerNum * 4 + 1]) * 14);
				}
			break;
			case 3:
				if(presetControllers[2 + m_controllerNum * 4] < 127)
				{
					presetControllers[2 + m_controllerNum * 4] = enc_speed_inc(
							presetControllers[2 + m_controllerNum * 4], 127);
					DisplayTask->ParamIndic(45, 0, presetControllers[2 + m_controllerNum * 4]);
				}
			break;
			case 4:
				if(presetControllers[3 + m_controllerNum * 4] < 127)
				{
					presetControllers[3 + m_controllerNum * 4] = enc_speed_inc(
							presetControllers[3 + m_controllerNum * 4], 127);
					DisplayTask->ParamIndic(45, 1, presetControllers[3 + m_controllerNum * 4]);
				}
			break;
			case 5:
				if(presetControllers[510] < 2)
				{
					presetControllers[510]++;
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0, &strOutPcList[presetControllers[510]][0]);
				}
			break;
			case 6:
				uint8_t a = 0;
				if(presetControllers[511] & 0x80)
					a = presetControllers[511] & 0x7f;
				else
					a = currentPresetNumber;

				if(a < 127)
				{
					a = enc_speed_inc(a, 127);
					DisplayTask->ParamIndicNum(45, 3, a + 1);
					presetControllers[511] = a | 0x80;
				}
			break;
		}

		tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
		for(uint8_t i = 0; i < 32; i++)
		{
			if(presetControllers[i * 4])
			{
				if(presetControllers[i * 4 + 1] == 9)
					tap_del_fl = 1;
				if(presetControllers[i * 4 + 1] == 26)
					tap_trem_fl = 1;
				if(presetControllers[i * 4 + 1] == 35)
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
			if(m_parNum < 3)
				DisplayTask->StringOut(0, m_parNum, TDisplayTask::fntSystem, 0, &strControllersMenu[m_parNum][0]);
			else
				DisplayTask->StringOut(0, m_parNum - 3, TDisplayTask::fntSystem, 0, &strControllersMenu[m_parNum][0]);

			m_parNum--;

			if(m_parNum == 2)
			{
				DisplayTask->Clear();
				DisplayTask->Strel(58, 3, 0);
				DisplayTask->StringOut(0, 0, TDisplayTask::fntSystem, 0, (uint8_t*)strControllersMenu);
				DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0, (uint8_t*)strControllersMenu + 7);
				for(uint8_t i = 0; i < 3; i++)
				{
					DisplayTask->StringOut(0, i, TDisplayTask::fntSystem, 0, (uint8_t*)strControllersMenu + i * 7);
					switch(i)
					{
						case 0:
							DisplayTask->ParamIndicNum(45, 0, m_controllerNum + 1);
						break;
						case 1:
							if(!presetControllers[m_controllerNum * 4])
							{
								DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "Off      ");
							}
							else
							{
								DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
								if(presetControllers[m_controllerNum * 4] > 4)
								{
									DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
									DisplayTask->ParamIndicNum(69, 1, presetControllers[m_controllerNum * 4] - 5);
								}
								else
									DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
											&strControllerExt[presetControllers[m_controllerNum * 4] - 1][0]);
							}
						break;
						case 2:
							DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
									&strMidiDstList[presetControllers[m_controllerNum * 4 + 1]][0]);
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
			case 0:
				if(m_controllerNum > 0)
				{
					DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
					DisplayTask->ParamIndicNum(45, 0, --m_controllerNum + 1);
					if(!presetControllers[m_controllerNum * 4])
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "Off      ");
					else
					{
						if(presetControllers[m_controllerNum * 4] > 4)
						{
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
							DisplayTask->ParamIndicNum(69, 1, presetControllers[m_controllerNum * 4] - 5);
						}
						else
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
									(uint8_t*)strControllerExt + (presetControllers[m_controllerNum * 4] - 1) * 12);
					}
					m_controllerDst = dest_tabl_start[presetControllers[m_controllerNum * 4 + 1]];
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
							(uint8_t*)strMidiDstList + (presetControllers[m_controllerNum * 4 + 1]) * 14);
				}
			break;
			case 1:
				if(presetControllers[m_controllerNum * 4] > 0)
				{
					DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
					presetControllers[m_controllerNum * 4] = enc_speed_dec(presetControllers[m_controllerNum * 4], 0);
					if(presetControllers[m_controllerNum * 4] == 4)
					{
						if(sys_para[fs3] != 1)
							presetControllers[m_controllerNum * 4]--;
					}
					if(presetControllers[m_controllerNum * 4] == 3)
					{
						if(sys_para[fs2] != 1)
							presetControllers[m_controllerNum * 4]--;
					}
					if(presetControllers[m_controllerNum * 4] == 2)
					{
						if(sys_para[fs1] != 1)
							presetControllers[m_controllerNum * 4]--;
					}
					if(presetControllers[m_controllerNum * 4] == 1)
					{
						if(sys_para[EXPRESSION_TYPE] < 3)
							presetControllers[m_controllerNum * 4]--;
					}
					if(!presetControllers[m_controllerNum * 4])
						DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "Off      ");
					else
					{
						if(presetControllers[m_controllerNum * 4] > 4)
						{
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, "CC#");
							DisplayTask->ParamIndicNum(69, 1, presetControllers[m_controllerNum * 4] - 5);
						}
						else
							DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
									(uint8_t*)strControllerExt + (presetControllers[m_controllerNum * 4] - 1) * 12);
					}
				}
			break;
			case 2:
				if(m_controllerDst)
				{
					presetControllers[m_controllerNum * 4 + 1] = dest_tabl[--m_controllerDst];
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
							(uint8_t*)strMidiDstList + (presetControllers[m_controllerNum * 4 + 1]) * 14);
				}
			break;
			case 3:
				if(presetControllers[2 + m_controllerNum * 4] > 0)
				{
					presetControllers[2 + m_controllerNum * 4] = enc_speed_dec(
							presetControllers[2 + m_controllerNum * 4], 0);
					DisplayTask->ParamIndic(45, 0, presetControllers[2 + m_controllerNum * 4]);
				}
			break;
			case 4:
				if(presetControllers[3 + m_controllerNum * 4] > 0)
				{
					presetControllers[3 + m_controllerNum * 4] = enc_speed_dec(
							presetControllers[3 + m_controllerNum * 4], 0);
					DisplayTask->ParamIndic(45, 1, presetControllers[3 + m_controllerNum * 4]);
				}
			break;
			case 5:
				if(presetControllers[510] > 0)
					DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
							(uint8_t*)strOutPcList + --presetControllers[510] * 8);
			break;
			case 6:
				uint8_t a = 0;
				if(presetControllers[511] & 0x80)
					a = presetControllers[511] & 0x7f;
				else
					a = currentPresetNumber;
				if(a > 0)
				{
					a = enc_speed_dec(a, 0);
					DisplayTask->ParamIndicNum(45, 3, a + 1);
					presetControllers[511] = a | 0x80;
				}
			break;
		}
		tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
		for(uint8_t i = 0; i < 32; i++)
		{
			if(presetControllers[i * 4])
			{
				if(presetControllers[i * 4 + 1] == 9)
					tap_del_fl = 1;
				if(presetControllers[i * 4 + 1] == 26)
					tap_trem_fl = 1;
				if(presetControllers[i * 4 + 1] == 35)
					tap_moog_fl = 1;
			}
		}
	}
}
