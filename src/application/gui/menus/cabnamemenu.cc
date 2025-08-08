#include "cabnamemenu.h"

#include "allFonts.h"
#include "display.h"

#include "preset.h"

CabNameMenu::CabNameMenu(AbstractMenu* parent)
	:AbstractMenu()
{
	topLevelMenu = parent;
	m_menuType = MENU_CABNAME;
}

void CabNameMenu::task()
{
	if(blinkFlag_fl)
	{
		if(m_delay)
		{
			m_delay = 0;
		}
		else
		{
			m_delay = 1;
			if((m_currentShowCab) && cab_type)
			{
				if(cab2.name.size)
				{
					DisplayTask->Clear();
					DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)"2 - ");
					DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)cab2.name.string);
					m_currentShowCab = 0;
				}
			}
			else
			{
				if(cab1.name.size)
				{
					DisplayTask->Clear();
					if(cab_type==2)
					{
						DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)"1 - ");
						DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)cab1.name.string);
					}
					else
					{
						DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)cab1.name.string);
					}
					m_currentShowCab = 1;
				}
			}
		}
	}
}

void CabNameMenu::encoderPressed()
{
	topLevelMenu->returnFromChildMenu();
}

void CabNameMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu();
}

void CabNameMenu::show(TShowMode showMode)
{
	if(!cab1.name.size && !cab2.name.size) return;

	if(currentPreset.modules.paramData.switches.cab)
	{
		currentMenu = this;
		DisplayTask->Clear();

		if(cab1.name.size)
		{
			if(cab_type == 2)
			{
				DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)"1 - ");
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)cab1.name.string);
			}
			else DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)cab1.name.string);
		}
		else
		{
			if(cab_type == 2)
			{
				DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)"2 - ");
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)cab2.name.string);
			}
		}
		tim5_start(0);
		m_delay = 1;
	}
}
