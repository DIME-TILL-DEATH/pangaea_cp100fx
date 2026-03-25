#include <tuner_task.h>
#include "tunermenu.h"

#include "codec.h"
#include "led.h"

#include "display_task.h"
#include "sharc_task.h"

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
	DisplayTask->TunerRefFreq(TunerTask->refFreq);
}

void TunerMenu::encoderClockwise()
{
	if(TunerTask->refFreq<450.0f)
	{
		TunerTask->refFreq += 1.0f;
		DisplayTask->TunerRefFreq(TunerTask->refFreq);
	}
}

void TunerMenu::encoderCounterClockwise()
{
	if(TunerTask->refFreq>430.0f)
	{
		DisplayTask->TunerRefFreq(TunerTask->refFreq);
		TunerTask->refFreq -= 1.0f;
	}
}

void TunerMenu::keyUp()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::key1()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::key2()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::key3()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::key4()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::key5()
{
	exitTunerMenu();
	returnToParent();
}

void TunerMenu::exitTunerMenu()
{
	TunerTask->Disable();
	LED_SetState(TLedType::LED_TAP_GREEN, TLedState::DISABLED);
	DisplayTask->Clear();
}
