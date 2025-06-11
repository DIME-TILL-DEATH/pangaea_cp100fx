#include "mainmenu.h"

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


#include "menu_preamp.h"
#include "menu_pa.h"


extern volatile uint8_t comp_fl;
extern uint8_t tim5_fl2;
extern volatile uint8_t preset_edited;
extern gui_menu_type current_menu;
extern uint8_t copy_temp;
extern uint8_t cab_type;
extern volatile uint8_t prog_sym_cur;
extern volatile uint8_t cab_n_fl1;
extern volatile uint8_t cab_n_fl2;
extern volatile uint8_t tim_fl;


extern uint8_t temp_sys;

const uint8_t MainMenu::att_db[][4];
const char MainMenu::atten[];

const uint8_t MainMenu::cab_list[][8];
const uint8_t MainMenu::cab_out_list[][6];
const uint8_t MainMenu::cab_list_menu[][9];

const uint8_t MainMenu::decib[];

const uint8_t MainMenu::master_eq_of[];
const uint8_t MainMenu::master_eq_on[];
const uint8_t MainMenu::mas_eq_list[][9];
const uint8_t MainMenu::master_vo[][14];

MainMenu::MainMenu()
{
	m_menuType = MENU_MAIN;
}

void MainMenu::show()
{

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
				DisplayTask->IndFoot(2,contr_kn[2]);
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
	num_men = 0;

	clean_flag();
	tim5_start(1);
}

void MainMenu::encoderClockwise()
{
	if(current_menu != MENU_MAIN) return;

	if(!comp_fl)
	{
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
	}
	clean_flag();
}

void MainMenu::encoderCounterClockwise()
{
	if(current_menu != MENU_MAIN) return;

	if(!comp_fl)
	{
		if(prog1 == 0)prog1 = 98;
		else prog1 -= 1;

		eepr_read_imya(prog1);
		imya_temp = 1;
		DisplayTask->Main_scr();
		if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
		if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);

		DisplayTask->Prog_ind(prog1);
		if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
		tim5_start(0);
		TIM_SetCounter(TIM6,0xa000);
		TIM_ClearFlag(TIM6,TIM_FLAG_Update);
		TIM_Cmd(TIM6,ENABLE);
	}
}

void MainMenu::keyUp()
{
	if(current_menu != MENU_MAIN) return;

	//-------------------------------------------Key UP----------------------------------------------------------------
	if(!comp_fl)
	{
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
			if((!prog_sym_cur) && prog_data[cab])
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
		clean_flag();
	}
}

void MainMenu::keyDown()
{
	if(current_menu != MENU_MAIN) return;

	if((comp_fl == 0) && (prog1 == prog))
	{
		eq_num = prog;

		currentMenu = &modulesMenu;
		modulesMenu.show();

		preset_edited = 1;
		prog_cur = 1;
		tim5_start(0);
		clean_flag();
	}
}

void MainMenu::key1()
{
	if(current_menu != MENU_MAIN) return;

	clean_flag();
	DisplayTask->Clear();

	current_menu = MENU_ATTENUATOR;
	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)atten);
	DisplayTask->StringOut(76,0,TDisplayTask::fntSystem,0,(uint8_t*)att_db + sys_para[127]*4);
	DisplayTask->StringOut(95,0,TDisplayTask::fntSystem,0,(uint8_t*)decib);
	DisplayTask->StringOut(2,2,TDisplayTask::fntSystem, 0 , (uint8_t*)in_out_strings);
//	edit_modules_fl = 1;
	ind_poin = 500;
	inp_ind_fl = 1;
	out_ind_fl = 0;
	gui_send(15,9);

	tim5_start(0);
}

void MainMenu::key2()
{
	if(current_menu != MENU_MAIN) return;

	clean_flag();
	DisplayTask->Clear();
	par_num = 0;

	current_menu = MENU_MASTER_VOLUME;
	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)master_vo);
	DisplayTask->ParamIndicNum(85,0,sys_para[126]);
	DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + 14);
	DisplayTask->ParamIndicNum(85,1,sys_para[125]);
	tim5_start(0);
}

void MainMenu::key3()
{
	if(current_menu != MENU_MAIN) return;

	clean_flag();

	DisplayTask->Clear();
	current_menu = MENU_MASTER_EQ;
	if(!sys_para[120])
	{
		DisplayTask->StringOut(12,1,TDisplayTask::fntSystem,0,(uint8_t*)master_eq_of);
		DisplayTask->StringOut(1,3,TDisplayTask::fntSystem,0,(uint8_t*)master_eq_on);
	}
	else
	{
		for(uint8_t i = 0 ; i < 4 ; i++)
		{
			uint16_t a = sys_para[121 + i];
			if(i == 2)
			{
				a = sys_para[510] << 8;
				a |= sys_para[511];
				DisplayTask->EqPar(72,2,a,0,5);
				DisplayTask->StringOut(106,i,TDisplayTask::fntSystem,0,(uint8_t*)"Hz");
			}
			else {
				DisplayTask->ParamIndicMix(66,i,(a * (63.0f/24.0f)) + 63);
				DisplayTask->ParamIndicNum(100,i,abs(a/2));
				uint8_t b;
				if(abs(a/2) < 10)b = 112;
				else b = 115;
				if(a & 1)DisplayTask->StringOut(b,i,TDisplayTask::fntSystem,0,(uint8_t*)".5");
				else DisplayTask->StringOut(b,i,TDisplayTask::fntSystem,0,(uint8_t*)".0");
				if(a < 0)DisplayTask->SymbolOut(58,i,TDisplayTask::fntSystem,0,45);
				else if(a > 0)DisplayTask->SymbolOut(58,i,TDisplayTask::fntSystem,0,43);
				DisplayTask->StringOut(45,i,TDisplayTask::fntSystem,0,(uint8_t*)"dB");
			}
			if(!i)DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + i*9);
			else DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + i*9);
		}
	}
	par_num = 0;
	tim5_start(0);
}

void MainMenu::key4()
{
	if(current_menu != MENU_MAIN) return;

	clean_flag();

	temp_sys = sys_para[2];
	DisplayTask->Clear();
	current_menu = MENU_SYSTEM;
	DisplayTask->Clear();
	DisplayTask->Icon_Strel(ICON_SY, STRELKA_DOWN);
	for(uint8_t i = 0 ; i < 4 ; i++)
	{
		if(!i) DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + i*12);
		else DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + i*12);

		switch(i)
		{
			case 0:DisplayTask->StringOut(36,i,TDisplayTask::fntSystem,0,(uint8_t*)mode_list + sys_para[i]*12);break;
			case 1:DisplayTask->ParamIndicNum(60,i,sys_para[i] + 1);break;
			case 2:DisplayTask->StringOut(60,i,TDisplayTask::fntSystem,0,(uint8_t*)cab_out_list + sys_para[i]*6);break;
			case 3:if(!(sys_para[exp_typ] & 0x80))DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
				   else DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
			break;
		}
	}
	par_num = 0;
	tim5_start(0);
}

void MainMenu::key5()
{
	if(current_menu != MENU_MAIN) return;

	clean_flag();

	send_codec(0xa102);
	gui_send(13,0);
	tun_base_old = 0.0f;
	tun_ini();
	tuner_use = 1;
	current_menu = MENU_TUNER;
	tim5_start(0);
	gui_send(15,9);

}
