#include "abstractmenu.h"

#include "../../display.h"
#include "../gui_task.h"

const uint8_t AbstractMenu::ascii_low[];
const uint8_t AbstractMenu::ascii_high[];

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
}

void AbstractMenu::returnFromChildMenu(TReturnMode returnMode)
{
	currentMenu = this;

	if(returnMode == TReturnMode::DeleteChild)
	{
		delete shownChildMenu;
	}
	shownChildMenu = nullptr;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	show(TShowMode::ReturnShow);
}

void AbstractMenu::setTopLevelMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
}

void AbstractMenu::showChild(AbstractMenu* child)
{
	shownChildMenu = child;
	child->show();
}

void AbstractMenu::setRunningString(StringOutParam* runningString)
{
	m_runningString = runningString;
}

StringOutParam* AbstractMenu::getRunningString()
{
	return m_runningString;
}
