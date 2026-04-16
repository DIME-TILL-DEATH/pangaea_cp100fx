#include "systemmenu.h"

#include "adc.h"

#include "system.h"
#include "sharc_task.h"
#include "eeprom.h"

#include "baseparam.h"
#include "stringlistparam.h"
#include "submenuparam.h"
#include "customparam.h"

#include "syssettings_handlers.h"
#include "master_setters.h"

#include "dialog.h"
#include "midimapmenu.h"
#include "expressionmenu.h"
#include "fswtypemenu.h"
#include "tunerextmenu.h"

SystemMenu::SystemMenu(AbstractMenu* parent, gui_menu_type menuType)
		:ParamListMenu(parent, menuType)
{
	m_previousCabConfig = sys_para[System::CAB_SIM_CONFIG];
}


void SystemMenu::keyUp()
{
	if(editingFinished())
	{
		currentMenu = topLevelMenu;
		topLevelMenu->returnFromChildMenu();
	}
}

void SystemMenu::keyDown()
{
	if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_SUBMENU)
	{
		SubmenuParam* submenuParam = static_cast<SubmenuParam*>(m_paramsList[m_currentParamNum]);
		submenuParam->showSubmenu(this);
	}
	else if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_CUSTOM)
	{
		CustomParam* customParam = static_cast<CustomParam*>(m_paramsList[m_currentParamNum]);
		if(customParam->keyDownCallback) customParam->keyDownCallback(customParam->valuePtr());
	}
}

void SystemMenu::key1()
{
	if(editingFinished()) topLevelMenu->key1();
}

void SystemMenu::key2()
{
	if(editingFinished()) topLevelMenu->key2();
}

void SystemMenu::key3()
{
	if(editingFinished()) topLevelMenu->key3();
}

void SystemMenu::key4()
{
	if(editingFinished()) topLevelMenu->returnFromChildMenu();
}
void SystemMenu::key5()
{
	if(editingFinished()) topLevelMenu->key5();
}

bool SystemMenu::editingFinished()
{
#ifdef __MONO_MOD__
	if(m_previousCabConfig == 2)
	{
		if(m_previousCabConfig != sys_para[System::CAB_SIM_CONFIG])
		{
			if(shownChildMenu) delete shownChildMenu;

			shownChildMenu = new Dialog(this, Dialog::RestartDevice);
			shownChildMenu->show();

			restartBlinking(0);
			return false;

		}
	}
	else
	{
		if(sys_para[System::CAB_SIM_CONFIG] == 2)
		{
			if(shownChildMenu) delete shownChildMenu;

			shownChildMenu = new Dialog(this, Dialog::RestartDevice);
			shownChildMenu->show();

			restartBlinking(0);
			return false;
		}
	}
#endif
	EEPROM_SaveSystemData();

	return true;
}

AbstractMenu* SystemMenu::create(AbstractMenu* parent)
{
	SystemMenu* systemMenu = new SystemMenu(parent, MENU_SYSTEM);

	const uint8_t paramNum = 16;
	BaseParam* params[paramNum];

	params[0] = new StringListParam(SysSettingsDesc.cabMode, {"CabSim On ", "CabSim Off"}, 10);
	params[0]->setDisplayPosition(36);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, SysSettingsDesc.midiChannel);
	params[1]->setDisplayPosition(60);
	params[1]->setBounds(0, 15);
	params[1]->setScaling(1, 1);
#ifdef __MONO_MOD__
	params[2] = new StringListParam(SysSettingsDesc.cabNum,
				{"1 L+R", "1R AP", "2 L+R", "1R A ", "1R P ", " 1 R "}, 6);
#endif

#ifdef __STEREO_MOD__
	params[2] = new StringListParam(SysSettingsDesc.cabNum,
				{"2 L+R","1R AP","1R A ","1R P "," 1 R "}, 6);
#endif
	params[2]->setDisplayPosition(60);

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Expression", &sys_para[System::EXPR_TYPE]);
	customParam->decreaseCallback = expressionDescrease;
	customParam->increaseCallback = expressionIncrease;
	customParam->printCallback = expressionPrint;
	customParam->keyDownCallback = expressionKeyDown;
	params[3] = customParam;

	params[4] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Footswitch", &SystemMenu::createFootswitchMenu, systemMenu);

	params[5] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "MIDI PC MAP", &SystemMenu::createMidiPcMapMenu, systemMenu);

	customParam = new CustomParam(CustomParam::TDisplayType::String, "Tempo", &sys_para[System::TAP_TYPE]);
	customParam->setStrings({"Preset   ", "Global   ", "Glob+MIDI"}, 10);
	customParam->setDisplayPosition(leftPad + 6 * 6);
	customParam->decreaseCallback = tempoDecrease;
	customParam->increaseCallback = tempoIncrease;
	params[6] = customParam;

	params[7] = new StringListParam("TAP Popup", &sys_para[System::TAP_SCREEN_POPUP], {"On ", "Off"}, 4);
	params[7]->setDisplayPosition(leftPad + 6 * 10);

	params[8] = new StringListParam(SysSettingsDesc.spdif, {"Main Output", " Dry Input "}, 12);
	params[8]->setDisplayPosition(44);

	customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Tuner contr", &sys_para[System::TUNER_EXTERNAL]);
	customParam->decreaseCallback = tunerExtDescrease;
	customParam->increaseCallback = tunerExtIncrease;
	customParam->printCallback = tunerExtPrint;
	customParam->keyDownCallback = tunerExtKeyDown;
	params[9] = customParam;

	params[10] = new StringListParam(SysSettingsDesc.timeFormat, {"Sec", "BPM"}, 3);
	params[10]->setDisplayPosition(39);

	params[11] = new StringListParam(SysSettingsDesc.swapConfig, {"Off", "On "}, 3);
	params[11]->setDisplayPosition(78);

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Speed tun", &sys_para[System::TUNER_SPEED]);
	customParam->setDisplayPosition(78);
	customParam->decreaseCallback = tunerSpeedDescrease;
	customParam->increaseCallback = tunerSpeedIncrease;
	params[12] = customParam;

	params[13] = new StringListParam("EQ View", &sys_para[System::EQ_SCREEN_MODE], {"Graph ", "Bar  "}, 5);
	params[13]->setDisplayPosition(leftPad + 6 * 12);

	params[14] = new StringListParam(MasterEqDesc.on, {"Off", "On "}, 4);
	params[14]->setDisplayPosition(leftPad + 6 * 12);

	params[15] = new StringListParam(AttenuatorDesc.source, {"Global ", "Preset"}, 7);
	params[15]->setDisplayPosition(leftPad + 6 * 12);

	systemMenu->setParams(params, paramNum);

	return systemMenu;
}

AbstractMenu* SystemMenu::createFootswitchMenu(AbstractMenu* parent)
{
	return new FswTypeMenu(parent);
}

AbstractMenu* SystemMenu::createMidiPcMapMenu(AbstractMenu* parent)
{
	return new MidiMapMenu(parent);
}

void SystemMenu::expressionPrint(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80) DisplayTask->StringOut(78, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"On >");
	else DisplayTask->StringOut(78, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Off ");
}

void SystemMenu::expressionDescrease(void* parameter)
{
	SysSettingsDesc.exprOn.setterHandler(0);
}

void SystemMenu::expressionIncrease(void* parameter)
{
	SysSettingsDesc.exprOn.setterHandler(1);
}

void SystemMenu::expressionKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80)
	{
		currentMenu->showChild(new ExpressionMenu(currentMenu));
	}
}

void SystemMenu::tunerExtPrint(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80) DisplayTask->StringOut(78, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"On >");
	else DisplayTask->StringOut(78, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Off " );
}

void SystemMenu::tunerExtDescrease(void* parameter)
{
	SysSettingsDesc.tunerCtrl.setterHandler(0);
}

void SystemMenu::tunerExtIncrease(void* parameter)
{
	SysSettingsDesc.tunerCtrl.setterHandler(1);
}

void SystemMenu::tunerExtKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80)
	{
		currentMenu->showChild(new TunerExtMenu(currentMenu));
	}
}

void SystemMenu::tempoDecrease(void* parameter)
{
	sys_para[System::GLOBAL_TAP_TIME] = 0;
	if(sys_para[System::TAP_TYPE] > 0) sys_para[System::TAP_TYPE]--;
	SysSettingsDesc.tempo.setterHandler(sys_para[System::TAP_TYPE]);
}

void SystemMenu::tempoIncrease(void* parameter)
{
	sys_para[System::GLOBAL_TAP_TIME] = 0;
	if(sys_para[System::TAP_TYPE] < 2) sys_para[System::TAP_TYPE]++;
	SysSettingsDesc.tempo.setterHandler(sys_para[System::TAP_TYPE]);
}

void SystemMenu::tunerSpeedDescrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr > 0)
	{
		*valuePtr = BaseParam::encSpeedDec(*valuePtr, 0);
		SysSettingsDesc.tunerSpeed.setterHandler(*valuePtr);
	}
}

void SystemMenu::tunerSpeedIncrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr < 127)
	{
		*valuePtr = BaseParam::encSpeedInc(*valuePtr, 127);
		System::tun_del_val = (127-*valuePtr)*(90.0f/127.0f)+10.0f;
		SysSettingsDesc.tunerSpeed.setterHandler(*valuePtr);
	}
}
