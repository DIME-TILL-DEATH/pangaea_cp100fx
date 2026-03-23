#include "tunermenu.h"

#include "codec.h"
#include "led.h"

#include "display_task.h"
#include "spectrum_task.h"
#include "sharc_task.h"

TunerMenu::TunerMenu(AbstractMenu *parent)
{
	m_menuType = MENU_TUNER;
	topLevelMenu = parent;
}

void TunerMenu::show(TShowMode showMode)
{
	currentMenu = this;

	CODEC_Send(0xa102);
	SharcTask->setParameter(DSP_ADDRESS_TUNER_PROCESS, 0);	// blinking TAP off
	SharcTask->setParameter(DSP_ADDRESS_IND_SRC, DSP_INDICATOR_IN, 0);

	DisplayTask->TunerInit();
	DisplayTask->TunerRefFreq(SpectrumTask->ref_freq);
}

void TunerMenu::encoderClockwise()
{
	if(SpectrumTask->ref_freq<450.0f)
	{
		SpectrumTask->ref_freq += 1.0f;
		DisplayTask->TunerRefFreq(SpectrumTask->ref_freq);
	}
}

void TunerMenu::encoderCounterClockwise()
{
	if(SpectrumTask->ref_freq>430.0f)
	{
		DisplayTask->TunerRefFreq(SpectrumTask->ref_freq);
		SpectrumTask->ref_freq -= 1.0f;
	}
}

void TunerMenu::keyUp()
{
	exitTunerMenu();
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}
}

void TunerMenu::key1()
{
	exitTunerMenu();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key2()
{
	exitTunerMenu();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key3()
{
	exitTunerMenu();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key4()
{
	exitTunerMenu();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key5()
{
	exitTunerMenu();
	restartBlinking(0);
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::exitTunerMenu()
{
	SharcTask->setParameter(DSP_ADDRESS_TUNER_PROCESS, 1);
	LED_SetState(TLedType::LED_TAP_GREEN, TLedState::DISABLED);
	CODEC_Send(0xa103);

	DisplayTask->Clear();
}
