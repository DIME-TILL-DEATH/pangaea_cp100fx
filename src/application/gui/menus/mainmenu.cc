#include "mainmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "modules.h"
#include "footswitch.h"

#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "cabnamemenu.h"
#include "attenuatormenu.h"
#include "mastervolumemenu.h"
#include "mastereqmenu.h"
#include "systemmenu.h"
#include "tunermenu.h"


extern uint8_t copy_temp;
extern uint8_t cab_type;
extern volatile uint8_t prog_sym_cur;
extern volatile uint8_t cab_n_fl1;
extern volatile uint8_t cab_n_fl2;
extern volatile uint8_t tim_fl;


MainMenu::MainMenu()
{
	m_menuType = MENU_MAIN;
}

void MainMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(preselectedPresetNumber);

	if((sys_para[FSW1_PRESS_TYPE] == 1) || ((sys_para[FSW1_HOLD_TYPE] == 1) && sys_para[FSW1_MODE] == Footswitch::Double)) DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[FSW2_PRESS_TYPE] == 1) || ((sys_para[FSW2_HOLD_TYPE] == 1) && sys_para[FSW2_MODE] == Footswitch::Double)) DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE] == Footswitch::Double)) DisplayTask->IndFoot(2, contr_kn[2]);
}

void MainMenu::task()
{
	if(preselectedPresetNumber != currentPresetNumber)
	{
		if(blinkFlag_fl == 0)
		{
			DisplayTask->Prog_ind(preselectedPresetNumber);
			if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE]))
			{
				DisplayTask->IndFoot(2, contr_kn[2]);
			}
		}
		else
		{
			if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)
			{
				DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
				if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE]))
				{
					DisplayTask->IndFoot(2,contr_kn[2]);
				}
			}
		}
	}
}

void MainMenu::encoderPressed()
{
	currentPresetNumber = preselectedPresetNumber;
	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	sys_para[31] = currentPresetNumber;
	prog_ch();

	show();
	tim5_start(1);
}

void MainMenu::encoderClockwise()
{
	if(preselectedPresetNumber == 98) preselectedPresetNumber = 0;
	else preselectedPresetNumber++;

	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[FSW1_PRESS_TYPE] == 1) || ((sys_para[FSW1_HOLD_TYPE] == 1) && sys_para[FSW1_MODE]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[FSW2_PRESS_TYPE] == 1) || ((sys_para[FSW2_HOLD_TYPE] == 1) && sys_para[FSW2_MODE]))DisplayTask->IndFoot(1,contr_kn[1]);
	DisplayTask->Prog_ind(preselectedPresetNumber);
	if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE]))DisplayTask->IndFoot(2,contr_kn[2]);

	tim5_start(0);
	TIM_SetCounter(TIM6,0x8000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);
}

void MainMenu::encoderCounterClockwise()
{
	if(preselectedPresetNumber == 0)preselectedPresetNumber = 98;
	else preselectedPresetNumber -= 1;

	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[FSW1_PRESS_TYPE] == 1) || ((sys_para[FSW1_HOLD_TYPE] == 1) && sys_para[FSW1_MODE]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[FSW2_PRESS_TYPE] == 1) || ((sys_para[FSW2_HOLD_TYPE] == 1) && sys_para[FSW2_MODE]))DisplayTask->IndFoot(1,contr_kn[1]);

	DisplayTask->Prog_ind(preselectedPresetNumber);
	if((sys_para[FSW3_PRESS_TYPE] == 1) || ((sys_para[FSW3_HOLD_TYPE] == 1) && sys_para[FSW3_MODE]))DisplayTask->IndFoot(2,contr_kn[2]);

	TIM_SetCounter(TIM6,0xa000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	tim5_start(0);
}

void MainMenu::keyUp()
{
	if(preselectedPresetNumber != currentPresetNumber)
	{
		preselectedPresetNumber = currentPresetNumber;
		eepr_read_imya(preselectedPresetNumber);

		show();
	}
	else
	{
		if(shownChildMenu) delete shownChildMenu;

		shownChildMenu = new CabNameMenu(this);
		shownChildMenu->show();
	}

	tim5_start(0);
}

void MainMenu::keyDown()
{
	shownChildMenu = &modulesMenu;
	modulesMenu.show();

	tim5_start(0);
}

void MainMenu::key1()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new AttenuatorMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key2()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new MasterVolumeMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key3()
{
	if(shownChildMenu) delete shownChildMenu;
	shownChildMenu = nullptr;

	shownChildMenu = new MasterEqMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key4()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = SystemMenu::create(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key5()
{
	if(shownChildMenu) delete shownChildMenu;
	shownChildMenu = nullptr;

	shownChildMenu = new TunerMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

