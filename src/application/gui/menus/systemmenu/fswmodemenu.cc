#include <bitmaps.h>
#include "fswmodemenu.h"

#include "../../../tasks/display_task.h"
#include "../../../tasks/io_task.h"
#include "../../../tasks/ui_task.h"
#include "eepr.h"
#include "fswcontrolmenu.h"
#include "footswitch.h"

#include "system.h"

const uint8_t FswModeMenu::strMode[];
const uint8_t FswModeMenu::strModeType[][7];
const uint8_t FswModeMenu::strReleaseHold[][10];

FswModeMenu::FswModeMenu(AbstractMenu* parent, uint8_t fswNumber)
{
	topLevelMenu = parent;
	m_menuType = MENU_FSW_MODE;

	m_fswNumber = fswNumber;
}

void FswModeMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();

	DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsNormal, &strMode[0]);
	DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);

	if(sys_para[System::FSW1_MODE+m_fswNumber])
	{
		DisplayTask->StringOut(10, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[0][0]);
		DisplayTask->StringOut(80, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[1][0]);
	}

	restartBlinking(0);
}

void FswModeMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(!m_parNum)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, FONT_BLINKING, &strMode[0]);
		}
		else
		{
			if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
				DisplayTask->StringOut(10+(m_parNum-1)*70, 2 , Font::fntSystem, FONT_BLINKING, &strReleaseHold[m_parNum-1][0]);
			else
				DisplayTask->StringOut(40, 0, Font::fntSystem, FONT_BLINKING, &strModeType[0][0]);
		}
	}
}

void FswModeMenu::encoderPressed()
{
	if(m_encoderKnobSelected)
	{
		m_encoderKnobSelected = 0;
		if(m_parNum == 0)
			DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsNormal, &strMode[0]);
		else
			DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[m_parNum-1][0]);
	}
	else
	{
		if(m_parNum == 0)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsHighlight, &strMode[0]);
			m_encoderKnobSelected = 1;
		}
		else
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsNormal, &Footswitch::expr_menu_str[0][0]);

			TFswControls fswControls;
			if(m_parNum == 1)
			{
				fswControls.fs = &sys_para[System::FSW1_PRESS_TYPE + m_fswNumber];
				fswControls.pr_start = &sys_para[System::FSW1_PRESS_PR1 + m_fswNumber*4];
				fswControls.k1_cc = &sys_para[System::FSW1_CTRL_PRESS_CC + m_fswNumber];
			}
			else if(m_parNum == 2)
			{
				fswControls.fs = &sys_para[System::FSW1_HOLD_TYPE + m_fswNumber];
				fswControls.pr_start = &sys_para[System::FSW1_HOLD_PR1 + m_fswNumber*4];
				fswControls.k1_cc = &sys_para[System::FSW1_CTRL_HOLD_CC + m_fswNumber];
			}
			shownChildMenu = new FswControlMenu(this, fswControls);
			shownChildMenu->show();
		}
	}

	restartBlinking(0);
}

void FswModeMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
		{
			if(m_parNum<2)
			{
				if(m_parNum == 0)
				{
					DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsNormal, &strMode[0]);

				}
				else
				{
					DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[m_parNum - 1][0]);
				}
				m_parNum++;

				restartBlinking(0);
			}
		}
		else
		{
			if(m_parNum == 0)
			{
				if(m_parNum == 0)
				{
					DisplayTask->StringOut(3, 0, Font::fntSystem, Font::fnsNormal, &strMode[0]);
					m_parNum++;
				}
				else
				{
					DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsHighlight, &strModeType[0][0]);
				}
				restartBlinking(0);
			}
		}
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Single)
		{
			sys_para[System::FSW1_MODE + m_fswNumber]++;
			DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, &strModeType[sys_para[System::FSW1_MODE+m_fswNumber]][0]);
			DisplayTask->StringOut(10, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[0][0]);
			DisplayTask->StringOut(80, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[1][0]);
		}
	}

	restartBlinking(0);
}

void FswModeMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
		{
			if(m_parNum > 0)
			{
				DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, Font::fnsNormal, &strReleaseHold[m_parNum-1][0]);
				m_parNum--;
				restartBlinking(0);
			}
		}
		else
		{
			if(m_parNum > 0)
			{
				m_parNum--;
				DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)&strModeType[m_parNum]);
				restartBlinking(0);
			}
		}
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
		{
			sys_para[System::FSW1_MODE + m_fswNumber]--;
			DisplayTask->ClearString(10, 2, Font::fntSystem, 35);
			DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);
		}
	}

	restartBlinking(0);
}
