#include "mastereqmenu.h"

#include "modules.h"
#include "system.h"
#include "eeprom.h"

#include "master_setters.h"

#include "display_task.h"
#include "io_task.h"
#include "sharc_task.h"

#include "realparam.h"
#include "customparam.h"

MasterEqMenu::MasterEqMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_MASTER_EQ)
{
	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	RealParam* realParam;
	realParam = new RealParam(MasterEqDesc.low);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[0] = realParam;

	realParam = new RealParam(MasterEqDesc.mid);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[1] = realParam;

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Mid Freq", &mstEqMidFreq);
	customParam->setBounds(20, 4000);
	customParam->setByteSize(2);
	customParam->printCallback = printMidFreqCallback;
	customParam->setToDspCallback = setMidFreqCallback;
	params[2] = customParam;

	realParam = new RealParam(MasterEqDesc.high);
	realParam->setDspAddress(DSP_ADDRESS_EQ, EQ_MASTER_HIGH_GAIN_POS);
	realParam->setScaling(1, 24);
	realParam->setBounds(-24, +24);
	realParam->setDisplayBounds(-12, 12);
	realParam->setUnits("dB", 2);
	realParam->setIndicatorType(BaseParam::IndBarTransparent);
	realParam->setDisplayPosition(30);
	params[3] = realParam;


	setParams(params, paramNum);
	setIcon(false, ICON_NONE);

}

void MasterEqMenu::show(TShowMode showMode)
{
	currentMenu = this;

	m_currentParamNum = 0;

	DisplayTask->Clear();
	if(!sys_para[System::MASTER_EQ_ON])
	{
		DisplayTask->StringOut(12, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Master EQ Bypass");
		DisplayTask->StringOut(1, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"EQ on/off press EDIT");
	}
	else
	{
		printPage();
	}
}

void MasterEqMenu::task()
{
	if(!sys_para[System::MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem, FONT_BLINKING, m_paramsList[m_currentParamNum]->name());
	}
}

void MasterEqMenu::encoderPressed()
{
	if(!sys_para[System::MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		m_encoderKnobSelected = true;
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
				Font::fnsHighlight, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}
	else
	{
		m_encoderKnobSelected = false;
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
				Font::fnsNormal, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}

	restartBlinking(1);
}

void MasterEqMenu::encoderClockwise()
{
	if(!sys_para[System::MASTER_EQ_ON])
	{

		return;
	}

	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum < m_paramsCount - 1)
		{
			m_currentParamNum++; // Порядок важен!

			printPage();
			restartBlinking(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->increaseParam();
		m_paramsList[m_currentParamNum]->setData();
		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}
}

void MasterEqMenu::encoderCounterClockwise()
{
	if(!sys_para[System::MASTER_EQ_ON]) return;

	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum > 0)
		{
			m_currentParamNum--; // Порядок важен!

			printPage();
			restartBlinking(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->decreaseParam();
		m_paramsList[m_currentParamNum]->setData();
		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}
}

void MasterEqMenu::keyUp()
{
	EEPROM_SaveSystemData();
	topLevelMenu->returnFromChildMenu();
}

void MasterEqMenu::keyDown()
{
	if(sys_para[System::MASTER_EQ_ON])
	{
		MasterEqDesc.on.setterHandler(0);
		m_encoderKnobSelected = false;
		show();
	}
	else
	{
		MasterEqDesc.on.setterHandler(1);
		show();
	}

	EEPROM_SaveSystemData();

	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[System::MASTER_EQ_ON]);
}

void MasterEqMenu::key1()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key1();
}

void MasterEqMenu::key2()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key2();
}

void MasterEqMenu::key3()
{
	EEPROM_SaveSystemData();
	topLevelMenu->returnFromChildMenu();
}

void MasterEqMenu::key4()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key4();
}

void MasterEqMenu::key5()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key5();
}

void MasterEqMenu::printMidFreqCallback(void* parameter)
{
	char string[16];
	kgp_sdk_libc::memset(string, 0, 16);

	uint16_t value = *(uint16_t*)parameter;

	ksprintf(string, "%d %s", value, "Hz");

	DisplayTask->ClearString(60, 2, Font::fntSystem, 8);
	DisplayTask->StringOut(60, 2, Font::fntSystem , Font::fnsNormal, (uint8_t*)string);
}

void MasterEqMenu::setMidFreqCallback(void* parameter)
{
	MasterEqDesc.midFreq.setterHandler(mstEqMidFreq);
}

void MasterEqMenu::refresh()
{
	show();
}
