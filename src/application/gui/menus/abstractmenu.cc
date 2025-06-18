#include "abstractmenu.h"

#include "../../display.h"
#include "../gui_task.h"

const uint8_t AbstractMenu::ascii_low1[];
const uint8_t AbstractMenu::ascii_low2[];
const uint8_t AbstractMenu::ascii_hig1[];
const uint8_t AbstractMenu::ascii_hig2[];

gui_menu_type AbstractMenu::menuType()
{
	return m_menuType;
}

void AbstractMenu::keyUp()
{
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}

	clean_flag();
}

void AbstractMenu::returnFromChildMenu(TReturnMode returnMode)
{
	currentMenu = this;

	if(returnMode == TReturnMode::DeleteChild)
	{
		delete shownChildMenu;
		shownChildMenu = nullptr;
	}

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	show(TShowMode::ReturnShow);
}
