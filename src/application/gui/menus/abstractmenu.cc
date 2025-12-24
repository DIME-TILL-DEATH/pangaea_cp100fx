#include "abstractmenu.h"

#include "eepr.h"

#include "display.h"
#include "gui_task.h"

#include "preset.h"



uint8_t AbstractMenu::subMenusToRoot = 1;

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
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	switch(returnMode)
	{
		case TReturnMode::DeleteChild:
		{
			if(shownChildMenu)
			{
				delete shownChildMenu;
				shownChildMenu = nullptr;
			}
			show(TShowMode::ReturnShow);
			break;
		}

		case TReturnMode::KeepChild:
		{
			show(TShowMode::ReturnShow);
			break;
		}

		case TReturnMode::ReturnToRoot:
		{
			if(shownChildMenu)
			{
				delete shownChildMenu;
				shownChildMenu = nullptr;
			}

			if(topLevelMenu)
			{
				if(subMenusToRoot == 1) topLevelMenu->returnFromChildMenu();
				else
				{
					subMenusToRoot--;
					topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
				}
			}
			else
			{
				subMenusToRoot = 1;
				show(TShowMode::ReturnShow);
			}
			break;
		}
	}
}

void AbstractMenu::setTopLevelMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
}

void AbstractMenu::showChild(AbstractMenu* child)
{
	shownChildMenu = child;
	currentMenu = this;
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
