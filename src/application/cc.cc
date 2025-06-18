#include "appdefs.h"
#include "eepr.h"
#include "display.h"
#include "cc.h"
#include "cs.h"
#include "enc.h"
#include "display.h"
#include "fonts/allFonts.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "gui/gui_task.h"

TCCTask *CCTask;

void con_ch(uint8_t adr, uint8_t data);

volatile uint8_t midi_b[3];
uint8_t contrs;
uint8_t mid_fl;
uint8_t ext_fl;
uint8_t ext_f_fl;
uint8_t ext_data;
uint8_t ext_sourc = 255;
volatile uint8_t tap_del_fl;
volatile uint8_t tap_trem_fl;
volatile uint8_t tap_moog_fl;

const uint8_t dest_tabl[55] =
{25, 36, 0, 44, 45, 46, 47, 48, //7
		1, 2, 3, 4, 37, 38, 5, 49, //15
		50, 51, 52, 53, 6, 7, 8, 9, //23
		10, 11, 12, 13, 14, 15, 16, 17, //31
		18, 19, 20, 21, 54, 22, 23, 24, //39
		26, 27, 28, 29, 30, 31, 32, 35, //47
		33, 34, 39, 40, 41, 42, 43};   //53

const uint8_t dest_tabl_start[55] =
{2, 8, 9, 10, 11, 14, 20, 21, //7
		22, 23, 24, 25, 26, 27, 28, 29, //15
		30, 31, 32, 33, 34, 35, 36, 37, //23
		38, 0, 39, 40, 41, 42, 43, 44, //31
		45, 47, 48, 46, 1, 12, 13, 49, //39
		50, 51, 52, 53, 54, 3, 4, 5, //47
		6, 7, 15, 16, 17, 18, 19};   //53

const uint8_t midi_dect_list[][14] =
{
/*0*/"Preamp On Off",
/*1*/"Amplif On Off", "Amplif Volume", "Amplif Slave ",
/*4*/"CabSim On Off", "Equal On Off ",
/*6*/"Delay On Off ", "Delay Volume ", "Delay Fedback", "Delay Tap    ",
/*10*/"Phaser On Off", "Phaser Volume", "Phaser Rate  ",
/*13*/"Flanger OnOff", "Flang  Volume", "Flang  Rate  ",
/*16*/"Chorus On Off", "Chorus Volume", "Chorus Rate  ",
/*19*/"Reverb On Off", "Reverb Volume", "Reverb Time  ",
/*22*/"Tremolo OnOff", "Tremolo Inten", "Tremolo Rate ",
/*25*/"Preset Level ",
/*26*/"Tremolo TAP  ",
/*27*/"Compr On Off ", "Compr Thresh ", "Compr Volume ",
/*30*/"Filt  On Off ", "Filt LFOrate ", "Filt freq    ",
/*33*/"ER On Off    ", "ER Volume    ",
/*35*/"Filt LFO TAP ",
/*36*/"Vol Ct On Off",
/*37*/"Cab1 Volume  ", "Cab2 Volume  ",
/*39*/"Gate On Off  ", "Gate Thresh  ",
/*41*/"HPF frequency", "LPF frequency", "Presence val ",
/*44*/"Preamp Gain  ", "Preamp Volume", "Preamp Low   ", "Preamp Mid   ", "Preamp High  ",
/*49*/"Eq Band1 Lev ", "Eq Band2 Lev ", "Eq Band3 Lev ", "Eq Band4 Lev ", "Eq Band5 Lev ",
/*54*/"Reverb Type  "};

void con_ch(uint8_t adr, uint8_t data)
{
	float i = control[adr*4+2]+(control[adr*4+3]-control[adr*4+2])*data/127.0f;
	volatile uint8_t p = control[adr*4+1];
	switch(p)
	{
		case 0:
			if(i<64.0f)
				prog_data[pream] = 0;
			else
				prog_data[pream] = 1;
			contr_send(18, 10|(prog_data[pream]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(56, 0, (uint8_t*)pr, prog_data[pream]);
			}
		break; // if(!current_menu)&&(edit_modules_fl)
//-------------------------------------------------PA-------------------------------------------------
		case 1:
			if(i<64.0f)
				prog_data[amp] = 0;
			else
				prog_data[amp] = 1;
			contr_send(18, 0|(prog_data[amp]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(74, 0, (uint8_t*)am, prog_data[amp]);
			}
		break; //if(!current_menu)&&(edit_modules_fl)
		case 2:
			prog_data[am_v] = i;/*DisplayTask->ParamIndic(58,0,prog_data[am_v]);*/
			contr_send(11, 0);
		break;
		case 3:
			prog_data[am_out] = i;/*DisplayTask->ParamIndic(58,2,prog_data[am_out]);*/
			contr_send(11, 2);
		break;
//-------------------------------------------------CAB SIM--------------------------------------------
		case 4:
			if(i<64.0f)
				prog_data[cab] = 0;
			else
				prog_data[cab] = 1;
			contr_send(18, 1|(prog_data[cab]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(92, 0, (uint8_t*)cs, prog_data[cab]);
			}
		break; //if(!current_menu)&&(edit_modules_fl)
//-------------------------------------------------EQ-------------------------------------------------
		case 5:
			if(i<64.0f)
				prog_data[eq] = 0;
			else
				prog_data[eq] = 1;
			contr_send(18, 2|(prog_data[eq]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(110, 0, (uint8_t*)equ, prog_data[eq]);
			}
		break; //if(!current_menu)&&(edit_modules_fl)
//-------------------------------------------------Delay----------------------------------------------
		case 6:
			if(i<64.0f)
				prog_data[delay] = 0;
			else
				prog_data[delay] = 1;
			contr_send(18, 3|(prog_data[delay]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(56, 2, (uint8_t*)dl, prog_data[delay]);
			}
		break; //if(!current_menu)&&(edit_modules_fl)
		case 7:
			prog_data[d_vol] = i;/*DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);*/
			contr_send(3, 0);
		break;
		case 8:
			prog_data[d_fed] = i;/*DisplayTask->ParamIndic(53,2,prog_data[d_fed]);*/
			contr_send(3, 2);
		break;
		case 9:
			if(tap_temp_global()&&!sys_para[tap_typ])
			{
				delay_time = tap_global/3.0f/tap_tim_v[prog_data[d_tap_t]];
				if(delay_time<2731)
				{
					if(!sys_para[tim_type])
					{
						contr_send(3, 1);
						if(current_menu==MENU_DELAY)
							DisplayTask->DelayTimeInd(53, 1, delay_time);
						if(current_menu==MENU_TAP_DELAY)
							DisplayTask->DelayTimeInd(53, 0, delay_time);
					}
					else
					{
						uint8_t temp = 0;
						if(delay_time<2728&&delay_time>249)
						{
							while(delay_time<bpm_time[temp++]);

							delay_time = bpm_time[temp];
							contr_send(3, 1);

							if(current_menu==MENU_DELAY)
							{
								DisplayTask->ParamIndicNum(53, 1, 60000/delay_time);
								DisplayTask->StringOut(90, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
							}

							if(current_menu==MENU_TAP_DELAY)
							{
								DisplayTask->ParamIndicNum(53, 0, 60000/delay_time);
								DisplayTask->StringOut(90, 0, TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
							}
						}
					}
				}
				if(tap_trem_fl)
				{
					trem_time = tap_global/3.0f/tap_tim_v[prog_data[t_tap_t]];
					if(trem_time>2730)
						trem_time = 2730;
					contr_send(10, 5);
				}
				if(tap_moog_fl)
				{
					moog_time = tap_global/3.0f;
					contr_send(31, 13);
				}
			}
		break;
//-------------------------------------------------Phazer---------------------------------------------
		case 10:
			if(i<64.0f)
				prog_data[phas] = 0;
			else
				prog_data[phas] = 1;
			contr_send(18, 4|(prog_data[phas]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(2, 2, (uint8_t*)ph, prog_data[phas]);
			}
		break;
		case 11:
			prog_data[phaser_vol] = i;
			contr_send(5, 0);
		break;
		case 12:
			prog_data[phaser_rate] = i;
			contr_send(5, 1);
		break;
//-------------------------------------------------Flanger--------------------------------------------
		case 13:
			if(i<64.0f)
				prog_data[fl] = 0;
			else
				prog_data[fl] = 1;
			contr_send(18, 5|(prog_data[fl]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(20, 2, (uint8_t*)rm, prog_data[fl]);
			}
		break;
		case 14:
			prog_data[fl_v] = i;
			contr_send(8, 0);
		break;
		case 15:
			prog_data[fl_r] = i;
			contr_send(8, 2);
		break;
//-------------------------------------------------Chorus---------------------------------------------
		case 16:
			if(i<64.0f)
				prog_data[chor] = 0;
			else
				prog_data[chor] = 1;
			contr_send(18, 6|(prog_data[chor]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(38, 2, (uint8_t*)ch, prog_data[chor]);
			}
		break;
		case 17:
			prog_data[chor_volum] = i;/*DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);*/
			contr_send(9, 0);
		break;
		case 18:
			prog_data[chor_rat] = i;/*DisplayTask->ParamIndic(53,1,prog_data[chor_volum + 1]);*/
			contr_send(9, 1);
		break;
//-------------------------------------------------Reverb---------------------------------------------
		case 19:
			if(i<64.0f)
				prog_data[reve] = 0;
			else
				prog_data[reve] = 1;
			contr_send(18, 8|(prog_data[reve]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(92, 2, (uint8_t*)rv, prog_data[reve]);
			}
		break;
		case 20:
			prog_data[r_vol] = i;
			contr_send(2, 0);
		break;
		case 21:
			prog_data[r_time] = i;
			contr_send(2, 2);
		break;
//-------------------------------------------------Tremolo--------------------------------------------
		case 22:
			if(i<64.0f)
				prog_data[trem] = 0;
			else
				prog_data[trem] = 1;
			contr_send(18, 9|(prog_data[trem]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(110, 2, (uint8_t*)tr, prog_data[trem]);
			}
		break;
		case 23:
			prog_data[tr_vol] = i;
			contr_send(10, 0);
		break;
		case 24:
			prog_data[tr_rate] = i;
			contr_send(10, 1);
		break;
		case 26:
			if(!tap_del_fl)
			{
				if(tap_temp_global()&&!sys_para[tap_typ])
				{
					trem_time = tap_global/3.0f/tap_tim_v[prog_data[t_tap_t]];
					if(trem_time<2731)
						contr_send(10, 5);
					if(tap_moog_fl)
					{
						moog_time = tap_global/3.0f;
						contr_send(31, 13);
					}
				}
			}
		break;
//---------------------------------------------------------------------------------------------------
		case 25:
			if(!prog_data[vol_contr])
				contr_send(19, i);
		break;
//-------------------------------------------------Compressor--------------------------------------------
		case 27:
			if(i<64.0f)
				prog_data[compr] = 0;
			else
				prog_data[compr] = 1;
			contr_send(18, 12|(prog_data[compr]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(38, 0, (uint8_t*)cm, prog_data[compr]);
			}
		break;
		case 28:
			prog_data[comp_thr] = i;
			contr_send(30, 0);
		break;
		case 29:
			prog_data[comp_v] = i;
			contr_send(30, 2);
		break;
//-------------------------------------------------Moog filter-------------------------------------------
		case 30:
			if(i<64.0f)
				prog_data[moog] = 0;
			else
				prog_data[moog] = 1;
			contr_send(18, 13|(prog_data[moog]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(2, 0, (uint8_t*)rf, prog_data[moog]);
			}
		break;
		case 31:
			prog_data[mog_rate] = i;
			contr_send(31, 3);
		break;
		case 32:
			prog_data[mog_ext] = i;
			contr_send(31, 12);
		break;
//--------------------------------------------------Early------------------------------------------------------------
		case 33:
			if(i<64.0f)
				prog_data[early] = 0;
			else
				prog_data[early] = 1;
			contr_send(18, 7|(prog_data[early]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(74, 2, (uint8_t*)er, prog_data[early]);
			}
		break;
		case 34:
			prog_data[early_vol] = i*0.5f;
			contr_send(6, 0);
		break;
//---------------------------------------------------Moog Tape-------------------------------------------------------
		case 35:
			if(!tap_del_fl&&!tap_trem_fl)
			{
				if(tap_temp_global()&&!sys_para[tap_typ])
				{
					moog_time = tap_global/3.0f;
					contr_send(31, 13);
				}
			}
		break;
		case 36:
			if(i<64.0f)
				prog_data[vol_contr] = 1;
			else
				prog_data[vol_contr] = 0;
		break;
		case 37:
			prog_data[vol] = i;
			contr_send(7, 0);
		break;
		case 38:
			prog_data[cab2_vol] = i;
			contr_send(7, 3);
		break;
		case 39:
			if(i<64.0f)
				prog_data[gate] = 0;
			else
				prog_data[gate] = 1;
			contr_send(18, 11|(prog_data[gate]<<8));
			if(currentMenu->menuType()==MENU_MODULES)
			{
				DisplayTask->EfIcon(20, 0, (uint8_t*)gt, prog_data[gate]);
			}
		break;
		case 40:
			prog_data[gate_thr] = i;
			contr_send(29, 0);
		break;
		case 41:
			prog_data[hpf_v] = i;
			contr_send(4, 5);
		break;
		case 42:
			prog_data[lpf_v] = i;
			contr_send(4, 6);
		break;
		case 43:
			prog_data[pre_v] = i;
			contr_send(4, 7);
		break;
//------------------------------------------------------Preamp parameter--------------------------

		case 44:
			prog_data[pre_gain] = i;
			contr_send(27, 0);
		break;
		case 45:
			prog_data[pre_volume] = i;
			contr_send(27, 1);
		break;
		case 46:
			prog_data[pre_low] = i;
			contr_send(27, 3);
		break;
		case 47:
			prog_data[pre_mid] = i;
			contr_send(27, 4);
		break;
		case 48:
			prog_data[pre_high] = i;
			contr_send(27, 5);
		break;
//-------------------------------------------------------------Eq band-------------------------------
		case 49:
			prog_data[eq1] = i*0.25f;
			contr_send(4, 0);
		break;
		case 50:
			prog_data[eq2] = i*0.25f;
			contr_send(4, 1);
		break;
		case 51:
			prog_data[eq3] = i*0.25f;
			contr_send(4, 2);
		break;
		case 52:
			prog_data[eq4] = i*0.25f;
			contr_send(4, 3);
		break;
		case 53:
			prog_data[eq5] = i*0.25f;
			contr_send(4, 4);
		break;
//-------------------------------------------------------------Reverb type---------------------------
		case 54:
			if(i<7)
				prog_data[rev_t] = i;
			contr_send(2, 1);
		break;
	}
}
//------------------------------------------------------------------------------
TCCTask::TCCTask() :
		TTask()
{
}
extern uint8_t tuner_use;
void TCCTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 8, 0);
	Delay(1000);
	while(1)
	{
		sem->Take(portMAX_DELAY);
		if(mid_fl&&!tuner_use&&pc_mute_fl)
		{
			if(control[0])
				if(midi_b[1]==control[0]-5)
					con_ch(0, midi_b[2]);
			if(control[4])
				if(midi_b[1]==control[4]-5)
					con_ch(1, midi_b[2]);
			if(control[8])
				if(midi_b[1]==control[8]-5)
					con_ch(2, midi_b[2]);
			if(control[12])
				if(midi_b[1]==control[12]-5)
					con_ch(3, midi_b[2]);
			if(control[16])
				if(midi_b[1]==control[16]-5)
					con_ch(4, midi_b[2]);
			if(control[20])
				if(midi_b[1]==control[20]-5)
					con_ch(5, midi_b[2]);
			if(control[24])
				if(midi_b[1]==control[24]-5)
					con_ch(6, midi_b[2]);
			if(control[28])
				if(midi_b[1]==control[28]-5)
					con_ch(7, midi_b[2]);
			if(control[32])
				if(midi_b[1]==control[32]-5)
					con_ch(8, midi_b[2]);
			if(control[36])
				if(midi_b[1]==control[36]-5)
					con_ch(9, midi_b[2]);
			if(control[40])
				if(midi_b[1]==control[40]-5)
					con_ch(10, midi_b[2]);
			if(control[44])
				if(midi_b[1]==control[44]-5)
					con_ch(11, midi_b[2]);
			if(control[48])
				if(midi_b[1]==control[48]-5)
					con_ch(12, midi_b[2]);
			if(control[52])
				if(midi_b[1]==control[52]-5)
					con_ch(13, midi_b[2]);
			if(control[56])
				if(midi_b[1]==control[56]-5)
					con_ch(14, midi_b[2]);
			if(control[60])
				if(midi_b[1]==control[60]-5)
					con_ch(15, midi_b[2]);
			if(control[64])
				if(midi_b[1]==control[64]-5)
					con_ch(16, midi_b[2]);
			if(control[68])
				if(midi_b[1]==control[68]-5)
					con_ch(17, midi_b[2]);
			if(control[72])
				if(midi_b[1]==control[72]-5)
					con_ch(18, midi_b[2]);
			if(control[76])
				if(midi_b[1]==control[76]-5)
					con_ch(19, midi_b[2]);
			if(control[80])
				if(midi_b[1]==control[80]-5)
					con_ch(20, midi_b[2]);
			if(control[84])
				if(midi_b[1]==control[84]-5)
					con_ch(21, midi_b[2]);
			if(control[88])
				if(midi_b[1]==control[88]-5)
					con_ch(22, midi_b[2]);
			if(control[92])
				if(midi_b[1]==control[92]-5)
					con_ch(23, midi_b[2]);
			if(control[96])
				if(midi_b[1]==control[96]-5)
					con_ch(24, midi_b[2]);
			if(control[100])
				if(midi_b[1]==control[100]-5)
					con_ch(25, midi_b[2]);
			if(control[104])
				if(midi_b[1]==control[104]-5)
					con_ch(26, midi_b[2]);
			if(control[108])
				if(midi_b[1]==control[108]-5)
					con_ch(27, midi_b[2]);
			if(control[112])
				if(midi_b[1]==control[112]-5)
					con_ch(28, midi_b[2]);
			if(control[116])
				if(midi_b[1]==control[116]-5)
					con_ch(29, midi_b[2]);
			if(control[120])
				if(midi_b[1]==control[120]-5)
					con_ch(30, midi_b[2]);
			if(control[124])
				if(midi_b[1]==control[124]-5)
					con_ch(31, midi_b[2]);
			mid_fl = 0;
		}
		if(ext_fl&&!tuner_use&&pc_mute_fl)
		{
			if(control[0]==ext_sourc)
				con_ch(0, ext_data);
			if(control[4]==ext_sourc)
				con_ch(1, ext_data);
			if(control[8]==ext_sourc)
				con_ch(2, ext_data);
			if(control[12]==ext_sourc)
				con_ch(3, ext_data);
			if(control[16]==ext_sourc)
				con_ch(4, ext_data);
			if(control[20]==ext_sourc)
				con_ch(5, ext_data);
			if(control[24]==ext_sourc)
				con_ch(6, ext_data);
			if(control[28]==ext_sourc)
				con_ch(7, ext_data);
			if(control[32]==ext_sourc)
				con_ch(8, ext_data);
			if(control[36]==ext_sourc)
				con_ch(9, ext_data);
			if(control[40]==ext_sourc)
				con_ch(10, ext_data);
			if(control[44]==ext_sourc)
				con_ch(11, ext_data);
			if(control[48]==ext_sourc)
				con_ch(12, ext_data);
			if(control[52]==ext_sourc)
				con_ch(13, ext_data);
			if(control[56]==ext_sourc)
				con_ch(14, ext_data);
			if(control[60]==ext_sourc)
				con_ch(15, ext_data);
			if(control[64]==ext_sourc)
				con_ch(16, ext_data);
			if(control[68]==ext_sourc)
				con_ch(17, ext_data);
			if(control[72]==ext_sourc)
				con_ch(18, ext_data);
			if(control[76]==ext_sourc)
				con_ch(19, ext_data);
			if(control[80]==ext_sourc)
				con_ch(20, ext_data);
			if(control[84]==ext_sourc)
				con_ch(21, ext_data);
			if(control[88]==ext_sourc)
				con_ch(22, ext_data);
			if(control[92]==ext_sourc)
				con_ch(23, ext_data);
			if(control[96]==ext_sourc)
				con_ch(24, ext_data);
			if(control[100]==ext_sourc)
				con_ch(25, ext_data);
			if(control[104]==ext_sourc)
				con_ch(26, ext_data);
			if(control[108]==ext_sourc)
				con_ch(27, ext_data);
			if(control[112]==ext_sourc)
				con_ch(28, ext_data);
			if(control[116]==ext_sourc)
				con_ch(29, ext_data);
			if(control[120]==ext_sourc)
				con_ch(30, ext_data);
			if(control[124]==ext_sourc)
				con_ch(31, ext_data);
			ext_fl = 0;
		}
	}
}

