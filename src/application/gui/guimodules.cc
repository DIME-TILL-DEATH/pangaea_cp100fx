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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[RFILTER_MIX]);

	const char* paramNames0[3][4] = {"LPF","HPF","BPF"};
	params[1] = new StringParam("F Type", &prog_data[RFILTER_FTYPE], (char**)&paramNames0, 3);

	const char* paramNames1[3][4] = {"LFO", "Dyn", "Ext"};
	params[2] = new StringParam("F Mod", &prog_data[RFILTER_FMOD], (char**)&paramNames1, 3);
	StringParam* typeSelect = static_cast<StringParam*>(params[2]);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	typeSelect->setDisableMask(2, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO r", &prog_data[RFILTER_RATE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Lo freq", &prog_data[RFILTER_LPF]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hi freq", &prog_data[RFILTER_HPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Res", &prog_data[RFILTER_RESONANCE]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn th", &prog_data[RFILTER_DYN_THRESHOLD]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn att", &prog_data[RFILTER_DYN_ATTACK]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Dyn rel", &prog_data[RFLITER_DYN_RELEASE]);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &prog_data[RFILTER_VOLUME]);

	const char* paramNames2[7][7] = {"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"};
	params[11] = new StringParam("F Mod", &prog_data[RFILTER_LFO_TYPE], (char**)&paramNames2, 7);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_RESONANCE_FILTER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_RESONANCE_FILTER);
	if(menu) menu->setParams(params, paramNum, 3);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createGateMenu(AbstractMenu* parentMenu)
{
	const uint8_t gateParamNum = 3;
	BaseParam* gateParams[gateParamNum];

	gateParams[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &prog_data[GATE_THRESHOLD]);
	gateParams[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &prog_data[GATE_ATTACK]);
	gateParams[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Decay", &prog_data[GATE_DECAY]);

	for(int i=0; i<gateParamNum; i++) gateParams[i]->setDspAddress(DSP_ADDRESS_GATE, i);

	ParamListMenu* gateMenu = new ParamListMenu(parentMenu, MENU_GATE);
	if(gateMenu) gateMenu->setParams(gateParams, gateParamNum, 1);

	return gateMenu;
}

AbstractMenu* GuiModules::createCompressorMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &prog_data[COMPRESSOR_THRESHOLD]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Ratio", &prog_data[COMPRESSOR_RATIO]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &prog_data[COMPRESSOR_VOLUME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &prog_data[COMPRESSOR_ATTACK]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Release", &prog_data[COMPRESSOR_RELEASE]);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_COMPRESSOR, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_COMPRESSOR);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

AbstractMenu* GuiModules::createPreampMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 7;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Gain", &prog_data[PREAMP_GAIN]);
	params[0]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &prog_data[PREAMP_VOLUME]);
	params[1]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &prog_data[PREAMP_0]);
	params[2]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &prog_data[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_PREAMP, NOT_SEND_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Low", &prog_data[PREAMP_LOW]);
	params[4]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Mid", &prog_data[PREAMP_MID]);
	params[5]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_MID_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "High", &prog_data[PREAMP_HIGH]);
	params[6]->setDspAddress(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PREAMP);
	if(menu)
	{
		menu->setParams(params, paramNum, 2);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_PREAMP);
	}

	return menu;
}

AbstractMenu* GuiModules::createAmpMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Master", &prog_data[AMP_MASTER]);
	params[0]->setDspAddress(DSP_ADDRESS_AMP, AMP_MASTER_POS);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Presence", &prog_data[EQ_PRESENCE]);
	params[1]->setDspAddress(DSP_ADDRESS_EQ, EQ_PRESENCE_POS);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &prog_data[AMP_LEVEL]);
	params[2]->setDspAddress(DSP_ADDRESS_AMP, AMP_LEVEL_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, "", &prog_data[PREAMP_0]);
	params[3]->setDspAddress(DSP_ADDRESS_AMP, NOT_SEND_POS);

	const char* paramNames[14][11] = {"  PP 6L6  ","  PP EL34 ","  SE 6L6  ","  SE EL34 ",
											  " AMT TC-3 ","California","British M ","British L ","    Flat   ","Calif mod ","Calif vint",
			                                  "PVH PR0RS0","PVH PR5RS5","PVH PR8RS7"};/*,"PVH PR9RS8","PA Modern ","PP Amp 6L6","PP AmpEL34",
											  "SE Amp 6L6","PVH IC V1 ","PVH IC V2 ","PVH PR00  ","PVH PR98  "};*/
	params[4] = new StringParam("Type", &prog_data[AMP_TYPE], (char**)&paramNames, 11);
	params[4]->setDspAddress(DSP_ADDRESS_AMP_TYPE, AMP_TYPE_POS);
	params[4]->setDisplayPosition(36);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PA);
	if(menu)
	{
		menu->setParams(params, paramNum, 2);
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

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[PHASER_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &prog_data[PHASER_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Center", &prog_data[PHASER_CENTER]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &prog_data[PHASER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &prog_data[PHASER_FEEDBACK]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "Stage", &prog_data[PHASER_TYPE]);
	params[5]->setScaling(2, 4);
	params[5]->setBounds(0, 4);
	params[5]->setDisplayPosition(62);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Hpf", &prog_data[PHASER_HPF]);

	const char* paramNames[2][4] = {"Post", "Pre "};
	params[7] = new StringParam("Pos.", &prog_data[PHASER_PREPOST], (char**)&paramNames, 2);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_PHASER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PHASER);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

AbstractMenu* GuiModules::createFlangerMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[FLANGER_MIX]);
	const char* paramNames0[2][8] = {"Triangle", "Sinus   "};
	params[1] = new StringParam("LFO", &prog_data[FLANGER_LFO], (char**)&paramNames0, 2);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &prog_data[FLANGER_RATE]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &prog_data[FLANGER_WIDTH]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &prog_data[FLANGER_DELAY]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &prog_data[FLANGER_FEEDBACK]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &prog_data[FLANGER_HPF]);
	const char* paramNames1[2][4] = {"Post", "Pre "};
	params[7] = new StringParam("Pos.", &prog_data[FLANGER_LFO], (char**)&paramNames1, 2);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_FLANGER, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_FLANGER);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

AbstractMenu* GuiModules::createChorusMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[CHORUS_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &prog_data[CHORUS_RATE]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Width", &prog_data[CHORUS_WIDTH]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Delay", &prog_data[CHORUS_DELAY]);

	const char* paramNames0[6][12] = {"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"};
	params[4] = new StringParam("Type", &prog_data[CHORUS_TYPE], (char**)&paramNames0, 6);
	params[4]->setDisplayPosition(36);
	StringParam* typeSelect = static_cast<StringParam*>(params[4]);
	typeSelect->setDisableMask(4, {0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 1, 1, 0, 0, 0});

	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &prog_data[CHORUS_HPF]);


	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_CHORUS, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CHORUS);
	if(menu) menu->setParams(params, paramNum, 2);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createEarlyMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramCout = 2;
	BaseParam* params[paramCout];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[EARLY_MIX]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &prog_data[EARLY_SIZE]);

	for(int i=0; i<paramCout; i++) params[i]->setDspAddress(DSP_ADDRESS_EARLY_REFLECTIONS, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_EARLY_REFLECTIONS);
	if(menu) menu->setParams(params, paramCout, 1);

	return menu;
}

AbstractMenu* GuiModules::createDelayMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	ParamListMenu* menu;

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[DELAY_MIX]);
	params[0]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MIX_POS);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME, "Time", &GuiModules::createDelayTapMenu, &delay_time);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS);
	params[1]->setByteSize(2);
	params[1]->setBounds(10, 2730);
	params[1]->setScaling(10, 0);

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &prog_data[DELAY_FEEDBACK]);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &prog_data[DELAY_LPF]);
	params[3]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_LPF_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &prog_data[DELAY_HPF]);
	params[4]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_HPF_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D1 Pan", &prog_data[DELAY_PAN1]);
	params[5]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN1_POS);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D2 Vol", &prog_data[DELAY_VOLUME2]);
	params[6]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_VOLUME2_POS);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "D2 Pan", &prog_data[DELAY_PAN2]);
	params[7]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_PAN2_POS);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D->D2", &prog_data[DELAY_OFFSET]);
	params[8]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_OFFSET_POS);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "D_Mod", &prog_data[DELAY_MODULATION]);
	params[9]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_POS);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "M_Rate", &prog_data[DELAY_MODULATION_RATE]);
	params[10]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MODULATION_RATE_POS);
	const char* paramNames1[2][7] = {"Forward", "Reverse"};
	params[11] = new StringParam("Direct", &prog_data[DELAY_DIRECTION], (char**)&paramNames1, 2);
	params[11]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_DIRECTION_POS);

	menu = new ParamListMenu(parentMenu, MENU_DELAY);
	if(menu) menu->setParams(params, paramNum, 3);

	return menu;
}

//constexpr static uint8_t del_tim_l     [][5] ={"Time","TAP","Tail"};
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

	const char* paramNames2[6][6] = {"1/1  ", "1/1.5" ,"1/2  " ,"1/3  " ,"1/4  " ,"2/1  "};
	params[1] = new StringParam("TAP", &prog_data[TREMOLO_TAP], (char**)&paramNames2, 5);
	params[1]->setDspAddress(DSP_ADDRESS_DELAY, NOT_SEND_POS);

	const char* paramNames1[2][4] = {"On ", "Off"};
	params[2] = new StringParam("Tail", &prog_data[REVERB_TAIL], (char**)&paramNames1, 2);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TAIL_POS);

	menu = new ParamListMenu(parentMenu, MENU_TAP_DELAY);
	if(menu) menu->setParams(params, paramNum, 1);

	return menu;
}

AbstractMenu* GuiModules::createReverbMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 11;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[REVERB_MIX]);

	const char* paramNames0[7][8] = {"Default", "Hall   ", "Room   ", "Plate  ", "Spring ", "Gate   ", "Reverse"};
	params[1] = new StringParam("Type", &prog_data[REVERB_TYPE], (char**)&paramNames0, 7);
	StringParam* typeSelect = static_cast<StringParam*>(params[1]);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
	typeSelect->setDisableMask(4, {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	typeSelect->setDisableMask(6, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Time", &prog_data[REVERB_TIME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Size", &prog_data[REVERB_SIZE]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Damp", &prog_data[REVERB_DAMPING]);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &prog_data[REVERB_LPF]);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &prog_data[REVERB_HPF]);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Detune", &prog_data[REVERB_DETUNE]);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Diffus", &prog_data[REVERB_DIFFUSION]);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "PreD", &prog_data[REVERB_PREDELAY]);
	const char* paramNames1[2][4] = {"On ", "Off"};
	params[10] = new StringParam("Tail", &prog_data[REVERB_TAIL], (char**)&paramNames1, 2);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_REVERB, i);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_REVERB);
	if(menu) menu->setParams(params, paramNum, 3);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createTremoloMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Intens", &prog_data[TREMOLO_INTENSITY]);
	params[0]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS);

	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Rate", &prog_data[TREMOLO_RATE]);
	params[1]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS);

	const char* paramNames0[3][8] = {"Sin     ", "Square  ", "Sawtooth"};
	params[2] = new StringParam("LFOType", &prog_data[TREMOLO_LFO_TYPE], (char**)&paramNames0, 3);
	params[2]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_TYPE_POS);

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LFO mod", &prog_data[TREMOLO_LFO_MOD]);
	params[3]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_MOD_POS);

	const char* paramNames1[2][6] = {"Mono  ", "Stereo"};
	params[4] = new StringParam("Type", &prog_data[TREMOLO_MS], (char**)&paramNames1, 2);
	params[4]->setDspAddress(DSP_ADDRESS_TREMOLO, TREMOLO_MS_POS);

	const char* paramNames2[5][6] = {"1    ", "1/1.5", "1/2  ", "1/3  ", "1/4  "};
	params[5] = new StringParam("TAP", &prog_data[TREMOLO_TAP], (char**)&paramNames2, 5);
	params[5]->setDspAddress(DSP_ADDRESS_TREMOLO, NOT_SEND_POS);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_TREMOLO);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

