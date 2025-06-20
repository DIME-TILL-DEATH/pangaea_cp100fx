#include "guimodules.h"

#include "eepr.h"

#include "menus/abstractmenu.h"
#include "menus/eqmenu.h"
#include "menus/cabtypemenu.h"
#include "menus/paramlistmenu.h"

#include "menus/params/submenuparam.h"

AbstractMenu* GuiModules::createRfMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[RFILTER_MIX]);
	params[1] = new StringParam("F Type", &presetData[RFILTER_FTYPE], {"LPF","HPF","BPF"}, 3);

	StringParam* typeSelect = new StringParam("F Mod", &presetData[RFILTER_FMOD], {"LFO", "Dyn", "Ext"}, 3);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	typeSelect->setDisableMask(2, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	params[2] = typeSelect;

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO r", &presetData[RFILTER_RATE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Lo freq", &presetData[RFILTER_LPF]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hi freq", &presetData[RFILTER_HPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Res", &presetData[RFILTER_RESONANCE]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn th", &presetData[RFILTER_DYN_THRESHOLD]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn att", &presetData[RFILTER_DYN_ATTACK]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn rel", &presetData[RFLITER_DYN_RELEASE]);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &presetData[RFILTER_VOLUME]);
	params[11] = new StringParam("F Mod", &presetData[RFILTER_LFO_TYPE],
			{"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"}, 7);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_RESONANCE_FILTER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_RESONANCE_FILTER);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createGateMenu(AbstractMenu* parentMenu)
{
	const uint8_t gateParamNum = 3;
	BaseParam* gateParams[gateParamNum];

	gateParams[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &presetData[GATE_THRESHOLD]);
	gateParams[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &presetData[GATE_ATTACK]);
	gateParams[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Decay", &presetData[GATE_DECAY]);

	for(int i=0; i<gateParamNum; i++) gateParams[i]->setDspAddress(DSP_ADDRESS_GATE, i);

	ParamListMenu* gateMenu = new ParamListMenu(parentMenu, MENU_GATE);
	if(gateMenu) gateMenu->setParams(gateParams, gateParamNum);

	return gateMenu;
}

AbstractMenu* GuiModules::createCompressorMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &presetData[COMPRESSOR_THRESHOLD]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Ratio", &presetData[COMPRESSOR_RATIO]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &presetData[COMPRESSOR_VOLUME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &presetData[COMPRESSOR_ATTACK]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Release", &presetData[COMPRESSOR_RELEASE]);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_COMPRESSOR, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_COMPRESSOR);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createPreampMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 7;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Gain", &presetData[PREAMP_GAIN]);
	params[0]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &presetData[PREAMP_VOLUME]);
	params[1]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &presetData[PREAMP_0]);
	params[2]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &presetData[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Low", &presetData[PREAMP_LOW]);
	params[4]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Mid", &presetData[PREAMP_MID]);
	params[5]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_MID_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "High", &presetData[PREAMP_HIGH]);
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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Master", &presetData[AMP_MASTER]);
	params[0]->setDspAddress(DSP_ADDRESS_AMP, AMP_MASTER_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Presence", &presetData[EQ_PRESENCE]);
	params[1]->setDspAddress(DSP_ADDRESS_EQ, EQ_PRESENCE_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &presetData[AMP_LEVEL]);
	params[2]->setDspAddress(DSP_ADDRESS_AMP, AMP_LEVEL_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &presetData[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_AMP, NOT_SEND_POS);
	params[4] = new StringParam("Type", &presetData[AMP_TYPE],
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
	return new CabTypeMenu(parentMenu);
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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[PHASER_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &presetData[PHASER_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Center", &presetData[PHASER_CENTER]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &presetData[PHASER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &presetData[PHASER_FEEDBACK]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "Stage", &presetData[PHASER_TYPE]);
	params[5]->setScaling(2, 4);
	params[5]->setBounds(0, 4);
	params[5]->setDisplayPosition(62);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hpf", &presetData[PHASER_HPF]);
	params[7] = new StringParam("Pos.", &presetData[PHASER_PREPOST], {"Post", "Pre "}, 4);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_PHASER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PHASER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createFlangerMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[FLANGER_MIX]);
	params[1] = new StringParam("LFO", &presetData[FLANGER_LFO], {"Triangle", "Sinus   "}, 8);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &presetData[FLANGER_RATE]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &presetData[FLANGER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &presetData[FLANGER_DELAY]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &presetData[FLANGER_FEEDBACK]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &presetData[FLANGER_HPF]);
	params[7] = new StringParam("Pos.", &presetData[FLANGER_LFO], {"Post", "Pre "}, 4);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_FLANGER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_FLANGER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createChorusMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[CHORUS_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &presetData[CHORUS_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &presetData[CHORUS_WIDTH]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &presetData[CHORUS_DELAY]);
	StringParam* typeSelect = new StringParam("Type", &presetData[CHORUS_TYPE],
			{"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"}, 12);
	typeSelect->setDisplayPosition(36);
	typeSelect->setDisableMask(4, {0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 1, 1, 0, 0, 0});
	params[4] = typeSelect;
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &presetData[CHORUS_HPF]);

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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[EARLY_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &presetData[EARLY_SIZE]);

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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[DELAY_MIX]);
	params[0]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MIX_POS);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME, "Time", &GuiModules::createDelayTapMenu, &delay_time);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS);
	params[1]->setByteSize(2);
	params[1]->setBounds(10, 2730);
	params[1]->setScaling(10, 0);

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &presetData[DELAY_FEEDBACK]);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &presetData[DELAY_LPF]);
	params[3]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_LPF_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &presetData[DELAY_HPF]);
	params[4]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_HPF_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D1 Pan", &presetData[DELAY_PAN1]);
	params[5]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN1_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D2 Vol", &presetData[DELAY_VOLUME2]);
	params[6]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_VOLUME2_POS);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D2 Pan", &presetData[DELAY_PAN2]);
	params[7]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN2_POS);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D->D2", &presetData[DELAY_OFFSET]);
	params[8]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_OFFSET_POS);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D_Mod", &presetData[DELAY_MODULATION]);
	params[9]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_POS);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "M_Rate", &presetData[DELAY_MODULATION_RATE]);
	params[10]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_RATE_POS);
	params[11] = new StringParam("Direct", &presetData[DELAY_DIRECTION], {"Forward", "Reverse"}, 7);
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
//	DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
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

	params[1] = new StringParam("TAP", &presetData[TREMOLO_TAP],
			{"1/1  ", "1/1.5" ,"1/2  " ,"1/3  " ,"1/4  " ,"2/1  "}, 6);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, NOT_SEND_POS);

	params[2] = new StringParam("Tail", &presetData[REVERB_TAIL], {"On ", "Off"}, 3);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TAIL_POS);

	menu = new ParamListMenu(parentMenu, MENU_TAP_DELAY);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createReverbMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 11;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &presetData[REVERB_MIX]);

	StringParam* typeSelect = new StringParam("Type", &presetData[REVERB_TYPE],
			{"Default", "Hall   ", "Room   ", "Plate  ", "Spring ", "Gate   ", "Reverse"}, 8);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
	typeSelect->setDisableMask(4, {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	typeSelect->setDisableMask(6, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	params[1] = typeSelect;

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Time", &presetData[REVERB_TIME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &presetData[REVERB_SIZE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Damp", &presetData[REVERB_DAMPING]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &presetData[REVERB_LPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &presetData[REVERB_HPF]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Detune", &presetData[REVERB_DETUNE]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Diffus", &presetData[REVERB_DIFFUSION]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "PreD", &presetData[REVERB_PREDELAY]);
	params[10] = new StringParam("Tail", &presetData[REVERB_TAIL], {"On ", "Off"}, 3);

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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Intens", &presetData[TREMOLO_INTENSITY]);
	params[0]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &presetData[TREMOLO_RATE]);
	params[1]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS);

	params[2] = new StringParam("LFOType", &presetData[TREMOLO_LFO_TYPE], {"Sin     ", "Square  ", "Sawtooth"}, 8);
	params[2]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_TYPE_POS);

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO mod", &presetData[TREMOLO_LFO_MOD]);
	params[3]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_MOD_POS);

	params[4] = new StringParam("Type", &presetData[TREMOLO_MS], {"Mono  ", "Stereo"}, 6);
	params[4]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_MS_POS);

	params[5] = new StringParam("TAP", &presetData[TREMOLO_TAP],
			{"1    ", "1/1.5", "1/2  ", "1/3  ", "1/4  "}, 6);
	params[5]->setDspAddress(DSP_ADDRESS_TREMOLO, NOT_SEND_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_TREMOLO);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

