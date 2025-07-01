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

#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "cabnamemenu.h"
#include "attenuatormenu.h"
#include "mastervolumemenu.h"
#include "mastereqmenu.h"
#include "systemmenu/systemmenu.h"
#include "tunermenu.h"

extern uint8_t tim5_fl2;
extern gui_menu_type current_menu_type;
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
	current_menu_type = m_menuType;
	currentMenu = this;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(preselectedPresetNumber);

	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1])) DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2])) DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3])) DisplayTask->IndFoot(2, contr_kn[2]);
}

void MainMenu::task()
{
	if(current_menu_type != MENU_MAIN) return;

	if(preselectedPresetNumber != currentPresetNumber)
	{
		if(tim5_fl == 0)
		{
			DisplayTask->Prog_ind(preselectedPresetNumber);
			if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))
			{
				DisplayTask->IndFoot(2, contr_kn[2]);
			}
		}
		else
		{
			if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)
			{
				DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
				if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))
				{
					DisplayTask->IndFoot(2,contr_kn[2]);
				}
			}
		}
	}
}

void MainMenu::encoderPressed()
{
	if(current_menu_type != MENU_MAIN) return;

	currentPresetNumber = preselectedPresetNumber;
	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(preselectedPresetNumber);
	sys_para[31] = currentPresetNumber;
	prog_ch();

	clean_flag();
	tim5_start(1);
}

void MainMenu::encoderClockwise()
{
	if(current_menu_type != MENU_MAIN) return;

	if(preselectedPresetNumber == 98) preselectedPresetNumber = 0;
	else preselectedPresetNumber++;

	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
	DisplayTask->Prog_ind(preselectedPresetNumber);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);

	tim5_start(0);
	TIM_SetCounter(TIM6,0x8000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	clean_flag();
}

void MainMenu::encoderCounterClockwise()
{
	if(current_menu_type != MENU_MAIN) return;

	if(preselectedPresetNumber == 0)preselectedPresetNumber = 98;
	else preselectedPresetNumber -= 1;

	eepr_read_imya(preselectedPresetNumber);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);

	DisplayTask->Prog_ind(preselectedPresetNumber);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);

	TIM_SetCounter(TIM6,0xa000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	tim5_start(0);
}

void MainMenu::keyUp()
{
	if(current_menu_type != MENU_MAIN) return;

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
	if(current_menu_type != MENU_MAIN) return;

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

	clean_flag();
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

