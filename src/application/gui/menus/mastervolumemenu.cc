#include "mastervolumemenu.h"

#include "system.h"
#include "eeprom.h"

#include "master_setters.h"

MasterVolumeMenu::MasterVolumeMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_MASTER_VOLUME)
{
	const uint8_t paramCount = 2;
	BaseParam* params[paramCount];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, &MasterVolDesc.master);
	params[0]->setDisplayPosition(85);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, &MasterVolDesc.phones);
	params[1]->setDisplayPosition(85);

	setParams(params, paramCount);
	setIcon(false, ICON_NONE);
}

void MasterVolumeMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum < m_paramsCount - 1)
		{
			m_currentParamNum++;
			printPage();
			restartBlinking(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->increaseParam();

		m_paramsList[m_currentParamNum]->setData();
		DisplayTask->PotWrite();

		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}
}

void MasterVolumeMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum > 0) //(paramsCount - 1)
		{
			m_currentParamNum--;
			printPage();
			restartBlinking(0);
		}
	}
	else
	{
		m_paramsList[m_currentParamNum]->decreaseParam();

		m_paramsList[m_currentParamNum]->setData();
		DisplayTask->PotWrite();

		m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);


	}
}

void MasterVolumeMenu::keyUp()
{
	EEPROM_SaveSystemData();
	topLevelMenu->returnFromChildMenu();
}

void MasterVolumeMenu::key1()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key1();
}

void MasterVolumeMenu::key2()
{
	EEPROM_SaveSystemData();
	topLevelMenu->returnFromChildMenu();
}

void MasterVolumeMenu::key3()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key3();
}

void MasterVolumeMenu::key4()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key4();
}

void MasterVolumeMenu::key5()
{
	EEPROM_SaveSystemData();
	topLevelMenu->key5();
}
