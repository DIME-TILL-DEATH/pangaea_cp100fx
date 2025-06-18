#include "eqmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

const uint8_t lpf_hpf       [][9] ={"HPF","LPF","Presence","Position"};
const uint8_t eq_pre_post   [][5] ={"Post","Pre "};
const uint8_t gerz[] = "Hz";
const uint8_t eq_p_l[][2] = {"F","Q"};

const uint8_t def_eq_band [][10]={"F 120Hz ","F 360Hz ","F 800Hz ","F 2000Hz","F 6000Hz"};

EqMenu::EqMenu(AbstractMenu* parent)
{
	m_menuType = MENU_EQ;
	topLevelMenu = parent;
}

void EqMenu::show(TShowMode showMode)
{
	current_menu = m_menuType;
	currentMenu = this;

	eq_num = 0;
	DisplayTask->EqInit();
}

void EqMenu::task()
{
	if(eq_num < 5)
	{
		if(tim5_fl == 0) DisplayTask->EqIndic(27 + eq_num*14, 0, prog_data[eq1 + eq_num], 0);
		else DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
	}
	else
	{
		if(tim5_fl == 0) DisplayTask->StringOut(6, eq_num - 5, TDisplayTask::fntSystem, 0, (uint8_t*)lpf_hpf + (eq_num - 5)*9);
		else DisplayTask->StringOut(6, eq_num - 5, TDisplayTask::fntSystem, 2, (uint8_t*)lpf_hpf + (eq_num - 5)*9);
	}
}

void EqMenu::encoderPressed()
{
	if(encoderKnobSelected == 0)
	{
		encoderKnobSelected = 1;
		if(eq_num < 5)DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
		else DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
	}
	else
	{
		encoderKnobSelected = 0;
		if(eq_num < 5)DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
		else DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
	}

	tim5_start(1);
	clean_flag();
}

void EqMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(eq_num == 4)
		{
			DisplayTask->Clear();
			for(uint8_t i = 0 ; i < 2 ; i++)
			{
				DisplayTask->StringOut(93,i,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
				DisplayTask->StringOut(6,1 + i,TDisplayTask::fntSystem,0,(uint8_t*)lpf_hpf + (1 + i)*9);
			}
			eq_num++;
			DisplayTask->StringOut(6,0,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf);
			DisplayTask->EqLH(prog_data[hpf_v]*(980.0/127.0)+20.0,0);
			DisplayTask->StringOut(28,0,TDisplayTask::fntSystem,0,(uint8_t*)"(");
			DisplayTask->StringOut(50,0,TDisplayTask::fntSystem,0,(uint8_t*)")");
			DisplayTask->ParamIndicNum(33,0,prog_data[hpf_v]);
			DisplayTask->EqLH(powf(127 - prog_data[lpf_v],2.0)*(19000.0/powf(127.0,2.0))+1000.0,1);
			DisplayTask->StringOut(28,1,TDisplayTask::fntSystem,0,(uint8_t*)"(");
			DisplayTask->StringOut(50,1,TDisplayTask::fntSystem,0,(uint8_t*)")");
			DisplayTask->ParamIndicNum(33,1,prog_data[lpf_v]);
			DisplayTask->ParamIndic(56,2,prog_data[pre_v]);
			DisplayTask->Icon_Strel(ICON_EQ, STRELKA_UP);
			DisplayTask->StringOut(6,3,TDisplayTask::fntSystem,0,(uint8_t*)lpf_hpf + 3*9);
			DisplayTask->StringOut(65,3,TDisplayTask::fntSystem,0,(uint8_t*)eq_pre_post + prog_data[eq_pr_po] * 5);
			tim5_start(0);
		}
		else
		{
			if((eq_num < 8) && (eq_num > 4))
			{
				DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,0,(uint8_t*)lpf_hpf + (eq_num++ - 5)*9);
				DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
				tim5_start(0);
			}
		}
		if(eq_num < 4)
		{
			DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num++],0);
			DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
			tim5_start(0);
		}
	}
	else {
		if(eq_num < 5)
		{
			if(prog_data[eq1 + eq_num] < 30)DisplayTask->EqIndic(27 + 14 * eq_num , 0 , ++prog_data[eq1 + eq_num] , 1);
		}
		else {
			switch(eq_num){
			case 5:if(prog_data[hpf_v] < 127)
				{
					prog_data[hpf_v] = enc_speed_inc(prog_data[hpf_v],127);
					DisplayTask->ParamIndicNum(33,0,prog_data[hpf_v]);
					DisplayTask->EqLH(prog_data[hpf_v]*(980.0/127.0)+20.0,0);
				}break;
			case 6:if(prog_data[lpf_v] > 0)
				{
					prog_data[lpf_v] = enc_speed_dec(prog_data[lpf_v],0);
					DisplayTask->ParamIndicNum(33,1,prog_data[lpf_v]);
					DisplayTask->EqLH(powf(127 - prog_data[lpf_v],2.0)*(19000.0/powf(127.0,2.0))+1000.0,1);
				}break;
			case 7:if(prog_data[pre_v] < 127)
				{
					prog_data[pre_v] = enc_speed_inc(prog_data[pre_v],127);
					DisplayTask->ParamIndic(56,eq_num - 5,prog_data[pre_v]);
				}break;
			case 8:if(!prog_data[eq_pr_po])
				{
					DisplayTask->StringOut(65,3,TDisplayTask::fntSystem,0,(uint8_t*)eq_pre_post + ++prog_data[eq_pr_po] * 5);
				}break;
			}
		}
	}
	if(eq_num != 8)gui_send(4,eq_num);
	else gui_send(4,12);

	clean_flag();
}

void EqMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(eq_num == 5)
		{
			DisplayTask->EqInit();
			DisplayTask->Icon_Strel(ICON_EQ, STRELKA_DOWN);
			eq_num--;
			DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
			tim5_start(0);
		}
		else {
			if((eq_num > 0) && (eq_num < 5))
			{
				DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num--],0);
				DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
				tim5_start(0);
			}
		}
		if(eq_num > 5)
		{
			DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,0,(uint8_t*)lpf_hpf + (eq_num-- - 5)*9);
			DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
			tim5_start(0);
		}
	}
	else
	{
		if(eq_num < 5)
		{
			if(prog_data[eq1 + eq_num] > 0)DisplayTask->EqIndic(27 + 14 * eq_num , 0 , --prog_data[eq1 + eq_num] , 1);
		}
		else {
			switch(eq_num){
			case 5:if(prog_data[hpf_v] > 0)
				{
					prog_data[hpf_v] = enc_speed_dec(prog_data[hpf_v],0);
					DisplayTask->ParamIndicNum(33,0,prog_data[hpf_v]);
					DisplayTask->EqLH(prog_data[hpf_v]*(980.0/127.0)+20.0,0);
				}break;
			case 6:if(prog_data[lpf_v] < 127)
				{
					prog_data[lpf_v] = enc_speed_inc(prog_data[lpf_v],127);
					DisplayTask->ParamIndicNum(33,1,prog_data[lpf_v]);
					DisplayTask->EqLH(powf(127 - prog_data[lpf_v],2.0)*(19000.0/powf(127.0,2.0))+1000.0,1);
				}break;
			case 7:if(prog_data[pre_v] > 0)
				{
					prog_data[pre_v] = enc_speed_dec(prog_data[pre_v],0);
					DisplayTask->ParamIndic(56,2,prog_data[pre_v]);
				}break;
			case 8:if(prog_data[eq_pr_po])
				{
					DisplayTask->StringOut(65,3,TDisplayTask::fntSystem,0,(uint8_t*)eq_pre_post + --prog_data[eq_pr_po] * 5);
				}break;
			}
		}
		if(eq_num != 8)gui_send(4,eq_num);
		else gui_send(4,12);
	}
	clean_flag();
}

//void EqMenu::keyUp()
//{
//	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
//}

void EqMenu::keyDown()
{
	if((!encoderKnobSelected) && (eq_num < 5))
	{
		current_menu = MENU_EQ_BAND;
		DisplayTask->Clear();
		DisplayTask->StringOut(6,0,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l);
		int8_t a =  prog_data[f1 + eq_num];
		DisplayTask->EqPar( 40 , 0 , a , 0 , eq_num);
		DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + 2);
		a =  prog_data[q1 + eq_num];
		DisplayTask->EqPar(40 , 1 , a , 1 , eq_num);
		DisplayTask->StringOut(80,0,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
		DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)"Press EQ for Default");
		DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)def_eq_band + eq_num * 10);
		DisplayTask->StringOut(78,3,TDisplayTask::fntSystem,0,(uint8_t*)"Q 0.70");
		par_num = 0;
		encoderKnobSelected = 0;
	}
	tim5_start(0);
	clean_flag();
}


