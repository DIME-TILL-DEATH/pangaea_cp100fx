#include "mastervolumemenu.h"

#include "eepr.h"
#include "../gui_task.h"

MasterVolumeMenu::MasterVolumeMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_MASTER_VOLUME)
{
	const uint8_t paramCount = 2;
	BaseParam* params[paramCount];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "Master Volume", &sys_para[MASTER_VOLUME]);
	params[0]->setDspAddress(DSP_ADDRESS_MASTER, PARAM_EQUAL_POS);
	params[0]->setDisplayPosition(85);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "Phones Volume", &sys_para[PHONES_VOLUME]);
	params[1]->setDisplayPosition(85);

	setParams(params, paramCount);
	setIcon(false, 0);
}

void MasterVolumeMenu::encoderClockwise()
{
	m_paramsList[m_currentParamNum]->increaseParam();

	m_paramsList[m_currentParamNum]->setToDsp();
	DisplayTask->Pot_Write();

	m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);

	clean_flag();
}

void MasterVolumeMenu::encoderCounterClockwise()
{
	m_paramsList[m_currentParamNum]->decreaseParam();

	m_paramsList[m_currentParamNum]->setToDsp();
	DisplayTask->Pot_Write();

	m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);

	clean_flag();
}

void MasterVolumeMenu::keyUp()
{
	write_sys();
	topLevelMenu->returnFromChildMenu();
}

void MasterVolumeMenu::key1()
{
	write_sys();
	topLevelMenu->key1();
}

void MasterVolumeMenu::key2()
{
	write_sys();
	clean_flag();
	topLevelMenu->returnFromChildMenu();
}

void MasterVolumeMenu::key3()
{
	write_sys();
	topLevelMenu->key3();
}

void MasterVolumeMenu::key4()
{
	write_sys();
	topLevelMenu->key4();
}

void MasterVolumeMenu::key5()
{
	write_sys();
	topLevelMenu->key5();
}
