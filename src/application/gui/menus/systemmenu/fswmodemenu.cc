#include "fswmodemenu.h"

#include "cs.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"
#include "fswcontrolmenu.h"

#include "system.h"

#include "gui_task.h"

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

	DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &strMode[0]);
	DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);

	if(sys_para[System::FSW1_MODE+m_fswNumber])
	{
		DisplayTask->StringOut(10, 2, Font::fntSystem, 0, &strReleaseHold[0][0]);
		DisplayTask->StringOut(80, 2, Font::fntSystem, 0, &strReleaseHold[1][0]);
	}

	tim5_start(0);
}

void FswModeMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(!m_parNum)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, 2 * blinkFlag_fl, &strMode[0]);
		}
		else
		{
			if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
				DisplayTask->StringOut(10+(m_parNum-1)*70, 2 , Font::fntSystem, 2 * blinkFlag_fl, &strReleaseHold[m_parNum-1][0]);
			else
				DisplayTask->StringOut(40, 0, Font::fntSystem, 2 * blinkFlag_fl, &strModeType[0][0]);
		}
	}
}

void FswModeMenu::encoderPressed()
{
	if(m_encoderKnobSelected)
	{
		m_encoderKnobSelected = 0;
		if(m_parNum == 0)
			DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &strMode[0]);
		else
			DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, 0, &strReleaseHold[m_parNum-1][0]);
	}
	else
	{
		if(m_parNum == 0)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, 2, &strMode[0]);
			m_encoderKnobSelected = 1;
		}
		else
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &expr_menu[0][0]);

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

	tim5_start(0);
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
					DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &strMode[0]);

				}
				else
				{
					DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, 0, &strReleaseHold[m_parNum - 1][0]);
				}
				m_parNum++;

				tim5_start(0);
			}
		}
		else
		{
			if(m_parNum == 0)
			{
				if(m_parNum == 0)
				{
					DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &strMode[0]);
					m_parNum++;
				}
				else
				{
					DisplayTask->StringOut(40, 0, Font::fntSystem, 2, &strModeType[0][0]);
				}
				tim5_start(0);
			}
		}
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Single)
		{
			sys_para[System::FSW1_MODE + m_fswNumber]++;
			DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strModeType[sys_para[System::FSW1_MODE+m_fswNumber]][0]);
			DisplayTask->StringOut(10, 2, Font::fntSystem, 0, &strReleaseHold[0][0]);
			DisplayTask->StringOut(80, 2, Font::fntSystem, 0, &strReleaseHold[1][0]);
		}
	}

	tim5_start(0);
}

void FswModeMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
		{
			if(m_parNum > 0)
			{
				DisplayTask->StringOut(10+(m_parNum-1)*70, 2, Font::fntSystem, 0, &strReleaseHold[m_parNum-1][0]);
				m_parNum--;
				tim5_start(0);
			}
		}
		else
		{
			if(m_parNum > 0)
			{
				m_parNum--;
				DisplayTask->StringOut(40, 0, Font::fntSystem, 0, (uint8_t*)&strModeType[m_parNum]);
				tim5_start(0);
			}
		}
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
		{
			sys_para[System::FSW1_MODE + m_fswNumber]--;
			DisplayTask->Clear_str(10, 2, Font::fntSystem, 35);
			DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);
		}
	}

	tim5_start(0);
}
