#include "tunermenu.h"

#include "appdefs.h"
#include "cs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
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
	DSP_GuiSendParameter(DSP_ADDRESS_TUN_PROC, 0, 0);
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
	tim5_start(0);
	topLevelMenu->returnFromChildMenu();
}

void TunerMenu::endTunerTask()
{
	DSP_GuiSendParameter(DSP_ADDRESS_TUN_PROC, 1, 0);
	DisplayTask->TunerDeinit();
//	DisplayTask->Clear();
	CSTask->Give();
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	send_codec(0xa103);
}
