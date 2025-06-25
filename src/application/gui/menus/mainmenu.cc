#include "mainmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "modules.h"

#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "attenuatormenu.h"
#include "mastervolumemenu.h"
#include "mastereqmenu.h"
#include "systemmenu.h"
#include "tunermenu.h"

extern uint8_t tim5_fl2;
extern gui_menu_type current_menu;
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
	current_menu = m_menuType;
	currentMenu = this;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(prog1);

	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1])) DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2])) DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3])) DisplayTask->IndFoot(2, contr_kn[2]);
}

void MainMenu::task()
{
	if(current_menu != MENU_MAIN) return;

	if(prog1 != prog)
	{
		if(tim5_fl == 0)
		{
			DisplayTask->Prog_ind(prog1);
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
	if(current_menu != MENU_MAIN) return;

	prog = prog1;
	eepr_read_imya(prog1);
	imya_temp = 1;
	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(prog1);
	sys_para[31] = prog;
	prog_ch();

	clean_flag();
	tim5_start(1);
}

void MainMenu::encoderClockwise()
{
	if(current_menu != MENU_MAIN) return;

	if(prog1 == 98) prog1 = 0;
	else prog1++;

	eepr_read_imya(prog1);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
	DisplayTask->Prog_ind(prog1);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);

	tim5_start(0);
	TIM_SetCounter(TIM6,0x8000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	clean_flag();
}

void MainMenu::encoderCounterClockwise()
{
	if(current_menu != MENU_MAIN) return;

	if(prog1 == 0)prog1 = 98;
	else prog1 -= 1;

	eepr_read_imya(prog1);
	imya_temp = 1;
	DisplayTask->Main_scr();
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);

	DisplayTask->Prog_ind(prog1);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);

	TIM_SetCounter(TIM6,0xa000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	tim5_start(0);
}

void MainMenu::keyUp()
{
	if(current_menu != MENU_MAIN) return;
/*
	if(prog1 != prog)
	{
		prog_cur = 0;
		prog1 = prog;
		eepr_read_imya(prog1);
		DisplayTask->Main_scr();
		DisplayTask->Prog_ind(prog1);
		contr_kn[0] = contr_kn[1] = contr_kn[2] = contr_kn1[0] = contr_kn1[1] = contr_kn1[2] = 0;
		if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
		if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
		if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
	}
	else
	{
		if((!prog_sym_cur) && presetData[cab])
		{
			DisplayTask->Clear();
			current_menu = MENU_CABNAME;
			if(name_buf[0])
			{
				if(cab_type == 2)
				{
					DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)"1 - ");
					DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf + 1);
				}
				else DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf + 1);

				cab_n_fl1 = 1;
				cab_n_fl2 = 0;
			}
			else
			{
				if(cab_type == 2)
				{
					DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)"2 - ");
					DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf1 + 1);
					cab_n_fl1 = 0;
					cab_n_fl2 = 1;
				}
			}
			tim5_start(0);
			tim_fl = 1;
		}
	}
*/
	clean_flag();
}

void MainMenu::keyDown()
{
	if(current_menu != MENU_MAIN) return;

	eq_num = prog;

	shownChildMenu = &modulesMenu;
	modulesMenu.show();

	prog_cur = 1;

	tim5_start(0);
	clean_flag();
}

void MainMenu::key1()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new AttenuatorMenu(this);
	shownChildMenu->show();

	clean_flag();
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

	clean_flag();
	tim5_start(0);
}

void MainMenu::key4()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = SystemMenu::create(this);
	shownChildMenu->show();

	clean_flag();
	tim5_start(0);
}

void MainMenu::key5()
{
	if(shownChildMenu) delete shownChildMenu;
	shownChildMenu = nullptr;

	shownChildMenu = new TunerMenu(this);
	shownChildMenu->show();

	clean_flag();
	tim5_start(0);
}

