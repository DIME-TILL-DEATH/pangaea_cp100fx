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

//#include "menu_gate.h"
#include "menu_compressor.h"
#include "menu_preamp.h"
#include "menu_pa.h"

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

const uint8_t ModulesMenu::moog_list     [][8];
const uint8_t ModulesMenu::moog_typ_list [][4];
const uint8_t ModulesMenu::moog_mod_list [][4];
const uint8_t ModulesMenu::moog_gen_type [][7];
const uint8_t ModulesMenu::phas_list     [][8];
const uint8_t ModulesMenu::chor_list     [][8];
const uint8_t ModulesMenu::chor_list1    [][8];
const uint8_t ModulesMenu::chor_list2    [][8];
const uint8_t ModulesMenu::fl_list       [][8];
const uint8_t ModulesMenu::fl_t          [][9];
const uint8_t ModulesMenu::ch_list_t     [][12];
const uint8_t ModulesMenu::del_list      [][8];
const uint8_t ModulesMenu::del_tim_l     [][5];
const uint8_t ModulesMenu::tap_tim       [][6];
const uint8_t ModulesMenu::rev_list      [][7];
const uint8_t ModulesMenu::rev_type_list [][8];
const uint8_t ModulesMenu::ear_list      [][5];
const uint8_t ModulesMenu::s_t_c_list    [][7];
const uint8_t ModulesMenu::tr_list       [][8];
const uint8_t ModulesMenu::tr_t_list     [][7];
const uint8_t ModulesMenu::tr_lfo_t_list [][9];

ModulesMenu::ModulesMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MODULES;

	const uint8_t gateParamNum = 3;
	BaseParam* gateParams[gateParamNum];
	gateParams[0] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Thresh", &prog_data[GATE_THRESHOLD]);
	gateParams[1] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Attack", &prog_data[GATE_ATTACK]);
	gateParams[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Decay", &prog_data[GATE_DECAY]);
	for(int i=0; i<gateParamNum; i++) gateParams[i]->setDspAddress(DSP_ADDRESS_GATE, i);

	gateMenu.setParams(gateParams, gateParamNum);
}

void ModulesMenu::show()
{

}

void ModulesMenu::task()
{
	if(current_menu != MENU_MAIN) return;

	switch (num_men)
	{
	case 0:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(2,0,(uint8_t*)rf,prog_data[ENABLE_RESONANCE_FILTER]);
	  else DisplayTask->Ef_icon(2,0,(uint8_t*)rf,2);
	  break;
	case 1:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(20,0,(uint8_t*)gt,prog_data[ENABLE_GATE]);
	  else DisplayTask->Ef_icon(20,0,(uint8_t*)gt,2);
	  break;
	case 2:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(38,0,(uint8_t*)cm,prog_data[ENABLE_COMPRESSOR]);
	  else DisplayTask->Ef_icon(38,0,(uint8_t*)cm,2);
	  break;
	case 3:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(56,0,(uint8_t*)pr,prog_data[ENABLE_PREAMP]);
	  else DisplayTask->Ef_icon(56,0,(uint8_t*)pr,2);
	  break;
	case 4:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(74,0,(uint8_t*)am,prog_data[ENABLE_AMP]);
	  else DisplayTask->Ef_icon(74,0,(uint8_t*)am,2);
	  break;
	case 5:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(92,0,(uint8_t*)cs,prog_data[ENABLE_CAB]);
	  else DisplayTask->Ef_icon(92,0,(uint8_t*)cs,2);
	  break;
	case 6:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(110,0,(uint8_t*)equ,prog_data[ENABLE_EQ]);
	  else DisplayTask->Ef_icon(110,0,(uint8_t*)equ,2);
	  break;
	case 7:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(2,2,(uint8_t*)ph,prog_data[ENABLE_PHASER]);
	  else DisplayTask->Ef_icon(2,2,(uint8_t*)ph,2);
	  break;
	case 8:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(20,2,(uint8_t*)rm,prog_data[ENABLE_FLANGER]);
	  else DisplayTask->Ef_icon(20,2,(uint8_t*)rm,2);
	  break;
	case 9:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(38,2,(uint8_t*)ch,prog_data[ENABLE_CHORUS]);
	  else DisplayTask->Ef_icon(38,2,(uint8_t*)ch,2);
	  break;
	case 10:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(56,2,(uint8_t*)dl,prog_data[ENABLE_DELAY]);
	  else DisplayTask->Ef_icon(56,2,(uint8_t*)dl,2);
	  break;
	case 11:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(74,2,(uint8_t*)er,prog_data[ENABLE_EARLY_REFLECTIONS]);
	  else DisplayTask->Ef_icon(74,2,(uint8_t*)er,2);
	  break;
	case 12:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(92,2,(uint8_t*)rv,prog_data[ENABLE_REVERB]);
	  else DisplayTask->Ef_icon(92,2,(uint8_t*)rv,2);
	  break;
	case 13:
	  if(tim5_fl == 0)DisplayTask->Ef_icon(110,2,(uint8_t*)tr,prog_data[ENABLE_TREMOLO]);
	  else DisplayTask->Ef_icon(110,2,(uint8_t*)tr,2);
	  break;
	}
}

void ModulesMenu::encoderPressed()
{
	if(current_menu != MENU_MAIN) return;


	switch (num_men)
	{
		case 0:
			prog_data[ENABLE_RESONANCE_FILTER] = !(bool)prog_data[ENABLE_RESONANCE_FILTER];
			gui_send(18, 13 | (prog_data[ENABLE_RESONANCE_FILTER] << 8));
			break;
		case 1:
			prog_data[ENABLE_GATE] = !(bool)prog_data[ENABLE_GATE];
			gui_send(DSP_ADDRESS_MODULES_ENABLE, ENABLE_GATE | (prog_data[ENABLE_GATE] << 8));
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
						  gui_send(15,1);
						  ind_poin = 500;
						  vol_fl = 1;
						  inp_ind_fl = 0;
						  out_ind_fl = 1;
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

	clean_flag();
	tim5_start(1);
}

void ModulesMenu::encoderClockwise()
{
	if(current_menu != MENU_MAIN) return;

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
	if(current_menu != MENU_MAIN) return;

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
	if(current_menu != MENU_MAIN) return;

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
			gui_send(15,0);
			DisplayTask->Menu_init();
			tim5_start(0);
		}

		clean_flag();
	}
}

void ModulesMenu::keyDown()
{
	if(current_menu != MENU_MAIN) return;

	//-------------------------------KEYS-----------------------------------------------
	if((comp_fl == 0) && (prog1 == prog))
	{
		switch (num_men){
		case 0:
		  if(prog_data[moog] == 1)
		  {
			  current_menu = MENU_RESONANCE_FILTER;
			  DisplayTask->Clear();
			  DisplayTask->Icon_Strel(15,2);
			  for(uint8_t i = 0 ; i < 4 ; i++)
				{
				  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + i*8);
				  switch(i){
				  case 0:DisplayTask->ParamIndicMix(53,i,prog_data[mog_mix]);break;
				  case 1:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_typ_list + prog_data[mog_ftype]*4);break;
				  case 2:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_mod_list + prog_data[mog_fmod]*4);break;
				  case 3:DisplayTask->ParamIndic(53,i,prog_data[mog_rate]);break;
				  }
				}
			  par_num = 0;
		  }
		  break;
		case 1:
		  if(prog_data[gate] == 1)
		  {
//			  current_menu = MENU_GATE;
//			  GUI_gate_select();
			  currentMenu = &gateMenu;
			  gateMenu.show();
		  }
		  break;
		case 2:
		  if(prog_data[compr] == 1)
		  {
			  current_menu = MENU_COMPRESSOR;
			  GUI_compressor_select();
		  }
		  break;
		case 3:
		  if(prog_data[pream] == 1)
		  {
			  GUI_preamp_select();
			  current_menu = MENU_PREAMP;
			  gui_send(15, 8);
		  }
		 break;
		case 4:
		  if(prog_data[amp] == 1)
		  {
			  GUI_amp_select();
			  current_menu = MENU_PA;
			  gui_send(15,2);
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
			   gui_send(15,1);
			   ind_poin = 500;
			   vol_fl = 1;
			   inp_ind_fl = 0;
			   out_ind_fl = 1;
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
		   DisplayTask->Icon_Strel(4,2);
		  }
		  break;
		case 7:
		  if(prog_data[phas] == 1){
		  current_menu = MENU_PHASER;
		  gui_send(15,7);
		  DisplayTask->Clear();
		  DisplayTask->Icon_Strel(9,2);
		  for(uint8_t i = 0 ; i < 4 ; i++)
			{
			  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)phas_list + i*8);
			  if(i)DisplayTask->ParamIndic(53,i,prog_data[phaser_vol + i]);
			  else DisplayTask->ParamIndicMix(53,0,prog_data[phaser_vol]);
			}
		  par_num = 0;
		 }
		  break;
		case 8:
			if(prog_data[fl] == 1){
			current_menu = MENU_FLANGER;
			gui_send(15,4);
			DisplayTask->Clear();
			DisplayTask->Icon_Strel(10,2);
			for(uint8_t i = 0 ; i < 4 ; i++)
			  {
				DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)fl_list + i*8);
				if(!i)DisplayTask->ParamIndicMix(53,0,prog_data[fl_v]);
				else {
					if(i == 1)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem,0,(uint8_t*)fl_t + prog_data[fl_lfo]*9);
					else DisplayTask->ParamIndic(53,i,prog_data[fl_v + i]);
				}
			  }
			par_num = 0;
		   }
		  break;
		case 9:
		  if(prog_data[chor] == 1){
		  current_menu = MENU_CHORUS;
		  gui_send(15,5);
		  DisplayTask->Clear();
		  DisplayTask->Icon_Strel(5,2);
		  for(uint8_t i = 0 ; i < 4 ; i++)
			{
			  if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list + i*8);
			  else {
				  if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list1 + i*8);
				  else DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list2 + i*8);
			  }
			  if(i)
			  {
				  if(prog_data[chor_typ] < 4)DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
				  else {
					  if((i == 1) && (prog_data[chor_typ] == 4))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
					  else if((i == 3) && (prog_data[chor_typ] == 5))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
				  }
			  }
			  else DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);
			}
		  par_num = 0;
		  }
		  break;
		case 10:
		  if(prog_data[delay] == 1){
		  current_menu = MENU_DELAY;
		  DisplayTask->Clear();
		  for(uint8_t i = 0 ; i < 4 ; i++)
			{
			  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + i*8);
			  if(i)
			  {
				  if(i > 1)DisplayTask->ParamIndic(53,i,prog_data[d_vol + i - 1]);
				  else {
					  if(!sys_para[tim_type])DisplayTask->DelayTimeInd(53,i,delay_time);
					  else {
						  DisplayTask->ParamIndicNum(53,i,60000/delay_time);
						  DisplayTask->StringOut(90,i,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
					  }
				  }
			  }
			  else DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);
			}
		  DisplayTask->Icon_Strel(0,2);
		  par_num = 0;
		  }
		  break;
		case 11:
		  if(prog_data[early] == 1){
		  current_menu = MENU_EARLY_REFLECTIONS;
		  DisplayTask->Clear();
		  DisplayTask->Icon_Strel(3,0);
		  for(uint8_t i = 0 ; i < 2 ; i++)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)ear_list + i*5);
		  DisplayTask->ParamIndicMix(53,0,prog_data[early_vol]);
		  DisplayTask->ParamIndic(53,1,prog_data[early_vol + 1]);
		  par_num = 0;
		 }
		  break;
		case 12:
		  if(prog_data[reve] == 1){
		  current_menu = MENU_REVERB;
		  DisplayTask->Clear();
		  DisplayTask->Icon_Strel(2,2);
		  for(uint8_t i = 0 ; i < 4 ; i++)
			{
			  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_list + i*7);
			  if(i != 1)
			  {
				if(i)
				{
					DisplayTask->ParamIndic(53,i,prog_data[r_vol + i - 1]);
					if(i == 3)
					{
						if(prog_data[rev_t] == 4)DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
					}
					else {
						if(i == 2)
						{
							if(prog_data[rev_t] > 4)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
						}
					}
				}
				else DisplayTask->ParamIndicMix(53,0,prog_data[r_vol]);
			  }
			  else  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_type_list + prog_data[rev_t] * 8);
			}
		  par_num = 0;
		 }
		  break;
		case 13:
		  if(prog_data[trem] == 1){
		  current_menu = MENU_TREMOLO;
		  gui_send(15,6);
		  DisplayTask->Clear();
		  DisplayTask->Icon_Strel(1,2);
		  for(uint8_t i = 0 ; i < 4 ; i++)
			{
			  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_list + i*8);
			  if(i == 2)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_lfo_t_list + prog_data[tr_lfo_t] * 9);
			  else {
				  if(i < 3)DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i]);
				  else DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i - 1]);
			  }
			}
		  par_num = 0;
		  }

		  break;
		}
		tim5_start(0);
		clean_flag();
	}
}

void ModulesMenu::key1()
{
	if(current_menu != MENU_MAIN) return;

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
	if(current_menu != MENU_MAIN) return;

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
	gui_send(15,0);
	inp_ind_fl = 0;
	out_ind_fl = 1;
	ind_poin = 500;
	vol_fl = 1;
	tim5_start(0);
	num_men_temp = num_men;

}

void ModulesMenu::key3()
{
	if(current_menu != MENU_MAIN) return;

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
	if(current_menu != MENU_MAIN) return;

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
	if(current_menu != MENU_MAIN) return;

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
	case 0:DisplayTask->Ef_icon(2,0,(uint8_t*)rf,prog_data[moog]);break;
	case 1:DisplayTask->Ef_icon(20,0,(uint8_t*)gt,prog_data[gate]);break;
	case 2:DisplayTask->Ef_icon(38,0,(uint8_t*)cm,prog_data[compr]);break;
	case 3:DisplayTask->Ef_icon(56,0,(uint8_t*)pr,prog_data[pream]);break;
	case 4:DisplayTask->Ef_icon(74,0,(uint8_t*)am,prog_data[amp]);break;
	case 5:DisplayTask->Ef_icon(92,0,(uint8_t*)cs,prog_data[cab]);break;
	case 6:DisplayTask->Ef_icon(110,0,(uint8_t*)equ,prog_data[eq]);break;
	case 7:DisplayTask->Ef_icon(2,2,(uint8_t*)ph,prog_data[phas]);break;
	case 8:DisplayTask->Ef_icon(20,2,(uint8_t*)rm,prog_data[fl]);break;
	case 9:DisplayTask->Ef_icon(38,2,(uint8_t*)ch,prog_data[chor]);break;
	case 10:DisplayTask->Ef_icon(56,2,(uint8_t*)dl,prog_data[delay]);break;
	case 11:DisplayTask->Ef_icon(74,2,(uint8_t*)er,prog_data[early]);break;
	case 12:DisplayTask->Ef_icon(92,2,(uint8_t*)rv,prog_data[reve]);break;
	case 13:DisplayTask->Ef_icon(110,2,(uint8_t*)tr,prog_data[trem]);break;
	}
}
