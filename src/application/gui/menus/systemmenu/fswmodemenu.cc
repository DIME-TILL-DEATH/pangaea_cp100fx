#include "fswmodemenu.h"

#include "bitmaps.h"
#include "system.h"
#include "eeprom.h"
#include "footswitch.h"

#include "display_task.h"
#include "io_task.h"
#include "ui_task.h"

#include "fswcontrolmenu.h"

const uint8_t FswModeMenu::strMode[];
const uint8_t FswModeMenu::strModeType[][7];
const uint8_t FswModeMenu::strReleaseHold[][10];

FswModeMenu::FswModeMenu(AbstractMenu* parent, Footswitch::FswButton fswNumber)
{
	topLevelMenu = parent;
	m_menuType = MENU_FSW_MODE;

	m_fswNumber = fswNumber;
}

void FswModeMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	refresh();
}

void FswModeMenu::refresh()
{
	if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::FswMode::Single)
	{
		if(m_parNum > 1) m_parNum = 0;
		DisplayTask->Clear();
		DisplayTask->StringOut(3, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 0)), &strMode[0]);
		DisplayTask->StringOut(40, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 1)), &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);
	}
	else
	{
		DisplayTask->StringOut(3, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 0)), &strMode[0]);
		DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, &strModeType[sys_para[System::FSW1_MODE + m_fswNumber]][0]);
		DisplayTask->StringOut(10, 2, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 1)), &strReleaseHold[0][0]);
		DisplayTask->StringOut(80, 2, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 2)), &strReleaseHold[1][0]);
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

			shownChildMenu = new FswControlMenu(this, m_fswNumber, static_cast<Footswitch::FswModeName>(m_parNum - 1));
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
			if(m_parNum < 2) m_parNum++;
		}
		else
		{
			if(m_parNum == 0) m_parNum++;
		}
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Single)
			Footswitch::setMode(m_fswNumber, Footswitch::Double);
	}
	refresh();
}

void FswModeMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum > 0) m_parNum--;
	}
	else
	{
		if(sys_para[System::FSW1_MODE + m_fswNumber] == Footswitch::Double)
			Footswitch::setMode(m_fswNumber, Footswitch::Single);
	}
	refresh();
}
