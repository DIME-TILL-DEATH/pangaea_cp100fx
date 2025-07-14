#include "attenuatormenu.h"

#include "eepr.h"
#include "gui_task.h"

#include "stringlistparam.h"

AttenuatorMenu::AttenuatorMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_ATTENUATOR)
{
	const uint8_t paramCount = 1;
	BaseParam* params[paramCount];

	params[0] = new StringListParam("Attenuator", &sys_para[ATTENUATOR],
			 {" +4 dB", " +3 dB", " +2 dB", " +1 dB", "  0 dB", " -1 dB", " -2 dB", " -3 dB",
			" -4 dB", " -5 dB", " -6 dB", " -7 dB", " -8 dB", " -9 dB", "-10 dB", "-11 dB", "-12 dB",
			"-13 dB", "-14 dB", "-15 dB", "-16 dB", "-17 dB", "-18 dB", "-19 dB", "-20 dB", "-21 dB",
			"-23 dB", "-24 dB", "-27 dB", "-30 dB", "-34 dB", "-35 dB", "-36 dB", "-37 dB", "-38 dB",
			"-40 dB", "-41 dB", "-42 dB", "-43 dB", "-44 dB", "-45 dB", "-46 dB", "-47 dB", "-48 dB",
			"-49 dB", "-50 dB", "-51 dB", "-52 dB", "-53 dB", "-54 dB", "-55 dB", "-56 dB", "-57 dB",
			"-58 dB", "-59 dB", "-60 dB"}, 6);
	params[0]->setDisplayPosition(76);
	params[0]->setInverse(true);

	setVolumeIndicator(TDisplayTask::VOL_INDICATOR_IN, DSP_INDICATOR_IN);

	setParams(params, paramCount);
	setIcon(false, ICON_NONE);

	DisplayTask->Pot_Write();
}

void AttenuatorMenu::encoderClockwise()
{
	if(!m_paramsList[m_currentParamNum]->inverse()) m_paramsList[m_currentParamNum]->increaseParam();
	else m_paramsList[m_currentParamNum]->decreaseParam();

	DisplayTask->Pot_Write();

	m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
}

void AttenuatorMenu::encoderCounterClockwise()
{
	if(!m_paramsList[m_currentParamNum]->inverse()) m_paramsList[m_currentParamNum]->decreaseParam();
	else m_paramsList[m_currentParamNum]->increaseParam();

	DisplayTask->Pot_Write();

	m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
}

void AttenuatorMenu::keyUp()
{
	write_sys();
	topLevelMenu->returnFromChildMenu();
}

void AttenuatorMenu::key1()
{
	write_sys();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	topLevelMenu->returnFromChildMenu();
}

void AttenuatorMenu::key2()
{
	write_sys();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	topLevelMenu->key2();
}

void AttenuatorMenu::key3()
{
	write_sys();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	topLevelMenu->key3();
}

void AttenuatorMenu::key4()
{
	write_sys();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	topLevelMenu->key4();
}

void AttenuatorMenu::key5()
{
	write_sys();
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	topLevelMenu->key5();
}
