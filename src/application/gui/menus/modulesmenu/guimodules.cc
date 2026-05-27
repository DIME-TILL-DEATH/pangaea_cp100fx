#include "guimodules.h"

#include "system.h"
#include "preset.h"
#include "modules.h"

#include "sharc_task.h"

#include "submenuparam.h"
#include "stringoutparam.h"
#include "customparam.h"

#include "abstractmenu.h"
#include "eqmenu.h"
#include "eqgraphmenu.h"
#include "cabbrowsermenu.h"
#include "paramlistmenu.h"

AbstractMenu* GuiModules::createRfMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &RfDesc.mix);
	params[1] = new StringListParam(&RfDesc.filtType, {"LPF","HPF","BPF"}, 3);

	StringListParam* typeSelect = new StringListParam(&RfDesc.fmod, {"LFO", "Dyn", "Ext"}, 3);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	typeSelect->setDisableMask(2, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1});
	params[2] = typeSelect;

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.rate);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.hpf);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.lpf);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.resonance);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.dynThreshold);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.dynAttack);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.dynRelease);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &RfDesc.volume);
	params[11] = new StringListParam(&RfDesc.lfo, {"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"}, 7);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_RESONANCE_FILTER);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setTapDestination(System::TapDestination::TAP_RFILTER);
	}

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createGateMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramCount = 3;
	BaseParam* params[paramCount];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &GateDesc.threshold);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &GateDesc.attack);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &GateDesc.decay);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_GATE);
	if(menu) menu->setParams(params, paramCount);

	return menu;
}

AbstractMenu* GuiModules::createCompressorMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &CompressorDesc.threshold);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &CompressorDesc.ratio);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &CompressorDesc.volume);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &CompressorDesc.attack);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &CompressorDesc.knee);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_COMPRESSOR);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createPreampMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 7;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PreampDesc.gain);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PreampDesc.volume);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &PreampDesc.dummy);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &PreampDesc.dummy);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PreampDesc.low);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PreampDesc.mid);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PreampDesc.high);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PREAMP);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_OUT, DSP_INDICATOR_PREAMP);
	}

	return menu;
}

AbstractMenu* GuiModules::createAmpMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PaDesc.master);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PaDesc.presence);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PaDesc.level);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &PreampDesc.dummy);
#ifdef __MONO_MOD__
	params[4] = new StringListParam(&PaDesc.type,
		 {"  PP 6L6  ", "  PP EL34 ", "  SE 6L6  ", "  SE EL34 ", " AMT TC-3 ",
		  "California",	"British M ", "British L ", "    Flat   ","Calif mod ",
		  "Calif vint", "PVH PR0RS0", "PVH PR5RS5", "PVH PR8RS7"}, 11);
#endif
#ifdef __STEREO_MOD__
	params[4] = new StringListParam(&PaDesc.type,
		 {"  PP 6L6  ", "  PP EL34 ", "  SE 6L6  ", "  SE EL34 ", " AMT TC-3 ",
		  "California",	"British M ", "British L ", "    Flat   ","Calif mod ",
		  "Calif vint", "PVH PR0RS0", "PVH PR5RS5", "PVH PR8RS7", "PVH PR9RS8",/*
		 "PA Modern ", "PP Amp 6L6", "PP AmpEL34", "SE Amp 6L6", "PVH IC V1 ",
		 "PVH IC V2 ", "PVH PR00  ", "PVH PR98  "*/}, 11);
#endif
	params[4]->setDisplayPosition(36);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PA);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_OUT, DSP_INDICATOR_AMP);
	}

	return menu;
}

AbstractMenu* GuiModules::createIrMenu(AbstractMenu* parentMenu)
{
#ifdef __MONO_MOD__
	if(System::cab_type == CAB_CONFIG_STEREO)
	{
		ParamListMenu* paramsMenu = new ParamListMenu(parentMenu, MENU_CABTYPE);

		const uint8_t paramNum = 2;
		BaseParam* params[paramNum];


		params[0] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet 1", &GuiModules::createCab1Menu, paramsMenu);
		params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet 2", &GuiModules::createCab2Menu, paramsMenu);

		paramsMenu->setParams(params, paramNum);
		paramsMenu->setIcon(false, ICON_NONE);

		return paramsMenu;
	}
	else
	{
		return GuiModules::createCab1Menu(parentMenu);
	}
#endif

#ifdef __STEREO_MOD__
	ParamListMenu* paramsMenu = new ParamListMenu(parentMenu, MENU_CABTYPE);

	const uint8_t paramNum = 2;
	BaseParam* params[paramNum];

	params[0] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet right", &GuiModules::createCab1Menu, paramsMenu);
	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Cabinet left", &GuiModules::createCab2Menu, paramsMenu);

	paramsMenu->setParams(params, paramNum);
	paramsMenu->setIcon(false, ICON_NONE);

	return paramsMenu;
#endif
}

AbstractMenu* GuiModules::createCab1Menu(AbstractMenu* parentMenu)
{
	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CABSIM);

	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	params[0] = new StringOutParam((const char*)currentPreset.cab1Name);
	params[0]->setDisplayPosition(ParamListMenu::leftPad);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Browser", &GuiModules::createCab1BrowserMenu, menu);

#ifdef __MONO_MOD__
	if(System::cab_type == CAB_CONFIG_STEREO) params[2] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, &IrDesc.pan1);
	else params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &IrDesc.dummy);
#endif

#ifdef __STEREO_MOD__
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &IrDesc.dummy);
#endif

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_VOLUME, &IrDesc.vol1);
	params[3]->setDisplayPosition(42);

	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB1, &currentPreset.modulesBuf[IR_VOLUME1]);
		menu->setIcon(false, ICON_NONE);

		StringOutParam* runningString = static_cast<StringOutParam*>(params[0]);
		runningString->setRunning(true, menu);
	}

	return menu;
}

AbstractMenu* GuiModules::createCab2Menu(AbstractMenu* parentMenu)
{
	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CABSIM);

	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	params[0] = new StringOutParam((const char*)currentPreset.cab2Name);
	params[0]->setDisplayPosition(ParamListMenu::leftPad);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "Browser", &GuiModules::createCab2BrowserMenu, menu);

#ifdef __MONO_MOD__
	if(System::cab_type == CAB_CONFIG_STEREO) params[2] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, &IrDesc.pan2);
	else params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &IrDesc.dummy);
#endif

#ifdef __STEREO_MOD__
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_DUMMY, &IrDesc.dummy);
#endif

	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_VOLUME, &IrDesc.vol2);
	params[3]->setDisplayPosition(42);


	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB2, &currentPreset.modulesBuf[IR_VOLUME2]);
		menu->setIcon(false, ICON_NONE);

		StringOutParam* runningString = static_cast<StringOutParam*>(params[0]);
		runningString->setRunning(true, menu);
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
	if(sys_para[System::EQ_SCREEN_MODE] == System::EQ_SCREEN_BARS)return new EqMenu(parentMenu);
	else return new EqGraphMenu(parentMenu);
}

AbstractMenu* GuiModules::createPhaserMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &PhaserDesc.mix);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PhaserDesc.rate);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PhaserDesc.center);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PhaserDesc.width);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PhaserDesc.feedback);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_NUM, &PhaserDesc.stages);
	params[5]->setScaling(2, 4);
	params[5]->setBounds(0, 4);
	params[5]->setDisplayPosition(62);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &PhaserDesc.hpf);
	params[7] = new StringListParam(&PhaserDesc.position, {"Post", "Pre "}, 4);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_PHASER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createFlangerMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 8;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &FlangerDesc.mix);
	params[1] = new StringListParam(&FlangerDesc.genType, {"Triangle", "Sinus   "}, 8);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &FlangerDesc.rate);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &FlangerDesc.width);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &FlangerDesc.delay);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &FlangerDesc.feedback);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &FlangerDesc.hpf);
	params[7] = new StringListParam(&FlangerDesc.position, {"Post", "Pre "}, 4);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_FLANGER);
	if(menu) menu->setParams(params, paramNum);

	return menu;
}

const char* rateChorusPrint(void* parameter)
{
	if(currentPreset.modulesBuf[CHORUS_TYPE] == 4) return "Detune";
	else return "Rate  ";
}

AbstractMenu* GuiModules::createChorusMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &ChorusDesc.mix);
	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Level, &ChorusDesc.rate);
	customParam->nameCallback = rateChorusPrint;
	params[1] = customParam;

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ChorusDesc.width);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ChorusDesc.delay);
	StringListParam* typeSelect = new StringListParam(&ChorusDesc.type,
			{"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"}, 12);
	typeSelect->setDisplayPosition(36);
	typeSelect->setDisableMask(4, {0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 1, 1, 0, 0, 0});
	params[4] = typeSelect;
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ChorusDesc.hpf);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_CHORUS);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createEarlyMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramCout = 2;
	BaseParam* params[paramCout];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &EarlyDesc.mix);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &EarlyDesc.size);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_EARLY_REFLECTIONS);
	if(menu) menu->setParams(params, paramCout);

	return menu;
}

AbstractMenu* GuiModules::createDelayMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_DELAY);;

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &DelayDesc.mix);

	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME, &DelayDesc.time,
			&GuiModules::createDelayTapMenu, menu);
	params[1]->setByteSize(2);
	params[1]->setBounds(10, 2730);
	params[1]->setScaling(10, 0);

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.feedback);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.lpf);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.hpf);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, &DelayDesc.pan1);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.volume2);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_PAN,  &DelayDesc.pan2);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.offset);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.modulation);
	params[10] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &DelayDesc.rate);
	params[11] = new StringListParam(&DelayDesc.direction, {"Forward", "Reverse"}, 7);

	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setTapDestination(System::TapDestination::TAP_DELAY);
	}

	return menu;
}

void delayTimeDecrease(void* parameter)
{
	if(sys_para[System::TIME_FORMAT] == System::TIME_FORMAT_SEC)
	{
		if(currentPreset.delayTime > 10) currentPreset.delayTime = BaseParam::encSpeedDec(currentPreset.delayTime, 10);
	}
	else
	{
		uint16_t tempo = 60000 / currentPreset.delayTime;
		if(tempo > System::minBpm) tempo = BaseParam::encSpeedDec(tempo, System::minBpm);
		else tempo = System::minBpm;
		currentPreset.delayTime = 60000 / tempo;
	}
}

void delayTimeIncrease(void* parameter)
{
	if(sys_para[System::TIME_FORMAT] == System::TIME_FORMAT_SEC)
	{
		if(currentPreset.delayTime < 2730) currentPreset.delayTime = BaseParam::encSpeedInc(currentPreset.delayTime, 2730);
	}
	else
	{
		uint16_t tempo = 60000 / currentPreset.delayTime;
		if(tempo < System::maxBpm) tempo = BaseParam::encSpeedInc(tempo, System::maxBpm);
		else tempo = System::maxBpm;
		currentPreset.delayTime = 60000 / tempo;
	}
}

void delayTimePrint(void* parameter)
{
	if(sys_para[System::TIME_FORMAT] == System::TIME_FORMAT_SEC)
	{
		DisplayTask->DelayTimeInd(58, 0, currentPreset.delayTime);
	}
	else
	{
		DisplayTask->ParamIndNum(58, 0, 60000/currentPreset.delayTime);
		DisplayTask->StringOut(58 + 24, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"BPM");
	}
}

AbstractMenu* GuiModules::createDelayTapMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 3;
	BaseParam* params[paramNum];

	ParamListMenu* menu;

	CustomParam* customParam = new CustomParam(CustomParam::TDisplayType::Custom, &DelayDesc.time);
	customParam->decreaseCallback = delayTimeDecrease;
	customParam->increaseCallback = delayTimeIncrease;
	customParam->printCallback = delayTimePrint;
	customParam->setByteSize(2);
	params[0] = customParam;

	params[1] = new StringListParam(&DelayDesc.tap, {"1/4 ", "1/8." ,"1/8 " ,"1/8t" ,"1/16" ,"1/2 "}, 5);
	params[2] = new StringListParam(&DelayDesc.tail, {"On ", "Off"}, 3);

	menu = new ParamListMenu(parentMenu, MENU_TAP_DELAY);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setIcon(true, ICON_DL);
		menu->setTapDestination(System::TapDestination::TAP_DELAY);
	}

	return menu;
}

AbstractMenu* GuiModules::createReverbMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 11;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, &ReverbDesc.mix);

	StringListParam* typeSelect = new StringListParam(&ReverbDesc.type,
			{"Default", "Hall   ", "Room   ", "Plate  ", "Spring ", "Gate   ", "Reverse"}, 8);
	typeSelect->setDisableMask(0, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
	typeSelect->setDisableMask(4, {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0});
	typeSelect->setDisableMask(5, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	typeSelect->setDisableMask(6, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0});
	params[1] = typeSelect;

	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.time);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.size);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.damping);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.lpf);
	params[6] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.hpf);
	params[7] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.detune);
	params[8] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.diffusion);
	params[9] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &ReverbDesc.predelay);
	params[10] = new StringListParam(&ReverbDesc.tail, {"On ", "Off"}, 3);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_REVERB);
	if(menu) menu->setParams(params, paramNum);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

AbstractMenu* GuiModules::createTremoloMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 6;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &TremoloDesc.depth);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &TremoloDesc.rate);
	params[2] = new StringListParam(&TremoloDesc.lfo_type, {"Sin     ", "Square  ", "Sawtooth"}, 8);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &TremoloDesc.lfo_mod);
	params[4] = new StringListParam(&TremoloDesc.ms, {"Mono  ", "Stereo"}, 6);
	params[5] = new StringListParam(&TremoloDesc.tap, {"1/4 ", "1/8." ,"1/8 " ,"1/8t" ,"1/16"}, 5);

	ParamListMenu* menu = new ParamListMenu(parentMenu, MENU_TREMOLO);
	if(menu)
	{
		menu->setParams(params, paramNum);
		menu->setTapDestination(System::TapDestination::TAP_TREMOLO);
	}
	return menu;
}

