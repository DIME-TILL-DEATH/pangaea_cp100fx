#include "systemmenu.h"


#include "appdefs.h"
#include "cs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "midimapmenu.h"
#include "expressionmenu.h"
#include "fswtypemenu.h"

#include "baseparam.h"
#include "stringlistparam.h"
#include "submenuparam.h"
#include "customparam.h"

SystemMenu::SystemMenu(AbstractMenu* parent, gui_menu_type menuType)
		:ParamListMenu(parent, menuType)
{
	m_previousCabConfig = sys_para[CAB_SIM_CONFIG];
}


void SystemMenu::encoderPressed()
{
	if(m_paramsList[m_currentParamNum]->disabled()) return;


	if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_SUBMENU)
	{
		SubmenuParam* submenuParam = static_cast<SubmenuParam*>(m_paramsList[m_currentParamNum]);
		submenuParam->showSubmenu(this);
	}
	else
	{
		if(!m_encoderKnobSelected)
		{
			m_encoderKnobSelected = true;
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
									2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
		else
		{
			m_encoderKnobSelected = false;
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
									0, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
	}

	tim5_start(1);
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
	if(m_previousCabConfig == 2)
	{
		if(m_previousCabConfig!=sys_para[CAB_SIM_CONFIG])
		{
			write_sys();
			NVIC_SystemReset();
		}
	}
	else
	{
		if(sys_para[CAB_SIM_CONFIG] == 2)
		{
			write_sys();
			NVIC_SystemReset();
		}
	}
	write_sys();

	return true;
}

AbstractMenu* SystemMenu::create(AbstractMenu* parent)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	params[0] = new StringListParam("Mode", &sys_para[CAB_SIM_DISABLED], {"CabSim On ", "CabSim Off"}, 10);
	params[0]->setDspAddress(DSP_ADDRESS_CAB_DRY_MUTE, PARAM_EQUAL_POS);
	params[0]->setDisplayPosition(36);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "MIDI ch", &sys_para[MIDI_CHANNEL]);
	params[1]->setDisplayPosition(60);
	params[1]->setBounds(0, 15);
	params[1]->setScaling(1, 1);

	params[2] = new StringListParam("Cab num", &sys_para[CAB_SIM_CONFIG],
				{"1 L+R", "1R AP", "2 L+R", "1R A ", "1R P ", " 1 R "}, 6);
	params[2]->setDspAddress(DSP_ADDRESS_CAB_CONFIG, PARAM_EQUAL_POS);
	params[2]->setDisplayPosition(60);

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Expression", &sys_para[EXPR_TYPE]);
	customParam->decreaseCallback = expressionDescrease;
	customParam->increaseCallback = expressionIncrease;
	customParam->printCallback = expressionPrint;
	customParam->keyDownCallback = expressionKeyDown;
	params[3] = customParam;

	params[4] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Footswitch", &SystemMenu::createFootswitchMenu, nullptr);

	params[5] = new StringListParam("S/PDIF", &sys_para[SPDIF_OUT_TYPE], {"Main Output", " Dry Input "}, 12);
	params[5]->setDspAddress(DSP_ADDRESS_SPDIF, PARAM_EQUAL_POS);
	params[5]->setDisplayPosition(44);

	params[6] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "MIDI PC MAP", &SystemMenu::createMidiPcMapMenu, nullptr);

	customParam = new CustomParam(CustomParam::TDisplayType::String, "Tempo", &sys_para[TAP_TYPE]);
	customParam->setStrings({"Preset   ", "Global   ", "Glob+MIDI"}, 10);
	customParam->setDisplayPosition(38);
	customParam->decreaseCallback = tempoDecrease;
	customParam->increaseCallback = tempoIncrease;
	params[7] = customParam;

	customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Tuner contr", &sys_para[TUNER_EXTERNAL]);
	customParam->decreaseCallback = tunerExtDescrease;
	customParam->increaseCallback = tunerExtIncrease;
	customParam->printCallback = tunerExtPrint;
	customParam->keyDownCallback = tunerExtKeyDown;
	params[8] = customParam;

	params[9] = new StringListParam("Time", &sys_para[TIME_FORMAT], {"Sec", "BPM"}, 3);
	params[9]->setDisplayPosition(39);

	params[10] = new StringListParam("Swap UpConf", &sys_para[SWAP_SWITCH], {"Off", "On "}, 3);
	params[10]->setDisplayPosition(78);

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Speed tun", &sys_para[TUNER_SPEED]);
	customParam->setDisplayPosition(78);
	customParam->decreaseCallback = tunerSpeedDescrease;
	customParam->increaseCallback = tunerSpeedIncrease;
	params[11] = customParam;

	SystemMenu* systemMenu = new SystemMenu(parent, MENU_SYSTEM);
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
	if(*valuePtr&0x80) DisplayTask->StringOut(78, 3, Font::fntSystem, 0, (uint8_t*)"On >");
	else DisplayTask->StringOut(78, 3, Font::fntSystem, 0, (uint8_t*)"Off ");
}

void SystemMenu::expressionDescrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	adc_init(0);
	*valuePtr &= 0x7f;
	ext_send(127);
}

void SystemMenu::expressionIncrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	*valuePtr |= 0x80;
	adc_init(1);
}

void SystemMenu::expressionKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80)
	{
		ExpressionMenu* expressionMenu = new ExpressionMenu(currentMenu);
		currentMenu->showChild(expressionMenu);
	}
}

void SystemMenu::tunerExtPrint(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80) DisplayTask->StringOut(78, 0, Font::fntSystem, 0, (uint8_t*)"On >");
	else DisplayTask->StringOut(78, 0, Font::fntSystem, 0, (uint8_t*)"Off " );
}

void SystemMenu::tunerExtDescrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80) sys_para[TUNER_EXTERNAL] &= ~0x80;
}

void SystemMenu::tunerExtIncrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(!(*valuePtr&0x80)) sys_para[TUNER_EXTERNAL] |= 0x80;
}

void SystemMenu::tunerExtKeyDown(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80)
	{
		BaseParam* ccParam = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "    CC#", &sys_para[TUNER_EXTERNAL]);
		ccParam->setScaling(1, -128);
//		ccParam->setBounds(129, 255);
		ccParam->setBounds(-127, -1);

		ParamListMenu* ccMenu = new ParamListMenu(currentMenu, MENU_TUNER_EXT);
		if(ccMenu)
		{
			ccMenu->setParams(&ccParam, 1);

			ccMenu->setIcon(false, ICON_NONE);
			currentMenu = ccMenu;
			currentMenu->show();
		}
	}
}

void SystemMenu::tempoDecrease(void* parameter)
{
	sys_para[TAP_HIGH] = 0;
	if(sys_para[TAP_TYPE] > 0) sys_para[TAP_TYPE]--;
	DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[TAP_HIGH]);
}

void SystemMenu::tempoIncrease(void* parameter)
{
//	sys_para[tap_hi] = 0; //???
	if(sys_para[TAP_TYPE] < 2) sys_para[TAP_TYPE]++;
	DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[TAP_HIGH]);
}

void SystemMenu::tunerSpeedDescrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr > 0)
	{
		*valuePtr = BaseParam::encSpeedDec(*valuePtr, 0);
		tun_del_val = (127-*valuePtr)*(90.0f/127.0f)+10.0f;
	}
}

void SystemMenu::tunerSpeedIncrease(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr < 127)
	{
		*valuePtr = BaseParam::encSpeedInc(*valuePtr, 127);
		tun_del_val = (127-*valuePtr)*(90.0f/127.0f)+10.0f;
	}
}
