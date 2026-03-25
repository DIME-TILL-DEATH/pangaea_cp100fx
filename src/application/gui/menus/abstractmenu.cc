#include "abstractmenu.h"

#include "eepr.h"
#include "preset.h"

#include "bitmaps.h"

#include "display_task.h"

bool AbstractMenu::blinkFlag = false;
uint8_t AbstractMenu::subMenusToRoot = 1;

gui_menu_type AbstractMenu::menuType()
{
	return m_menuType;
}

void AbstractMenu::keyUp()
{
	returnToParent();
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

void AbstractMenu::returnToParent()
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
	DisplayTask->SetVolIndicator(TVolIndicatorType::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

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

void AbstractMenu::keyEvent(const TKeysEvents& keysEvents)
{
	if(keysEvents.hold) return;

	if(keysEvents.keyUp)
	{
		keyUp();
		return;
	}

	if(keysEvents.keyDown)
	{
		keyDown();
		return;
	}

	if(keysEvents.key1)
	{
		key1();
		return;
	}

	if(keysEvents.key2)
	{
		key2();
		return;
	}

	if(keysEvents.key3)
	{
		key3();
		return;
	}

	if(keysEvents.key4)
	{
		key4();
		return;
	}

	if(keysEvents.key5)
	{
		key5();
		return;
	}
};

void AbstractMenu::encoderEvent(const TEncoderEvents& encoderEvents)
{

};

void AbstractMenu::setRunningString(StringOutParam* runningString)
{
	m_runningString = runningString;
}

StringOutParam* AbstractMenu::getRunningString()
{
	return m_runningString;
}

void AbstractMenu::restartBlinking(uint8_t val)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_SetCounter(TIM4, 0xffff);
	blinkFlag = val;
}
