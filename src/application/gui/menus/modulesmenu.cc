#include "modulesmenu.h"

#include "../guimodules.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

#include "modules.h"

#include "paramlistmenu.h"

#include "params/stringlistparam.h"
#include "params/stringoutparam.h"


extern gui_menu_type current_menu_type;

extern uint8_t control_destin;


const uint8_t ModulesMenu::cc[];
const uint8_t ModulesMenu::contr_list[][7];
const uint8_t ModulesMenu::volum[];

const uint8_t ModulesMenu::sd_nr [];
const uint8_t ModulesMenu::imp_dir_n [];
const uint8_t ModulesMenu::imp_dir_no [ ];
const uint8_t ModulesMenu::sd_lo [];


GuiModules::TModule modules[14];

GuiModules::TModule RF = {"RF", &presetData[ENABLE_RESONANCE_FILTER], &GuiModules::createRfMenu, nullptr, ENABLE_RESONANCE_FILTER};
GuiModules::TModule GT = {"GT", &presetData[ENABLE_GATE], &GuiModules::createGateMenu, nullptr, ENABLE_GATE};
GuiModules::TModule CM = {"CM", &presetData[ENABLE_COMPRESSOR], &GuiModules::createCompressorMenu, nullptr, ENABLE_COMPRESSOR};
GuiModules::TModule PR = {"PR", &presetData[ENABLE_PREAMP], &GuiModules::createPreampMenu, nullptr, ENABLE_PREAMP};
GuiModules::TModule PA = {"PA", &presetData[ENABLE_AMP], &GuiModules::createAmpMenu, nullptr, ENABLE_AMP};
GuiModules::TModule IR = {"IR", &presetData[ENABLE_CAB], &GuiModules::createIrMenu, &ModulesMenu::enableCab, ENABLE_CAB};
GuiModules::TModule EQ = {"EQ", &presetData[ENABLE_EQ], &GuiModules::createEqMenu, nullptr, ENABLE_EQ};
GuiModules::TModule PH = {"PH", &presetData[ENABLE_PHASER], &GuiModules::createPhaserMenu, nullptr, ENABLE_PHASER};
GuiModules::TModule FL = {"FL", &presetData[ENABLE_FLANGER], &GuiModules::createFlangerMenu, nullptr, ENABLE_FLANGER};
GuiModules::TModule CH = {"CH", &presetData[ENABLE_CHORUS], &GuiModules::createChorusMenu, nullptr, ENABLE_CHORUS};
GuiModules::TModule DL = {"DL", &presetData[ENABLE_DELAY], &GuiModules::createDelayMenu, nullptr, ENABLE_DELAY};
GuiModules::TModule ER = {"ER", &presetData[ENABLE_EARLY_REFLECTIONS], &GuiModules::createEarlyMenu, nullptr, ENABLE_EARLY_REFLECTIONS};
GuiModules::TModule RV = {"RV", &presetData[ENABLE_REVERB], &GuiModules::createReverbMenu, nullptr, ENABLE_REVERB};
GuiModules::TModule TR = {"TR", &presetData[ENABLE_TREMOLO], &GuiModules::createTremoloMenu, nullptr, ENABLE_TREMOLO};


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
	current_menu_type = m_menuType;
	currentMenu = this;

	if(showMode == TShowMode::FirstShow) presetEdited = false;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	DisplayTask->Menu_init();
	tim5_start(0);
}

void ModulesMenu::task()
{
	if(current_menu_type != MENU_MODULES) return;

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
	if(current_menu_type != MENU_MODULES) return;

	presetEdited = true;

	*modules[m_numMenu].enablePtr = !((bool)*modules[m_numMenu].enablePtr);
	gui_send(18, modules[m_numMenu].dspEnablePosition | (*modules[m_numMenu].enablePtr << 8));

	if(modules[m_numMenu].enableFunction) modules[m_numMenu].enableFunction();

	clean_flag();
	tim5_start(1);
}

void ModulesMenu::encoderClockwise()
{
	if(current_menu_type != MENU_MODULES) return;

	icon_refresh(m_numMenu);

	if(m_numMenu < 13) m_numMenu++;
	else m_numMenu = 0;

	tim5_start(0);

	clean_flag();
}

void ModulesMenu::encoderCounterClockwise()
{
	if(current_menu_type != MENU_MODULES) return;


	icon_refresh(m_numMenu);

	if(m_numMenu > 0) m_numMenu--;
	else m_numMenu = 13;

	tim5_start(0);

	clean_flag();
}

void ModulesMenu::keyUp()
{
	if(current_menu_type != MENU_MODULES) return;

	//parent->show(); //????

	if(presetEdited == 1)
	{
		confirmSaveMenu.setTopLevelMenu(topLevelMenu);
		saveDialog.setYesMenu(&confirmSaveMenu);
		saveDialog.setNoMenu(topLevelMenu);
		saveDialog.show();
	}
	else
	{
		topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
	}
	tim5_start(0);
	clean_flag();
}

void ModulesMenu::keyDown()
{
	if(current_menu_type != MENU_MODULES) return;

	if(preselectedPresetNumber == currentPresetNumber)
	{
		if(*modules[m_numMenu].enablePtr)
		{
			presetEdited = true;
			shownChildMenu = modules[m_numMenu].createMenu(this);
			shownChildMenu->show();
		}
	}

	tim5_start(0);
	clean_flag();
}

void ModulesMenu::key1()
{
	if(current_menu_type != MENU_MODULES) return;


	shownChildMenu = &erasePresetDialog;
	shownChildMenu->show();

	clean_flag();
	tim5_start(0);
}

void ModulesMenu::key2()
{
	if(current_menu_type != MENU_MODULES) return;

	// было своё copyMenu

	const uint8_t paramCount = 3;
	BaseParam* params[paramCount];

	params[0] = new StringOutParam("Preset level");
	params[0]->setDisplayPosition(40);
	params[1] = new StringListParam("Control", &presetData[PRESET_VOLUME_CONTROL], {"On ", "Off"}, 3);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &presetData[PRESET_VOLUME]);
	params[2]->setDspAddress(DSP_ADDRESS_PRESET_VOLUME, PARAM_EQUAL_POS);

	ParamListMenu* menu = new ParamListMenu(this, MENU_VOLUME);
	if(menu)
	{
		menu->setParams(params, paramCount);
		menu->setIcon(false, 0);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_OUT);
	}

	shownChildMenu = menu;
	shownChildMenu->show();

	clean_flag();
	tim5_start(0);


}

void ModulesMenu::key3()
{
	if(current_menu_type != MENU_MODULES) return;

	// было своё copyMenu

	current_menu_type = MENU_CONTROLLERS;
	control_destin = dest_tabl_start[presetControllers[1]];
	contrs = 0;
	DisplayTask->Clear();
	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		DisplayTask->StringOut(0,i,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_list + i*7);
		switch (i)
		{
		case 0:DisplayTask->ParamIndicNum(45,0,1);break;
		case 1:if(presetControllers[0] == 0)DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
			   else {
				 if(presetControllers[0] > 4)
				 {
					 DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
					 DisplayTask->ParamIndicNum(69,1,presetControllers[0] - 5);
				 }
				 else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (presetControllers[0]-1)*12);
			   }break;
		case 2:DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dest_list + presetControllers[1]*14);break;
		}
	}
	DisplayTask->Strel(58,3,0);
	par_num = 0;

	clean_flag();
}

void ModulesMenu::key4()
{
	if(current_menu_type != MENU_MODULES) return;

	// было своё copyMenu

	shownChildMenu = &nameEditMenu;
	shownChildMenu->show();

	clean_flag();

}

void ModulesMenu::key5()
{
	if(current_menu_type != MENU_MODULES) return;

	shownChildMenu = &copyMenu;
	shownChildMenu->show();

	clean_flag();
}

void ModulesMenu::icon_refresh(uint8_t num)
{
	DisplayTask->EfIcon(2 + (num%7) * 18, (num/7) * 2,(uint8_t*)modules[num].name, *modules[num].enablePtr);
}

void ModulesMenu::enableCab()
{
	if(cab1_name_buf[0] == 0)
	{
		kgp_sdk_libc::memset(preset_temp, 0, 24576);
		kgp_sdk_libc::memset(name_buf_temp, 0, 64);
		cab_num = 0;
		extern uint8_t sd_init_fl;

		if(sd_init_fl == 1)
		{
		 // current_menu = MENU_CABBROWSER;
		  DisplayTask->Clear();
		  extern volatile uint8_t imp_dir_fl;
		  if(imp_dir_fl)
		  {
			  vol_ind_level_pos = presetData[vol];
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
			  presetData[cab] = 0;
			  current_menu_type = MENU_MAIN;
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

			presetData[cab] = 0;
			current_menu_type = MENU_MAIN;
			encoder_knob_selected = 0;
			DisplayTask->Menu_init();
		}
	}
}
