#include "cabnamemenu.h"

#include "display_task.h"

#include "preset.h"
#include "system.h"

CabNameMenu::CabNameMenu(AbstractMenu* parent)
	:AbstractMenu()
{
	topLevelMenu = parent;
	m_menuType = MENU_CABNAME;
}

void CabNameMenu::task()
{
	if(blinkFlag)
	{
		if(m_delay)
		{
			m_delay = 0;
		}
		else
		{
			m_delay = 1;
			if((m_currentShowCab) && System::cab_type)
			{
				if(currentPreset.cab2NameSize)
				{
					DisplayTask->Clear();
					DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)"2 - ");
					DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)currentPreset.cab2Name);
					m_currentShowCab = 0;
				}
			}
			else
			{
				if(currentPreset.cab1NameSize)
				{
					if(System::cab_type == CAB_CONFIG_STEREO)
					{
						DisplayTask->Clear();
						DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)"1 - ");
						DisplayTask->StringOut(24, 0, Font::fntSystem, 0, (uint8_t*)currentPreset.cab1Name);
					}
					else
					{
						DisplayTask->StringOut(0, 0, Font::fntSystem, 0, (uint8_t*)currentPreset.cab1Name);
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
	if(!currentPreset.cab1NameSize && !currentPreset.cab2NameSize) return;

	if(currentPreset.paramData.switches.cab)
	{
		currentMenu = this;
		DisplayTask->Clear();

		if(currentPreset.cab1NameSize)
		{
			if(System::cab_type == CAB_CONFIG_STEREO)
			{
				DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)"1 - ");
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)currentPreset.cab1Name);
			}
			else DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)currentPreset.cab1Name);
		}
		else
		{
			if(System::cab_type == CAB_CONFIG_STEREO)
			{
				DisplayTask->StringOut(0,0,Font::fntSystem,0,(uint8_t*)"2 - ");
				DisplayTask->StringOut(24,0,Font::fntSystem,0,(uint8_t*)currentPreset.cab2Name);
			}
		}
		restartBlinking(0);
		m_delay = 1;
	}
}
