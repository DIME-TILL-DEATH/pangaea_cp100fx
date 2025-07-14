#include "guimodules.h"

#include "eepr.h"

#include "abstractmenu.h"
#include "eqmenu.h"
#include "cabbrowsermenu.h"
#include "paramlistmenu.h"

#include "submenuparam.h"
#include "stringoutparam.h"

#include "preset.h"

extern uint8_t cab_type;
extern uint8_t name_run_fl;

AbstractMenu* GuiModules::createRfMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[RFILTER_MIX]);
	params[1] = new StringListParam("F Type", &currentPreset.modules.rawData[RFILTER_FTYPE], {"LPF","HPF","BPF"}, 3);

	StringListParam* typeSelect = new StringListParam("F Mod", &currentPreset.modules.rawData[RFILTER_FMOD], {"LFO", "Dyn", "Ext"}, 3);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	typeSelect->setDisableMask(2, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	params[2] = typeSelect;

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO r", &currentPreset.modules.rawData[RFILTER_RATE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Lo freq", &currentPreset.modules.rawData[RFILTER_LPF]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hi freq", &currentPreset.modules.rawData[RFILTER_HPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Res", &currentPreset.modules.rawData[RFILTER_RESONANCE]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn th", &currentPreset.modules.rawData[RFILTER_DYN_THRESHOLD]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn att", &currentPreset.modules.rawData[RFILTER_DYN_ATTACK]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn rel", &currentPreset.modules.rawData[RFLITER_DYN_RELEASE]);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &currentPreset.modules.rawData[RFILTER_VOLUME]);
	params[11] = new StringListParam("F Mod", &currentPreset.modules.rawData[RFILTER_LFO_TYPE],
			{"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"}, 7);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_RESONANCE_FILTER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_RESONANCE_FILTER);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createGateMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramCount = 3;
	BaseParam* params[paramCount];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &currentPreset.modules.rawData[GATE_THRESHOLD]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &currentPreset.modules.rawData[GATE_ATTACK]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Decay", &currentPreset.modules.rawData[GATE_DECAY]);

	for(int i=0; i<paramCount; i++) params[i]->setDspAddress(DSP_ADDRESS_GATE, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_GATE);
	if(menu) menu->setParams(params, paramCount);

	return menu;
}

AbstractMenu* GuiModules::createCompressorMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &currentPreset.modules.rawData[COMPRESSOR_THRESHOLD]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Ratio", &currentPreset.modules.rawData[COMPRESSOR_RATIO]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &currentPreset.modules.rawData[COMPRESSOR_VOLUME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &currentPreset.modules.rawData[COMPRESSOR_ATTACK]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Release", &currentPreset.modules.rawData[COMPRESSOR_RELEASE]);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_COMPRESSOR, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_COMPRESSOR);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createPreampMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 7;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Gain", &currentPreset.modules.rawData[PREAMP_GAIN]);
	params[0]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &currentPreset.modules.rawData[PREAMP_VOLUME]);
	params[1]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &currentPreset.modules.rawData[PREAMP_0]);
	params[2]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &currentPreset.modules.rawData[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Low", &currentPreset.modules.rawData[PREAMP_LOW]);
	params[4]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Mid", &currentPreset.modules.rawData[PREAMP_MID]);
	params[5]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_MID_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "High", &currentPreset.modules.rawData[PREAMP_HIGH]);
	params[6]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PREAMP);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_PREAMP);
	}

	return menu;
}

AbstractMenu* GuiModules::createAmpMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Master", &currentPreset.modules.rawData[AMP_MASTER]);
	params[0]->setDspAddress(DSP_ADDRESS_AMP, AMP_MASTER_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Presence", &currentPreset.modules.rawData[EQ_PRESENCE]);
	params[1]->setDspAddress(DSP_ADDRESS_EQ, EQ_PRESENCE_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &currentPreset.modules.rawData[AMP_LEVEL]);
	params[2]->setDspAddress(DSP_ADDRESS_AMP, AMP_LEVEL_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &currentPreset.modules.rawData[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_AMP, NOT_SEND_POS);
	params[4] = new StringListParam("Type", &currentPreset.modules.rawData[AMP_TYPE],
		 {"  PP 6L6  ", "  PP EL34 ", "  SE 6L6  ", "  SE EL34 ", " AMT TC-3 ",
		  "California",	"British M ", "British L ", "    Flat   ","Calif mod ",
		  "Calif vint", "PVH PR0RS0", "PVH PR5RS5", "PVH PR8RS7"}, 11);
	params[4]->setDspAddress(DSP_ADDRESS_AMP_TYPE, AMP_TYPE_POS);
	params[4]->setDisplayPosition(36);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PA);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_AMP);
	}

	return menu;
}

AbstractMenu* GuiModules::createIrMenu(AbstractMenu* parentMenu)
{
	AbstractMenu* menu;
	if(cab_type==2)
	{
		const uint8_t paramNum = 2;
		BaseParam* params[paramNum];

		params[0] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet 1", &GuiModules::createCab1Menu, nullptr);
		params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet 2", &GuiModules::createCab2Menu, nullptr);

		ParamListMenu* paramsMenu = new ParamListMenu(parentMenu, MENU_CABTYPE);
		paramsMenu->setParams(params, paramNum);
		paramsMenu->setIcon(false, ICON_NONE);

		menu = paramsMenu;
	}
	else
	{
		menu = GuiModules::createCab1Menu(parentMenu);
	}

	return menu;
}

AbstractMenu* GuiModules::createCab1Menu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	params[0] = new StringOutParam(cab1.name);
	params[0]->setDisplayPosition(ParamListMenu::leftPad);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Browser", &GuiModules::createCab1BrowserMenu);

	if(cab_type==2)
	{
		params[2] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "Pan", &currentPreset.modules.rawData[IR_PAN1]);
		params[2]->setDspAddress(DSP_ADDRESS_CAB, IR_PAN1_POS);
	}
	else params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", nullptr);


	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_VOLUME, "Volume", &currentPreset.modules.rawData[IR_VOLUME1]);
	params[3]->setDisplayPosition(42);
	params[3]->setDspAddress(DSP_ADDRESS_CAB, IR_VOLUME1_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CABSIM);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB1);
		menu->setIcon(false, ICON_NONE);

		StringOutParam* runningString = static_cast<StringOutParam*>(params[0]);
		runningString->setRunning(true, menu);
	}

	return menu;
}

AbstractMenu* GuiModules::createCab2Menu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	params[0] = new StringOutParam(cab2.name);
	params[0]->setDisplayPosition(ParamListMenu::leftPad);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Browser", &GuiModules::createCab2BrowserMenu);

	if(cab_type==2)
	{
		params[2] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "Pan", &currentPreset.modules.rawData[IR_PAN2]);
		params[2]->setDspAddress(DSP_ADDRESS_CAB, IR_PAN2_POS);
	}
	else params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", nullptr);


	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_VOLUME, "Volume", &currentPreset.modules.rawData[IR_VOLUME2]);
	params[3]->setDspAddress(DSP_ADDRESS_CAB, IR_VOLUME2_POS);
	params[3]->setDisplayPosition(42);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CABSIM);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB2);
		menu->setIcon(false, ICON_NONE);

		StringOutParam* runningString = static_cast<StringOutParam*>(params[0]);
		menu->setRunningString(runningString);
	}

	return menu;
}

AbstractMenu* GuiModules::createCab1BrowserMenu(AbstractMenu* parentMenu)
{
	return new CabBrowserMenu(parentMenu, 0);
}

AbstractMenu* GuiModules::createCab2BrowserMenu(AbstractMenu* parentMenu)
{
	return new CabBrowserMenu(parentMenu, 1);
}

AbstractMenu* GuiModules::createEqMenu(AbstractMenu* parentMenu)
{
	return new EqMenu(parentMenu);
}

// При режиме детюн параметр Rate должен называться Detune
AbstractMenu* GuiModules::createPhaserMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[PHASER_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &currentPreset.modules.rawData[PHASER_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Center", &currentPreset.modules.rawData[PHASER_CENTER]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &currentPreset.modules.rawData[PHASER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &currentPreset.modules.rawData[PHASER_FEEDBACK]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "Stage", &currentPreset.modules.rawData[PHASER_TYPE]);
	params[5]->setScaling(2, 4);
	params[5]->setBounds(0, 4);
	params[5]->setDisplayPosition(62);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hpf", &currentPreset.modules.rawData[PHASER_HPF]);
	params[7] = new StringListParam("Pos.", &currentPreset.modules.rawData[PHASER_PREPOST], {"Post", "Pre "}, 4);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_PHASER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PHASER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createFlangerMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[FLANGER_MIX]);
	params[1] = new StringListParam("LFO", &currentPreset.modules.rawData[FLANGER_LFO], {"Triangle", "Sinus   "}, 8);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &currentPreset.modules.rawData[FLANGER_RATE]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &currentPreset.modules.rawData[FLANGER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &currentPreset.modules.rawData[FLANGER_DELAY]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &currentPreset.modules.rawData[FLANGER_FEEDBACK]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &currentPreset.modules.rawData[FLANGER_HPF]);
	params[7] = new StringListParam("Pos.", &currentPreset.modules.rawData[FLANGER_LFO], {"Post", "Pre "}, 4);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_FLANGER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_FLANGER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createChorusMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[CHORUS_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &currentPreset.modules.rawData[CHORUS_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &currentPreset.modules.rawData[CHORUS_WIDTH]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &currentPreset.modules.rawData[CHORUS_DELAY]);
	StringListParam* typeSelect = new StringListParam("Type", &currentPreset.modules.rawData[CHORUS_TYPE],
			{"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"}, 12);
	typeSelect->setDisplayPosition(36);
	typeSelect->setDisableMask(4, {0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 1, 1, 0, 0, 0});
	params[4] = typeSelect;
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &currentPreset.modules.rawData[CHORUS_HPF]);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_CHORUS, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CHORUS);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createEarlyMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramCout = 2;
	BaseParam* params[paramCout];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[EARLY_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &currentPreset.modules.rawData[EARLY_SIZE]);

	for(int i=0; i<paramCout; i++) params[i]->setDspAddress(DSP_ADDRESS_EARLY_REFLECTIONS, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_EARLY_REFLECTIONS);
	if(menu) menu->setParams(params, paramCout);

	return menu;
}

AbstractMenu* GuiModules::createDelayMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	ParamListMenu* menu;

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[DELAY_MIX]);
	params[0]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MIX_POS);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME, "Time", &GuiModules::createDelayTapMenu, &delay_time);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS);
	params[1]->setByteSize(2);
	params[1]->setBounds(10, 2730);
	params[1]->setScaling(10, 0);

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &currentPreset.modules.rawData[DELAY_FEEDBACK]);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &currentPreset.modules.rawData[DELAY_LPF]);
	params[3]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_LPF_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &currentPreset.modules.rawData[DELAY_HPF]);
	params[4]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_HPF_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D1 Pan", &currentPreset.modules.rawData[DELAY_PAN1]);
	params[5]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN1_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D2 Vol", &currentPreset.modules.rawData[DELAY_VOLUME2]);
	params[6]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_VOLUME2_POS);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D2 Pan", &currentPreset.modules.rawData[DELAY_PAN2]);
	params[7]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN2_POS);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D->D2", &currentPreset.modules.rawData[DELAY_OFFSET]);
	params[8]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_OFFSET_POS);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D_Mod", &currentPreset.modules.rawData[DELAY_MODULATION]);
	params[9]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_POS);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "M_Rate", &currentPreset.modules.rawData[DELAY_MODULATION_RATE]);
	params[10]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_RATE_POS);
	params[11] = new StringListParam("Direct", &currentPreset.modules.rawData[DELAY_DIRECTION], {"Forward", "Reverse"}, 7);
	params[11]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_DIRECTION_POS);

	menu = new ParamListMenu(parentMenu, MENU_DELAY);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}


//constexpr static uint8_t tap_tim       [][6] ={"1/1  ","1/1.5","1/2  ","1/3  ","1/4  ","2/1  "};
//constexpr static float   tap_tim_v     [6]   ={1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};

// Отображение в BPM
//uint16_t temp = 60000 / delay_time;
//if(temp < 240)
//{
//	temp = enc_speed_inc(temp , 240);
//	delay_time = 60000 / temp;
//	DisplayTask->ParamIndicNum(53,0,temp);
//	DisplayTask->StringOut(90,0,Font::fntSystem , 0 , (uint8_t*)"BPM");
//}
//else {
//	temp = 240;
//	delay_time = 60000 / temp;
//}

AbstractMenu* GuiModules::createDelayTapMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 3;
	BaseParam* params[paramNum];

	ParamListMenu* menu;

	// Отдельный класс для delay time
	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_DELAY_TIME, "Time", &delay_time);
	params[0]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS);
	params[0]->setByteSize(2);
	params[0]->setBounds(10, 2730);
	params[0]->setScaling(10, 0);

	params[1] = new StringListParam("TAP", &currentPreset.modules.rawData[TREMOLO_TAP],
			{"1/1  ", "1/1.5" ,"1/2  " ,"1/3  " ,"1/4  " ,"2/1  "}, 6);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, NOT_SEND_POS);

	params[2] = new StringListParam("Tail", &currentPreset.modules.rawData[REVERB_TAIL], {"On ", "Off"}, 3);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TAIL_POS);

	menu = new ParamListMenu(parentMenu, MENU_TAP_DELAY);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setIcon(true, ICON_DL);
	}

	return menu;
}

AbstractMenu* GuiModules::createReverbMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 11;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &currentPreset.modules.rawData[REVERB_MIX]);

	StringListParam* typeSelect = new StringListParam("Type", &currentPreset.modules.rawData[REVERB_TYPE],
			{"Default", "Hall   ", "Room   ", "Plate  ", "Spring ", "Gate   ", "Reverse"}, 8);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
	typeSelect->setDisableMask(4, {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	typeSelect->setDisableMask(6, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	params[1] = typeSelect;

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Time", &currentPreset.modules.rawData[REVERB_TIME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &currentPreset.modules.rawData[REVERB_SIZE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Damp", &currentPreset.modules.rawData[REVERB_DAMPING]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &currentPreset.modules.rawData[REVERB_LPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &currentPreset.modules.rawData[REVERB_HPF]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Detune", &currentPreset.modules.rawData[REVERB_DETUNE]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Diffus", &currentPreset.modules.rawData[REVERB_DIFFUSION]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "PreD", &currentPreset.modules.rawData[REVERB_PREDELAY]);
	params[10] = new StringListParam("Tail", &currentPreset.modules.rawData[REVERB_TAIL], {"On ", "Off"}, 3);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_REVERB, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_REVERB);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createTremoloMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Intens", &currentPreset.modules.rawData[TREMOLO_INTENSITY]);
	params[0]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &currentPreset.modules.rawData[TREMOLO_RATE]);
	params[1]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS);

	params[2] = new StringListParam("LFOType", &currentPreset.modules.rawData[TREMOLO_LFO_TYPE], {"Sin     ", "Square  ", "Sawtooth"}, 8);
	params[2]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_TYPE_POS);

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO mod", &currentPreset.modules.rawData[TREMOLO_LFO_MOD]);
	params[3]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_MOD_POS);

	params[4] = new StringListParam("Type", &currentPreset.modules.rawData[TREMOLO_MS], {"Mono  ", "Stereo"}, 6);
	params[4]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_MS_POS);

	params[5] = new StringListParam("TAP", &currentPreset.modules.rawData[TREMOLO_TAP],
			{"1    ", "1/1.5", "1/2  ", "1/3  ", "1/4  "}, 6);
	params[5]->setDspAddress(DSP_ADDRESS_TREMOLO, NOT_SEND_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_TREMOLO);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

