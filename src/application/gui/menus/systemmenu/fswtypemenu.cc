#include "fswtypemenu.h"

#include "cs.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

#include "fswmodemenu.h"

#include "gui_task.h"

const uint8_t FswTypeMenu::strFswMenu[][12];

FswTypeMenu::FswTypeMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_FSW_TYPE;

}

void FswTypeMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	for(uint8_t i = 0; i<4; i++)
		DisplayTask->StringOut(3, i, Font::fntSystem, 0, (uint8_t*)&strFswMenu[i]);
	DisplayTask->ParamIndic(58, 3, sys_para[FSW_SPEED]);

	tim5_start(0);
}

void FswTypeMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 2 * blinkFlag_fl, (uint8_t*)&strFswMenu[m_parNum]);
	}

}

void FswTypeMenu::encoderPressed()
{
	if(m_parNum<3)
	{
		shownChildMenu = new FswModeMenu(this, m_parNum);
		shownChildMenu->show();
	}
	else
	{
		if(!m_encoderKnobSelected)
		{
			m_encoderKnobSelected = 1;
			DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 2, (uint8_t*)&strFswMenu[m_parNum]);
		}
		else
		{
			m_encoderKnobSelected = 0;
			DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 0, (uint8_t*)&strFswMenu[m_parNum]);
		}
	}
	tim5_start(0);
}

void FswTypeMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum<3)
		{
			DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 0, (uint8_t*)&strFswMenu[m_parNum++]);
		}
	}
	else
	{
		if(sys_para[FSW_SPEED]<127)
		{
			sys_para[FSW_SPEED] = enc_speed_inc(sys_para[FSW_SPEED], 127);
			DisplayTask->ParamIndic(58, 3, sys_para[FSW_SPEED]);
		}
	}

	tim5_start(0);
}

void FswTypeMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum>0)
		{
			DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 0, (uint8_t*)&strFswMenu[m_parNum--]);
		}
	}
	else
	{
		if(sys_para[FSW_SPEED])
		{
			sys_para[FSW_SPEED] = enc_speed_dec(sys_para[FSW_SPEED], 0);
			DisplayTask->ParamIndic(58, 3, sys_para[FSW_SPEED]);
		}
	}

	tim5_start(0);
}
