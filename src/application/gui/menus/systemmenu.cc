#include "systemmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "params/baseparam.h"
#include "params/stringparam.h"
#include "params/customparam.h"
#include "params/submenuparam.h"

SystemMenu::SystemMenu(AbstractMenu* parent, gui_menu_type menuType)
		:ParamListMenu(parent, menuType)
{

}

//
//void SystemMenu::encoderPressed()
//{
//
//}
//
//void SystemMenu::encoderClockwise()
//{
//
//}

//void SystemMenu::encoderCounterClockwise()
//{
//
//}

void SystemMenu::keyDown()
{

}

//const uint8_t sys_menu_list[][12] = {"Mode", "MIDI ch", "Cab num", "Expression", "Footswitch", "S/PDIF", "MIDI PC Map", "Tempo", "Tuner contr", "Time",
//		"Swap UpConf", "Speed tun"};
//const uint8_t expr_menu[][10] = {"Type", "Calibrate", "CC#", "Store Lev"};

//const uint8_t expr_type[][12] = {"   Off     ", "Standard V ", "Alternat V ", "Standard CC", "Alternat CC"};
//const uint8_t footsw_menu[][12] = {"FSW-DOWN", "FSW-CONFIRM", "FSW-UP", "Speed FS"};
//const uint8_t ext_switch[][12] = {"Off        ", "Expression ", "Foot switch"};
//const uint8_t int_sw_list[][12] = {"True bypass", "Controllers", "Presets sw "};
//const uint8_t contr_ext_l[][12] = {"Expression ", " FSW DOWN  ", "FSW CONFIRM", "  FSW UP   "};
//const uint8_t fsw_t[][12] = {"Default    ", "Controller ", "Tuner      ", "Preset Map1", "Preset Map2", "Preset Map3", "Preset Map4"};
//const uint8_t tempo_type[][10] = {"Preset   ", "Global   ", "Glob+MIDI"};


AbstractMenu* SystemMenu::create(AbstractMenu* parent)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	params[0] = new StringParam("Mode", &sys_para[CAB_SIM_DISABLED], {"CabSim On ", "CabSim Off"}, 10);
	params[0]->setDspAddress(DSP_ADDRESS_CAB_DRY_MUTE, PARAM_EQUAL_POS);
	params[0]->setDisplayPosition(36);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "MIDI ch", &sys_para[MIDI_CHANNEL]);
	params[1]->setDisplayPosition(60);
	params[1]->setBounds(0, 16);
	params[1]->setScaling(1, 1);

	params[2] = new StringParam("Cab num", &sys_para[CAB_SIM_CONFIG],
				{"1 L+R", "1R AP", "2 L+R", "1R A ", "1R P ", " 1 R "}, 6);
	params[2]->setDspAddress(DSP_ADDRESS_CAB_CONFIG, PARAM_EQUAL_POS);
	params[2]->setDisplayPosition(60);

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, "Expression", &sys_para[EXPRESSION_TYPE]);
	customParam->decreaseCallback = expressionDescrease;
	customParam->increaseCallback = expressionIncrease;
	customParam->printCallback = expressionPrint;
	params[3] = customParam;

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Footswitch", &sys_para[EXPRESSION_TYPE]);
//	customParam->decreaseCallback = expressionDescrease;
//	customParam->increaseCallback = expressionIncrease;
//	customParam->printCallback = expressionPrint;
	params[4] = customParam;

	params[5] = new StringParam("S/PDIF", &sys_para[SPDIF_OUT_TYPE], {"Main Output", " Dry Input "}, 12);
	params[5]->setDspAddress(DSP_ADDRESS_SPDIF, PARAM_EQUAL_POS);
	params[5]->setDisplayPosition(44);

	SubmenuParam* midiMapParam = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "MIDI PC MAP", &SystemMenu::create, nullptr);
	params[6] = midiMapParam;

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Tempo", &sys_para[EXPRESSION_TYPE]);
//	customParam->decreaseCallback = expressionDescrease;
//	customParam->increaseCallback = expressionIncrease;
//	customParam->printCallback = expressionPrint;
	params[7] = customParam;

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Tuner contr", &sys_para[EXPRESSION_TYPE]);
//	customParam->decreaseCallback = expressionDescrease;
//	customParam->increaseCallback = expressionIncrease;
//	customParam->printCallback = expressionPrint;
	params[8] = customParam;


	params[9] = new StringParam("Time", &sys_para[TIME_FORMAT], {"Sec", "BPM"}, 3);
	params[9]->setDisplayPosition(39);

	params[10] = new StringParam("Swap UpConf", &sys_para[SWAP_SWITCH], {"Off", "On "}, 3);
	params[10]->setDisplayPosition(78);

	customParam = new CustomParam(CustomParam::TDisplayType::Number, "Speed tun", &sys_para[EXPRESSION_TYPE]);
//	customParam->decreaseCallback = expressionDescrease;
//	customParam->increaseCallback = expressionIncrease;
//	customParam->printCallback = expressionPrint;
	params[11] = customParam;

	SystemMenu* systemMenu = new SystemMenu(parent, MENU_SYSTEM);
	systemMenu->setParams(params, paramNum, 3);

	return systemMenu;
}

void SystemMenu::expressionPrint(void* parameter)
{
	uint8_t* valuePtr = static_cast<uint8_t*>(parameter);
	if(*valuePtr&0x80) DisplayTask->StringOut(78, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Off ");
	else DisplayTask->StringOut(78, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"On >");
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


