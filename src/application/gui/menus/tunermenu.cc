#include "tunermenu.h"

#include "appdefs.h"
#include "cs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "spectrum.h"
#include "BF706_send.h"

TunerMenu::TunerMenu(AbstractMenu *parent)
{
	m_menuType = MENU_TUNER;
	topLevelMenu = parent;
}

void TunerMenu::show(TShowMode showMode)
{
	currentMenu = this;
	send_codec(0xa102);
	gui_send(13,0);
	tun_base_old = 0.0f;
	tun_ini();
	tuner_use = 1;
	current_menu = MENU_TUNER;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_IN, DSP_INDICATOR_IN);
}

void TunerMenu::encoderClockwise()
{
	if(SpectrumTask->ref_freq<450.0f)
		SpectrumTask->ref_freq += 1.0f;
}

void TunerMenu::encoderCounterClockwise()
{
	if(SpectrumTask->ref_freq>430.0f)
		SpectrumTask->ref_freq -= 1.0f;
}

void TunerMenu::keyUp()
{
	endTunerTask();
	clean_flag();
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}
}

void TunerMenu::key1()
{
	endTunerTask();
	topLevelMenu->key1();

	clean_flag();
}

void TunerMenu::key2()
{
	endTunerTask();
	topLevelMenu->key2();

	clean_flag();
}

void TunerMenu::key3()
{
	endTunerTask();
	topLevelMenu->key3();

	clean_flag();
}

void TunerMenu::key4()
{
	endTunerTask();
	topLevelMenu->key4();

	clean_flag();
}

void TunerMenu::key5()
{
	clean_flag();
	endTunerTask();

	tim5_start(0);
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::endTunerTask()
{
	tuner_use = 0;
	gui_send(13, 1);
	DisplayTask->Clear();
	CSTask->Give();
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	send_codec(0xa103);
}
