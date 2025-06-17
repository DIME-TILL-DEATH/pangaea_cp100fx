#include "modulesmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "filt.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"
#include "spectrum.h"
#include "vdt/vdt.h"

#include "modules.h"

#include "abstractmenu.h"

extern volatile uint8_t comp_fl;
extern volatile uint8_t preset_edited;
extern gui_menu_type current_menu;
extern uint8_t copy_temp;


extern uint8_t cab_type;
extern uint8_t name_run_fl;

extern uint8_t key_shift;
extern uint8_t control_destin;
extern volatile uint8_t sub_men;
extern volatile uint8_t nam_sym_temp;

const uint8_t ModulesMenu::cab_list[][8];
const uint8_t ModulesMenu::cab_out_list[][6];
const uint8_t ModulesMenu::cab_list_menu[][9];

const uint8_t ModulesMenu::cc[];
const uint8_t ModulesMenu::contr_list[][7];
const uint8_t ModulesMenu::volum[];
const uint8_t ModulesMenu::s_to_ch[];

const uint8_t ModulesMenu::sd_nr [];
const uint8_t ModulesMenu::imp_dir_n [];
const uint8_t ModulesMenu::imp_dir_no [ ];
const uint8_t ModulesMenu::sd_lo [];

const uint8_t ModulesMenu::s_t_c_list    [][7];

constexpr static uint8_t del_list      [][8] ={"Mix","Time","F_Back","LPF","HPF","D_Pan","D2 Vol","D2 Pan","D->D2",
		"D_Mod","M_Rate","Direct"};

ModulesMenu::ModulesMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MODULES;
}

void ModulesMenu::show(TShowMode showMode)
{
	current_menu = m_menuType;
	currentMenu = this;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	DisplayTask->Menu_init();
	tim5_start(0);
}

void ModulesMenu::task()
{
	if(current_menu != MENU_MODULES) return;

	switch (num_men)
	{
	case 0:
	  if(tim5_fl == 0)DisplayTask->EfIcon(2,0,(uint8_t*)rf,prog_data[ENABLE_RESONANCE_FILTER]);
	  else DisplayTask->EfIcon(2,0,(uint8_t*)rf,2);
	  break;
	case 1:
	  if(tim5_fl == 0)DisplayTask->EfIcon(20,0,(uint8_t*)gt,prog_data[ENABLE_GATE]);
	  else DisplayTask->EfIcon(20,0,(uint8_t*)gt,2);
	  break;
	case 2:
	  if(tim5_fl == 0)DisplayTask->EfIcon(38,0,(uint8_t*)cm,prog_data[ENABLE_COMPRESSOR]);
	  else DisplayTask->EfIcon(38,0,(uint8_t*)cm,2);
	  break;
	case 3:
	  if(tim5_fl == 0)DisplayTask->EfIcon(56,0,(uint8_t*)pr,prog_data[ENABLE_PREAMP]);
	  else DisplayTask->EfIcon(56,0,(uint8_t*)pr,2);
	  break;
	case 4:
	  if(tim5_fl == 0)DisplayTask->EfIcon(74,0,(uint8_t*)am,prog_data[ENABLE_AMP]);
	  else DisplayTask->EfIcon(74,0,(uint8_t*)am,2);
	  break;
	case 5:
	  if(tim5_fl == 0)DisplayTask->EfIcon(92,0,(uint8_t*)cs,prog_data[ENABLE_CAB]);
	  else DisplayTask->EfIcon(92,0,(uint8_t*)cs,2);
	  break;
	case 6:
	  if(tim5_fl == 0)DisplayTask->EfIcon(110,0,(uint8_t*)equ,prog_data[ENABLE_EQ]);
	  else DisplayTask->EfIcon(110,0,(uint8_t*)equ,2);
	  break;
	case 7:
	  if(tim5_fl == 0)DisplayTask->EfIcon(2,2,(uint8_t*)ph,prog_data[ENABLE_PHASER]);
	  else DisplayTask->EfIcon(2,2,(uint8_t*)ph,2);
	  break;
	case 8:
	  if(tim5_fl == 0)DisplayTask->EfIcon(20,2,(uint8_t*)rm,prog_data[ENABLE_FLANGER]);
	  else DisplayTask->EfIcon(20,2,(uint8_t*)rm,2);
	  break;
	case 9:
	  if(tim5_fl == 0)DisplayTask->EfIcon(38,2,(uint8_t*)ch,prog_data[ENABLE_CHORUS]);
	  else DisplayTask->EfIcon(38,2,(uint8_t*)ch,2);
	  break;
	case 10:
	  if(tim5_fl == 0)DisplayTask->EfIcon(56,2,(uint8_t*)dl,prog_data[ENABLE_DELAY]);
	  else DisplayTask->EfIcon(56,2,(uint8_t*)dl,2);
	  break;
	case 11:
	  if(tim5_fl == 0)DisplayTask->EfIcon(74,2,(uint8_t*)er,prog_data[ENABLE_EARLY_REFLECTIONS]);
	  else DisplayTask->EfIcon(74,2,(uint8_t*)er,2);
	  break;
	case 12:
	  if(tim5_fl == 0)DisplayTask->EfIcon(92,2,(uint8_t*)rv,prog_data[ENABLE_REVERB]);
	  else DisplayTask->EfIcon(92,2,(uint8_t*)rv,2);
	  break;
	case 13:
	  if(tim5_fl == 0)DisplayTask->EfIcon(110,2,(uint8_t*)tr,prog_data[ENABLE_TREMOLO]);
	  else DisplayTask->EfIcon(110,2,(uint8_t*)tr,2);
	  break;
	}
}

void ModulesMenu::encoderPressed()
{
	if(current_menu != MENU_MODULES) return;


	switch (num_men)
	{
		case 0:
			prog_data[ENABLE_RESONANCE_FILTER] = !(bool)prog_data[ENABLE_RESONANCE_FILTER];
			gui_send(18, 13 | (prog_data[ENABLE_RESONANCE_FILTER] << 8));
			break;
		case 1:
			prog_data[ENABLE_GATE] = !(bool)prog_data[ENABLE_GATE];
			gui_send(18, ENABLE_GATE | (prog_data[ENABLE_GATE] << 8));
			break;
		case 2:
			prog_data[ENABLE_COMPRESSOR] = !(bool)prog_data[ENABLE_COMPRESSOR];
		    gui_send(18, ENABLE_COMPRESSOR | (prog_data[ENABLE_COMPRESSOR] << 8));
		  break;
		case 3:
		  if(prog_data[pream] == 0)prog_data[pream] = 1;
		  else prog_data[pream] = 0;
		  gui_send(18, ENABLE_PREAMP | (prog_data[pream] << 8));
		  break;
		case 4:
		  if(prog_data[amp] == 0)prog_data[amp] = 1;
		  else prog_data[amp] = 0;
		  gui_send(18, ENABLE_AMP | (prog_data[amp] << 8));
		  break;
		case 5:
		  if(prog_data[cab] == 0)
		  {
			  prog_data[cab] = 1;
			  if(name_buf[0] == 0)
			  {
				  kgp_sdk_libc::memset(preset_temp,0,24576);
				  kgp_sdk_libc::memset(name_buf_temp,0,64);
				  cab_num = 0;
				  extern uint8_t sd_init_fl;
				  if(sd_init_fl == 1)
				  {
					  current_menu = MENU_CABBROWSER;
					  DisplayTask->Clear();
					  extern volatile uint8_t imp_dir_fl;
					  if(imp_dir_fl)
					  {
						  vol_vol = prog_data[vol];
						  gui_send(7,0);
						  DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_CAB1);
						  FSTask->SendCommand( TFsBrowser::bcCurrent );
						  FSTask->SendCommand( TFsBrowser::bcLoadImp );
					  }
					  else
					  {
						  DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_n);
						  DisplayTask->StringOut(42,3,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_no);
						  CSTask->CS_del(1000);
						  prog_data[cab] = 0;
						  current_menu = MENU_MAIN;
						  encoder_knob_selected = 0;
						  DisplayTask->Menu_init();
					  }
				  }
				  else
				  {
					  DisplayTask->Clear();
					  if(!sd_init_fl)DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_nr);
					  else DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_lo);
					  CSTask->CS_del(1000);
					  prog_data[cab] = 0;
					  current_menu = MENU_MAIN;
					  encoder_knob_selected = 0;
					  DisplayTask->Menu_init();
				  }
			  }
		  }
		  else prog_data[cab] = 0;
		  gui_send(18, ENABLE_CAB | (prog_data[cab] << 8));
		  break;
		case 6:
		  if(prog_data[eq] == 0)prog_data[eq] = 1;
		  else prog_data[eq] = 0;
		  gui_send(18, ENABLE_EQ | (prog_data[eq] << 8));
		  break;
		case 7:
		  if(prog_data[phas] == 0)prog_data[phas] = 1;
		  else prog_data[phas] = 0;
		  gui_send(18, ENABLE_PHASER | (prog_data[phas] << 8));
		  break;
		case 8:
		  if(prog_data[fl] == 0)prog_data[fl] = 1;
		  else prog_data[fl] = 0;
		  gui_send(18, ENABLE_FLANGER | (prog_data[fl] << 8));
		  break;
		case 9:
		  if(prog_data[chor] == 0)prog_data[chor] = 1;
		  else prog_data[chor] = 0;
		  gui_send(18, ENABLE_CHORUS | (prog_data[chor] << 8));
		  break;
		case 10:
		  if(prog_data[delay] == 0)prog_data[delay] = 1;
		  else prog_data[delay] = 0;
		  gui_send(18, ENABLE_DELAY | (prog_data[delay] << 8));
		  break;
		case 11:
		  if(prog_data[early] == 0)prog_data[early] = 1;
		  else prog_data[early] = 0;
		  gui_send(18, ENABLE_EARLY_REFLECTIONS | (prog_data[early] << 8));
		  break;
		case 12:
		  if(prog_data[reve] == 0)prog_data[reve] = 1;
		  else prog_data[reve] = 0;
		  gui_send(18, ENABLE_REVERB | (prog_data[reve] << 8));
		  break;
		case 13:
		  if(prog_data[ENABLE_TREMOLO] == 0) prog_data[ENABLE_TREMOLO] = 1;
		  else prog_data[ENABLE_TREMOLO] = 0;
		  gui_send(18, ENABLE_TREMOLO | (prog_data[trem] << 8));
		  break;
	}
	clean_flag();
	tim5_start(1);
}

void ModulesMenu::encoderClockwise()
{
	if(current_menu != MENU_MODULES) return;

	if(!comp_fl)
	{
		icon_refresh(num_men);

		if(num_men < 13) num_men++;
		else num_men = 0;

		tim5_start(0);
	}
	clean_flag();
}

void ModulesMenu::encoderCounterClockwise()
{
	if(current_menu != MENU_MODULES) return;

	if(!comp_fl)
	{
		icon_refresh(num_men);

		if(num_men > 0) num_men--;
		else num_men = 13;

		tim5_start(0);
	}
	clean_flag();
}

void ModulesMenu::keyUp()
{
	if(current_menu != MENU_MODULES) return;

	//-------------------------------------------Key UP----------------------------------------------------------------
	if(!comp_fl)
	{
		currentMenu = topLevelMenu;
		//parent->show(); //????

		if(preset_edited == 1)
		{
			current_menu = MENU_PRESET_SAVE;
			DisplayTask->Clear();
			DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)s_to_ch);
			for(uint8_t i = 0 ; i < 3 ; i++)DisplayTask->StringOut(i*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + i*7);
			par_num = 2;
			tim5_start(1);
		}
		else
		{
			vol_fl = 0;
//			edit_modules_fl = 0;
			prog_cur = 0;
			num_men = 0;
			DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
			DisplayTask->Menu_init();
			tim5_start(0);
		}

		clean_flag();
	}
}

void ModulesMenu::keyDown()
{
	if(current_menu != MENU_MODULES) return;

	//-------------------------------KEYS-----------------------------------------------
	if((comp_fl == 0) && (prog1 == prog))
	{
		switch (num_men){
		case 0:
			if(prog_data[ENABLE_RESONANCE_FILTER])
			{
				shownChildMenu = createRfMenu();
				shownChildMenu->show();
			}
		  break;
		case 1:
			if(prog_data[ENABLE_GATE])
			{
				shownChildMenu = createGateMenu();
				shownChildMenu->show();
			}
		break;
		case 2:
			if(prog_data[ENABLE_COMPRESSOR])
			{
				shownChildMenu = createCompressorMenu();
				shownChildMenu->show();
			}
		  break;
		case 3:
			if(prog_data[ENABLE_PREAMP])
			{
				DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_PREAMP);
				shownChildMenu = createPreampMenu();
				shownChildMenu->show();
			}
		 break;
		case 4:
		  if(prog_data[ENABLE_AMP])
		  {
			  DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_AMP);
			  shownChildMenu = createAmpMenu();
			  shownChildMenu->show();
		  }
		  break;
		case 5:
			if(prog_data[cab])
			{
			 DisplayTask->Clear();
			 if(cab_type != 2)
			 {
			   name_run_fl = 1;
			   DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
			   DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
			   DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
			   vol_vol = prog_data[vol];
			   gui_send(7,0);
			   current_menu = MENU_CABSIM;
			   DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_CAB1);
			   par_num = 1;
			 }
			 else {
			   DisplayTask->StringOut(6,0,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu);
			   DisplayTask->StringOut(6,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu + 9);
			   current_menu = MENU_CABTYPE;
			   par_num = 0;
			 }
		   }
		  break;
		case 6:
		  if(prog_data[eq] == 1)
		  {
		   current_menu = MENU_EQ;
		   eq_num = 0;
		   DisplayTask->EqInit();
//		   DisplayTask->Icon_Strel(4,2);
		  }
		  break;
		case 7:
		  if(prog_data[ENABLE_PHASER])
		  {
			  shownChildMenu = createPhaserMenu();
			  shownChildMenu->show();
		  }
		  break;
		case 8:
			if(prog_data[ENABLE_FLANGER])
			{
				shownChildMenu = createFlangerMenu();
				shownChildMenu->show();
			}
		  break;
		case 9:
			if(prog_data[ENABLE_CHORUS])
			{
				shownChildMenu = createChorusMenu();
				shownChildMenu->show();
			}
		  break;
		case 10:
			if(prog_data[ENABLE_DELAY])
			{
				shownChildMenu = createDelayMenu(this);
				shownChildMenu->show();
			}
		  break;
		case 11:
			if(prog_data[ENABLE_EARLY_REFLECTIONS])
			{
				shownChildMenu = createEarlyMenu(this);
				shownChildMenu->show();
			}
		  break;
		case 12:
			if(prog_data[ENABLE_REVERB])
			{
				shownChildMenu = createReverbMenu();
				shownChildMenu->show();
			}
		  break;
		case 13:
			if(prog_data[ENABLE_TREMOLO])
			{
				shownChildMenu = createTremoloMenu();
				shownChildMenu->show();
			}
		  break;
		}
		tim5_start(0);
		clean_flag();
	}
}

void ModulesMenu::key1()
{
	if(current_menu != MENU_MODULES) return;

	clean_flag();
	DisplayTask->Clear();

	current_menu = MENU_ERASE;
	par_num = 0;

	DisplayTask->StringOut(36,2,TDisplayTask::fntSystem,0,(uint8_t*)"Preset");
	DisplayTask->StringOut(12,3,TDisplayTask::fntSystem,0,(uint8_t*)"Default Settings?");
	DisplayTask->StringOut(30,0,TDisplayTask::fntSystem,0,(uint8_t*)"No      Yes");

	tim5_start(0);
}

void ModulesMenu::key2()
{
	if(current_menu != MENU_MODULES) return;

	clean_flag();
	DisplayTask->Clear();
	par_num = 0;

	sub_men = 1;
	current_menu = MENU_VOLUME;
	DisplayTask->StringOut(28,0,TDisplayTask::fntSystem , 0 , (uint8_t*)volum);
	DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
	if(prog_data[vol_contr])DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Off");
	else DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"On ");
	DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
	DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
	vol_vol = prog_data[pres_lev];
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	tim5_start(0);
	num_men_temp = num_men;

}

void ModulesMenu::key3()
{
	if(current_menu != MENU_MODULES) return;

	clean_flag();

	sub_men = 2;
	current_menu = MENU_CONTROLLERS;
	control_destin = dest_tabl_start[control[1]];
	contrs = 0;
	DisplayTask->Clear();
	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		DisplayTask->StringOut(0,i,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_list + i*7);
		switch (i)
		{
		case 0:DisplayTask->ParamIndicNum(45,0,1);break;
		case 1:if(control[0] == 0)DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
			   else {
				 if(control[0] > 4)
				 {
					 DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
					 DisplayTask->ParamIndicNum(69,1,control[0] - 5);
				 }
				 else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[0]-1)*12);
			   }break;
		case 2:DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + control[1]*14);break;
		}
	}
	DisplayTask->Strel(58,3,0);
	par_num = 0;
	num_men_temp = num_men;
}

void ModulesMenu::key4()
{
	if(current_menu != MENU_MODULES) return;

	clean_flag();

	sub_men = 3;
	DisplayTask->Clear();
	current_menu = MENU_NAME_EDIT;
	key_shift = 0;
	eq_num = 0;
	par_num = 0;
	nam_sym_temp = 32;
	DisplayTask->StringOut(2,0,TDisplayTask::fntSystem,0,(uint8_t*)imya);
	DisplayTask->StringOut(2,1,TDisplayTask::fntSystem,0,(uint8_t*)imya1);
	DisplayTask->SymbolOut(86,0,TDisplayTask::fntSystem,0,46);
	DisplayTask->SymbolOut(86,1,TDisplayTask::fntSystem,0,46);
	DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low1);
	DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low2);
	num_men_temp = num_men;
}

void ModulesMenu::key5()
{
	if(current_menu != MENU_MODULES) return;

	clean_flag();

	current_menu = MENU_COPY;
	read_prog_temp(copy_temp);
	imya_temp = 1;
	for(uint8_t i = 0 ; i < 15 ; i++)imya_t[i] = preset_temp[i];
	for(uint8_t i = 0 ; i < 15 ; i++)imya1_t[i] = preset_temp[15 + i];

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(copy_temp);
	DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");
}


void ModulesMenu::icon_refresh(uint8_t num)
{
	switch (num)
	{
	case 0:DisplayTask->EfIcon(2,0,(uint8_t*)rf,prog_data[moog]);break;
	case 1:DisplayTask->EfIcon(20,0,(uint8_t*)gt,prog_data[gate]);break;
	case 2:DisplayTask->EfIcon(38,0,(uint8_t*)cm,prog_data[compr]);break;
	case 3:DisplayTask->EfIcon(56,0,(uint8_t*)pr,prog_data[pream]);break;
	case 4:DisplayTask->EfIcon(74,0,(uint8_t*)am,prog_data[amp]);break;
	case 5:DisplayTask->EfIcon(92,0,(uint8_t*)cs,prog_data[cab]);break;
	case 6:DisplayTask->EfIcon(110,0,(uint8_t*)equ,prog_data[eq]);break;
	case 7:DisplayTask->EfIcon(2,2,(uint8_t*)ph,prog_data[phas]);break;
	case 8:DisplayTask->EfIcon(20,2,(uint8_t*)rm,prog_data[fl]);break;
	case 9:DisplayTask->EfIcon(38,2,(uint8_t*)ch,prog_data[chor]);break;
	case 10:DisplayTask->EfIcon(56,2,(uint8_t*)dl,prog_data[delay]);break;
	case 11:DisplayTask->EfIcon(74,2,(uint8_t*)er,prog_data[early]);break;
	case 12:DisplayTask->EfIcon(92,2,(uint8_t*)rv,prog_data[reve]);break;
	case 13:DisplayTask->EfIcon(110,2,(uint8_t*)tr,prog_data[trem]);break;
	}
}

ParamListMenu* ModulesMenu::createRfMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_RESONANCE_FILTER);
	if(menu) menu->setParams(params, paramNum, 3);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

ParamListMenu* ModulesMenu::createGateMenu()
{
	const uint8_t gateParamNum = 3;
	BaseParam* gateParams[gateParamNum];

	gateParams[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &prog_data[GATE_THRESHOLD]);
	gateParams[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &prog_data[GATE_ATTACK]);
	gateParams[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Decay", &prog_data[GATE_DECAY]);

	for(int i=0; i<gateParamNum; i++) gateParams[i]->setDspAddress(DSP_ADDRESS_GATE, i);

	ParamListMenu* gateMenu = new ParamListMenu(this, MENU_GATE);
	if(gateMenu) gateMenu->setParams(gateParams, gateParamNum, 1);

	return gateMenu;
}

ParamListMenu* ModulesMenu::createCompressorMenu()
{
	const uint8_t paramNum = 5;
	BaseParam* params[paramNum];

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &prog_data[COMPRESSOR_THRESHOLD]);
	params[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Ratio", &prog_data[COMPRESSOR_RATIO]);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Volume", &prog_data[COMPRESSOR_VOLUME]);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &prog_data[COMPRESSOR_ATTACK]);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Release", &prog_data[COMPRESSOR_RELEASE]);

	for(int i=0; i<paramNum; i++) params[i]->setDspAddress(DSP_ADDRESS_COMPRESSOR, i);

	ParamListMenu* menu = new ParamListMenu(this, MENU_COMPRESSOR);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

ParamListMenu* ModulesMenu::createPreampMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_PREAMP);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

ParamListMenu* ModulesMenu::createAmpMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_PA);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

// При режиме детюн параметр Rate должен называться Detune
ParamListMenu* ModulesMenu::createPhaserMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_PHASER);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

ParamListMenu* ModulesMenu::createFlangerMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_FLANGER);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}

ParamListMenu* ModulesMenu::createChorusMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_CHORUS);
	if(menu) menu->setParams(params, paramNum, 2);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}


AbstractMenu* ModulesMenu::createEarlyMenu(AbstractMenu* parentMenu)
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

//constexpr static uint8_t del_tim_l     [][5] ={"Time","TAP","Tail"};
//constexpr static uint8_t tap_tim       [][6] ={"1/1  ","1/1.5","1/2  ","1/3  ","1/4  ","2/1  "};
//constexpr static float   tap_tim_v     [6]   ={1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};

ParamListMenu* ModulesMenu::createDelayMenu(AbstractMenu* parentMenu)
{
	const uint8_t paramNum = 12;
	BaseParam* params[paramNum];

	ParamListMenu* menu;

	params[0] = new BaseParam(BaseParam::GUI_PARAMETER_MIX, "Mix", &prog_data[DELAY_MIX]);
	params[0]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_MIX_POS);
	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_DELAY_TIME, "Time", &ModulesMenu::createEarlyMenu, &delay_time);
//	params[1]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS);
//	params[1]->setByteSize(2);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "F_Back", &prog_data[DELAY_FEEDBACK]);
	params[2]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "LPF", &prog_data[DELAY_LPF]);
	params[3]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_LPF_POS);
	params[4] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "HPF", &prog_data[DELAY_HPF]);
	params[4]->setDspAddress(DSP_ADDRESS_DELAY, DELAY_HPF_POS);
	params[5] = new BaseParam(BaseParam::GUI_PARAMETER_PAN, "LPF", &prog_data[DELAY_PAN1]);
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

ParamListMenu* ModulesMenu::createReverbMenu()
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

	ParamListMenu* menu = new ParamListMenu(this, MENU_REVERB);
	if(menu) menu->setParams(params, paramNum, 3);

	typeSelect->setAffectedParamsList(params, paramNum);

	return menu;
}

ParamListMenu* ModulesMenu::createTremoloMenu()
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

	const char* paramNames2[3][5] = {"1    ", "1/1.5", "1/2  ", "1/3  ", "1/4  "};
	params[5] = new StringParam("TAP", &prog_data[TREMOLO_TAP], (char**)&paramNames2, 5);
	params[5]->setDspAddress(DSP_ADDRESS_TREMOLO, NOT_SEND_POS);

	ParamListMenu* menu = new ParamListMenu(this, MENU_TREMOLO);
	if(menu) menu->setParams(params, paramNum, 2);

	return menu;
}
