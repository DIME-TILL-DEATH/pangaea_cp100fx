#include "modulesmenu.h"

#include "../guimodules.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

#include "modules.h"

extern volatile uint8_t preset_edited;
extern gui_menu_type current_menu;

extern uint8_t control_destin;


const uint8_t ModulesMenu::cc[];
const uint8_t ModulesMenu::contr_list[][7];
const uint8_t ModulesMenu::volum[];
const uint8_t ModulesMenu::s_to_ch[];

const uint8_t ModulesMenu::sd_nr [];
const uint8_t ModulesMenu::imp_dir_n [];
const uint8_t ModulesMenu::imp_dir_no [ ];
const uint8_t ModulesMenu::sd_lo [];

const uint8_t ModulesMenu::s_t_c_list    [][7];

GuiModules::TModule modules[14];

GuiModules::TModule RF = {"RF", &prog_data[ENABLE_RESONANCE_FILTER], &GuiModules::createRfMenu, nullptr, ENABLE_RESONANCE_FILTER};
GuiModules::TModule GT = {"GT", &prog_data[ENABLE_GATE], &GuiModules::createGateMenu, nullptr, ENABLE_GATE};
GuiModules::TModule CM = {"CM", &prog_data[ENABLE_COMPRESSOR], &GuiModules::createCompressorMenu, nullptr, ENABLE_COMPRESSOR};
GuiModules::TModule PR = {"PR", &prog_data[ENABLE_PREAMP], &GuiModules::createPreampMenu, nullptr, ENABLE_PREAMP};
GuiModules::TModule PA = {"PA", &prog_data[ENABLE_AMP], &GuiModules::createAmpMenu, nullptr, ENABLE_AMP};
GuiModules::TModule IR = {"IR", &prog_data[ENABLE_CAB], &GuiModules::createIrMenu, &ModulesMenu::enableCab, ENABLE_CAB};
GuiModules::TModule EQ = {"EQ", &prog_data[ENABLE_EQ], &GuiModules::createEqMenu, nullptr, ENABLE_EQ};
GuiModules::TModule PH = {"PH", &prog_data[ENABLE_PHASER], &GuiModules::createPhaserMenu, nullptr, ENABLE_PHASER};
GuiModules::TModule FL = {"FL", &prog_data[ENABLE_FLANGER], &GuiModules::createFlangerMenu, nullptr, ENABLE_FLANGER};
GuiModules::TModule CH = {"CH", &prog_data[ENABLE_CHORUS], &GuiModules::createChorusMenu, nullptr, ENABLE_CHORUS};
GuiModules::TModule DL = {"DL", &prog_data[ENABLE_DELAY], &GuiModules::createDelayMenu, nullptr, ENABLE_DELAY};
GuiModules::TModule ER = {"ER", &prog_data[ENABLE_EARLY_REFLECTIONS], &GuiModules::createEarlyMenu, nullptr, ENABLE_EARLY_REFLECTIONS};
GuiModules::TModule RV = {"RV", &prog_data[ENABLE_REVERB], &GuiModules::createReverbMenu, nullptr, ENABLE_REVERB};
GuiModules::TModule TR = {"TR", &prog_data[ENABLE_TREMOLO], &GuiModules::createTremoloMenu, nullptr, ENABLE_TREMOLO};


ModulesMenu::ModulesMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MODULES;

	modules[0] = RF;
	modules[1] = GT;
	modules[2] = CM;
	modules[3] = PR;
	modules[4] = PA;
	modules[5] = IR;
	modules[6] = EQ;
	modules[7] = PH;
	modules[8] = FL;
	modules[9] = CH;
	modules[10] = DL;
	modules[11] = ER;
	modules[12] = RV;
	modules[13] = TR;
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

	uint8_t xCoord, yCoord;

	if(m_numMenu < 7)
	{
		xCoord = 2 + 18*m_numMenu;
		yCoord = 0;
	}
	else
	{
		xCoord = 2 + 18*(m_numMenu - 7);
		yCoord = 2;
	}

	if(tim5_fl == 0 )DisplayTask->EfIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, *modules[m_numMenu].enablePtr);
	else DisplayTask->EfIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, 2);
}

void ModulesMenu::encoderPressed()
{
	if(current_menu != MENU_MODULES) return;

	*modules[m_numMenu].enablePtr = !((bool)*modules[m_numMenu].enablePtr);
	gui_send(18, modules[m_numMenu].dspEnablePosition | (*modules[m_numMenu].enablePtr << 8));

	if(modules[m_numMenu].enableFunction) modules[m_numMenu].enableFunction();

	clean_flag();
	tim5_start(1);
}

void ModulesMenu::encoderClockwise()
{
	if(current_menu != MENU_MODULES) return;

	icon_refresh(m_numMenu);

	if(m_numMenu < 13) m_numMenu++;
	else m_numMenu = 0;

	tim5_start(0);

	clean_flag();
}

void ModulesMenu::encoderCounterClockwise()
{
	if(current_menu != MENU_MODULES) return;


	icon_refresh(m_numMenu);

	if(m_numMenu > 0) m_numMenu--;
	else m_numMenu = 13;

	tim5_start(0);

	clean_flag();
}

void ModulesMenu::keyUp()
{
	if(current_menu != MENU_MODULES) return;

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
		prog_cur = 0;
		m_numMenu = 0;
		DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
		DisplayTask->Menu_init();
		tim5_start(0);
	}

	clean_flag();
}

void ModulesMenu::keyDown()
{
	if(current_menu != MENU_MODULES) return;

	if(prog1 == prog)
	{
		if(*modules[m_numMenu].enablePtr)
		{
			shownChildMenu = modules[m_numMenu].createMenu(this);
			shownChildMenu->show();
		}
	}

	tim5_start(0);
	clean_flag();
}

void ModulesMenu::key1()
{
	if(current_menu != MENU_MODULES) return;


	DisplayTask->Clear();

	// было своё copyMenu

	current_menu = MENU_ERASE;
	par_num = 0;

	DisplayTask->StringOut(36,2,TDisplayTask::fntSystem,0,(uint8_t*)"Preset");
	DisplayTask->StringOut(12,3,TDisplayTask::fntSystem,0,(uint8_t*)"Default Settings?");
	DisplayTask->StringOut(30,0,TDisplayTask::fntSystem,0,(uint8_t*)"No      Yes");

	clean_flag();
	tim5_start(0);
}

void ModulesMenu::key2()
{
	if(current_menu != MENU_MODULES) return;

	// было своё copyMenu


	DisplayTask->Clear();
	par_num = 0;

	current_menu = MENU_VOLUME;
	DisplayTask->StringOut(28,0,TDisplayTask::fntSystem , 0 , (uint8_t*)volum);
	DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
	if(prog_data[vol_contr])DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Off");
	else DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"On ");
	DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
	DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
	vol_vol = prog_data[pres_lev];
	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	clean_flag();
	tim5_start(0);
}

void ModulesMenu::key3()
{
	if(current_menu != MENU_MODULES) return;

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

	clean_flag();
}

void ModulesMenu::key4()
{
	if(current_menu != MENU_MODULES) return;

	shownChildMenu = &nameEditMenu;
	shownChildMenu->show();

	clean_flag();

}

void ModulesMenu::key5()
{
	if(current_menu != MENU_MODULES) return;

	shownChildMenu = &copyMenu;
	shownChildMenu->show();

	clean_flag();
}

void ModulesMenu::icon_refresh(uint8_t num)
{
	if(num < 7) DisplayTask->EfIcon(2 + 18*num, 0,(uint8_t*)modules[num].name, *modules[num].enablePtr);
	else DisplayTask->EfIcon(2 + 18*(num - 7), 2,(uint8_t*)modules[num].name, *modules[num].enablePtr);
}

void ModulesMenu::enableCab()
{
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
