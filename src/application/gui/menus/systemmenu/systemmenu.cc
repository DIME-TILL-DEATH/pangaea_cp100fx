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


static TParamDescriptor tapPopupDesc = {
	.ptr = &sys_para[System::TAP_SCREEN_POPUP],
	.handlerStr = "",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "TAP Popup",
	.setterHandler = nullptr
};

static TParamDescriptor eqViewDesc = {
	.ptr = &sys_para[System::EQ_SCREEN_MODE],
	.handlerStr = "",
	.dspAddress = DSP_NOT_USED,
	.dspPosition = NOT_SEND_POS,
	.name = "EQ view",
	.setterHandler = nullptr
};

AbstractMenu* SystemMenu::create(AbstractMenu* parent)
{
	SystemMenu* systemMenu = new SystemMenu(parent, MENU_SYSTEM);
#ifdef __MONO_MOD__
	const uint8_t paramNum = 16;
#endif

#ifdef __STEREO_MOD__
	const uint8_t paramNum = 15;
#endif
	BaseParam* params[paramNum];

	params[0] = new StringListParam(&SysSettingsDesc.cabMode, {"CabSim On ", "CabSim Off"}, 10);
	params[0]->setDisplayPosition(36);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, &SysSettingsDesc.midiChannel);
	params[1]->setDisplayPosition(60);
	params[1]->setBounds(0, 15);
	params[1]->setScaling(1, 1);
#ifdef __MONO_MOD__
	params[2] = new StringListParam(&SysSettingsDesc.cabNum,
				{"1 L+R", "1R AP", "2 L+R", "1R A ", "1R P ", " 1 R "}, 6);
#endif

#ifdef __STEREO_MOD__
	params[2] = new StringListParam(&SysSettingsDesc.cabNum,
				{"2 L+R","1R AP","1R A ","1R P "," 1 R "}, 6);
#endif
	params[2]->setDisplayPosition(60);

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, &SysSettingsDesc.exprOn);
	customParam->decreaseCallback = expressionDescrease;
	customParam->increaseCallback = expressionIncrease;
	customParam->printCallback = expressionPrint;
	customParam->keyDownCallback = expressionKeyDown;
	params[3] = customParam;

	params[4] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Footswitch", &FswTypeMenu::create, systemMenu);
	params[5] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "MIDI PC MAP", &SystemMenu::createMidiPcMapMenu, systemMenu);

	params[6] = new StringListParam(&SysSettingsDesc.tempo, {"Preset   ", "Global   ", "Glob+MIDI"}, 10);
	params[6]->setDisplayPosition(leftPad + 6 * 6);

	params[7] = new StringListParam(&tapPopupDesc, {"On ", "Off"}, 4);
	params[7]->setDisplayPosition(leftPad + 6 * 10);

	params[8] = new StringListParam(&SysSettingsDesc.spdif, {"Main Output", " Dry Input "}, 12);
	params[8]->setDisplayPosition(44);

	customParam = new CustomParam(CustomParam::TDisplayType::Custom, &SysSettingsDesc.tunerCtrl);
	customParam->decreaseCallback = tunerExtDescrease;
	customParam->increaseCallback = tunerExtIncrease;
	customParam->printCallback = tunerExtPrint;
	customParam->keyDownCallback = tunerExtKeyDown;
	params[9] = customParam;

	params[10] = new StringListParam(&SysSettingsDesc.timeFormat, {"Sec", "BPM"}, 3);
	params[10]->setDisplayPosition(39);

	params[11] = new StringListParam(&SysSettingsDesc.swapConfig, {"Off", "On "}, 3);
	params[11]->setDisplayPosition(78);

	params[12] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, &SysSettingsDesc.tunerSpeed);
	params[12]->setDisplayPosition(78);

	params[13] = new StringListParam(&eqViewDesc, {"Graph ", "Bar  "}, 5);
	params[13]->setDisplayPosition(leftPad + 6 * 12);

	params[14] = new StringListParam(&MasterEqDesc.on, {"Off", "On "}, 4);
	params[14]->setDisplayPosition(leftPad + 6 * 12);

#ifdef __MONO_MOD__
	params[15] = new StringListParam(&AttenuatorDesc.source, {"Global ", "Preset"}, 7);
	params[15]->setDisplayPosition(leftPad + 6 * 12);
#endif

	systemMenu->setParams(params, paramNum);

	return systemMenu;
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
	*(uint8_t*)(SysSettingsDesc.exprOn.ptr) &= 0x7F;
}

void SystemMenu::expressionIncrease(void* parameter)
{
	*(uint8_t*)(SysSettingsDesc.exprOn.ptr) |= 0x80;
}

void SystemMenu::expressionKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80)
	{
		currentMenu->showChild(ExpressionMenu::create(currentMenu));
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
	*(uint8_t*)(SysSettingsDesc.tunerCtrl.ptr) &= 0x7F;
}

void SystemMenu::tunerExtIncrease(void* parameter)
{
	*(uint8_t*)(SysSettingsDesc.tunerCtrl.ptr) |= 0x80;
}

void SystemMenu::tunerExtKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr & 0x80)
	{
		currentMenu->showChild(new TunerExtMenu(currentMenu));
	}
}
