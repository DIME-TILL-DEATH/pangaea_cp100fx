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
		if(tim_fl)
			tim_fl = 0;
		else
		{
			tim_fl = 1;
			if((cab_n_fl1)&&cab_type)
			{
				if(cab2.name.size)
				{
					DisplayTask->Clear();
					DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)"2 - ");
					DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)cab2.name.string+1);
					cab_n_fl1 = 0;
					cab_n_fl2 = 1;
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
						DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)cab1.name.string+1);
					}
					else
						DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)cab1.name.string+1);
					cab_n_fl1 = 1;
					cab_n_fl2 = 0;
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
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)cab1.name.string + 1);
			}
			else DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)cab1.name.string + 1);
		}
		else
		{
			if(cab_type == 2)
			{
				DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)"2 - ");
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)cab2.name.string + 1);
			}
		}
		tim5_start(0);
		tim_fl = 1;
	}
}
