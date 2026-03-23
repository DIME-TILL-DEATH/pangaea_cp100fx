#include "tunermenu.h"

#include "codec.h"

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
	SharcTask->setParameter(DSP_ADDRESS_TUN_PROC, 0, 0);
	tun_base_old = 0.0f;

	DisplayTask->TunerInit();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_IN);
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
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}
}

void TunerMenu::key1()
{
	endTunerTask();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key2()
{
	endTunerTask();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key3()
{
	endTunerTask();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key4()
{
	endTunerTask();
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::key5()
{
	endTunerTask();
	restartBlinking(0);
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::endTunerTask()
{
	SharcTask->setParameter(DSP_ADDRESS_TUN_PROC, 1, 0);
	DisplayTask->TunerDeinit();

	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	CODEC_Send(0xa103);
}
