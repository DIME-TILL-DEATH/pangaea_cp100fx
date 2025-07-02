#include "mastereqmenu.h"

#include "modules.h"

#include "appdefs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

#include "paramlistmenu/realparam.h"

extern uint8_t k_master_eq;

MasterEqMenu::MasterEqMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_MASTER_EQ)
{
	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	RealParam* realParam;
	realParam = new RealParam("Low", &sys_para[MASTER_EQ_LOW]);
	realParam->setDspAddress(DSP_ADDRESS_EQ, EQ_MASTER_LOW_GAIN_POS);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[0] = realParam;

	realParam = new RealParam("Mid", &sys_para[MASTER_EQ_MID]);
	realParam->setDspAddress(DSP_ADDRESS_EQ, EQ_MASTER_MID_GAIN_POS);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[1] = realParam;

	realParam = new RealParam("Mid Freq", &mstEqMidFreq);
	realParam->setDspAddress(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS);
	realParam->setByteSize(2);
	realParam->setBounds(20, 4000);
	realParam->setDisplayBounds(20, 4000);
	realParam->setUnits("Hz", 2);
	realParam->setDisplayPrecision(0);
	realParam->setIndicatorType(BaseParam::IndNone);
	realParam->setDisplayPosition(60);
	params[2] = realParam;

	realParam = new RealParam("High", &sys_para[MASTER_EQ_HIGH]);
	realParam->setDspAddress(DSP_ADDRESS_EQ, EQ_MASTER_HIGH_GAIN_POS);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[3] = realParam;


	setParams(params, paramNum);
	setIcon(false, 0);

}

void MasterEqMenu::show(TShowMode showMode)
{
	current_menu_type = m_menuType;
	currentMenu = this;

	m_currentParamNum = 0;

	DisplayTask->Clear();
	if(!sys_para[MASTER_EQ_ON])
	{
		DisplayTask->StringOut(12, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"Master EQ Bypass");
		DisplayTask->StringOut(1, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"EQ on/off press EDIT");
	}
	else
	{
		printPage();
	}
}

void MasterEqMenu::task()
{
	if(!sys_para[MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								blinkFlag_fl * 2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}
}

void MasterEqMenu::encoderPressed()
{
	if(!sys_para[MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		m_encoderKnobSelected = true;
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}
	else
	{
		m_encoderKnobSelected = false;
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								0, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}

	tim5_start(1);
}

void MasterEqMenu::encoderClockwise()
{
	if(!sys_para[MASTER_EQ_ON])
	{

		return;
	}

	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum < m_paramsCount - 1)
		{
			m_currentParamNum++; // Порядок важен!

			printPage();
			tim5_start(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->increaseParam();
		m_paramsList[m_currentParamNum]->setToDsp();
		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}
}

void MasterEqMenu::encoderCounterClockwise()
{
	if(!sys_para[MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum > 0)
		{
			m_currentParamNum--; // Порядок важен!

			printPage();
			tim5_start(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->decreaseParam();
		m_paramsList[m_currentParamNum]->setToDsp();
		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}
}

void MasterEqMenu::keyUp()
{
	write_sys();
	topLevelMenu->returnFromChildMenu();
}

void MasterEqMenu::keyDown()
{
	if(sys_para[MASTER_EQ_ON])
	{
		sys_para[MASTER_EQ_ON] = 0;
		m_encoderKnobSelected = false;
		show();
	}
	else
	{
		sys_para[MASTER_EQ_ON] = 1;
		show();
	}

	write_sys();

	DSP_gui_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[MASTER_EQ_ON]);
}

void MasterEqMenu::key1()
{
	write_sys();
	topLevelMenu->key1();
}

void MasterEqMenu::key2()
{
	write_sys();
	topLevelMenu->key2();
}

void MasterEqMenu::key3()
{
	write_sys();
	topLevelMenu->returnFromChildMenu();
}

void MasterEqMenu::key4()
{
	write_sys();
	topLevelMenu->key4();
}

void MasterEqMenu::key5()
{
	write_sys();
	topLevelMenu->key5();
}
