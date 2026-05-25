#include "tunermenu.h"

#include "codec.h"
#include "led.h"
#include "system.h"
#include "eeprom.h"

#include "display_task.h"
#include "sharc_task.h"
#include "tuner_task.h"

TunerMenu::TunerMenu(AbstractMenu *parent)
{
	m_menuType = MENU_TUNER;
	topLevelMenu = parent;
}

void TunerMenu::show(TShowMode showMode)
{
	currentMenu = this;

	TunerTask->Enable(TTunerTask::TUNER_GUI);

	DisplayTask->TunerInit();
	refresh();
}

void TunerMenu::refresh()
{
	DisplayTask->StringOut(8, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 0)), (uint8_t*)"R");
	DisplayTask->ParamIndNum(15, 0, TunerTask->refFreq);
	DisplayTask->StringOut(35, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Hz");

	DisplayTask->StringOut(92, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 1)), (uint8_t*)"S");
	DisplayTask->ParamIndNum(100, 0, sys_para[System::TUNER_SPEED]);
}

void TunerMenu::encoderClockwise()
{
	if(m_parNum == 0)
	{
		if(TunerTask->refFreq < 450.0f) TunerTask->refFreq += 1.0f;
	}
	else
	{
		if(sys_para[System::TUNER_SPEED] < 127)
		{
			sys_para[System::TUNER_SPEED] = BaseParam::encSpeedInc(sys_para[System::TUNER_SPEED], 127, 1);
			EEPROM_DelayedSaveSystemData();
		}
	}
	refresh();
}

void TunerMenu::encoderCounterClockwise()
{
	if(m_parNum == 0)
	{
		if(TunerTask->refFreq>430.0f) TunerTask->refFreq -= 1.0f;
	}
	else
	{
		if(sys_para[System::TUNER_SPEED] > 0)
		{
			sys_para[System::TUNER_SPEED] = BaseParam::encSpeedDec(sys_para[System::TUNER_SPEED], 0, 1);
			EEPROM_DelayedSaveSystemData();
		}
	}
	refresh();
}

void TunerMenu::encoderPressed()
{
	if(m_parNum == 0) m_parNum = 1;
	else m_parNum = 0;

	refresh();
}

void TunerMenu::keyUp()
{
	returnToParent();
}

void TunerMenu::key1()
{
	returnToParent();
}

void TunerMenu::key2()
{
	returnToParent();
}

void TunerMenu::key3()
{
	returnToParent();
}

void TunerMenu::key4()
{
	returnToParent();
}

void TunerMenu::key5()
{
	returnToParent();
}

void TunerMenu::returnToParent()
{
	exitTunerMenu();
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}
}

void TunerMenu::exitTunerMenu()
{
	TunerTask->Disable();
	LED_SetState(TLedType::LED_TAP_GREEN, TLedState::DISABLED);
	DisplayTask->Clear();
}
