#include "appdefs.h"
#include "eepr.h"
#include "display.h"
#include "cc.h"
#include "cs.h"
#include "enc.h"
#include "display.h"

#include "BF706_send.h"
#include "midi_send.h"

#include "allFonts.h"
#include "gui_task.h"
#include "modulesmenu.h"

#include "controllers.h"
#include "preset.h"

TCCTask *CCTask;



volatile uint8_t midi_b[3];
uint8_t mid_fl;
uint8_t ext_fl;
uint8_t ext_f_fl;
uint8_t ext_data;
uint8_t ext_sourc = 255;
volatile uint8_t tap_del_fl;
volatile uint8_t tap_trem_fl;
volatile uint8_t tap_moog_fl;

void controllerSetData(uint8_t adr, uint8_t data)
{
	float i = currentPreset.controller[adr].minVal + (currentPreset.controller[adr].maxVal - currentPreset.controller[adr].minVal) * data / 127.0f;

	switch(currentPreset.controller[adr].dst)
	{
		case 0:
			if(i < 64.0f)
				presetData[ENABLE_PREAMP] = 0;
			else
				presetData[ENABLE_PREAMP] = 1;
			contr_send(18, 10 | (presetData[ENABLE_PREAMP] << 8));
		break;
//-------------------------------------------------PA-------------------------------------------------
		case 1:
			if(i < 64.0f)
				presetData[amp] = 0;
			else
				presetData[amp] = 1;
			contr_send(18, 0 | (presetData[amp] << 8));
		break;
		case 2:
			presetData[am_v] = i;/*DisplayTask->ParamIndic(58,0,prog_data[am_v]);*/
			contr_send(11, 0);
		break;
		case 3:
			presetData[am_out] = i;/*DisplayTask->ParamIndic(58,2,prog_data[am_out]);*/
			contr_send(11, 2);
		break;
//-------------------------------------------------CAB SIM--------------------------------------------
		case 4:
			if(i < 64.0f)
				presetData[cab] = 0;
			else
				presetData[cab] = 1;
			contr_send(18, 1 | (presetData[cab] << 8));
		break;
//-------------------------------------------------EQ-------------------------------------------------
		case 5:
			if(i < 64.0f)
				presetData[eq] = 0;
			else
				presetData[eq] = 1;
			contr_send(18, 2 | (presetData[eq] << 8));
		break;
//-------------------------------------------------Delay----------------------------------------------
		case 6:
			if(i < 64.0f)
				presetData[delay] = 0;
			else
				presetData[delay] = 1;
			contr_send(18, 3 | (presetData[delay] << 8));
		break;
		case 7:
			presetData[d_vol] = i;/*DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);*/
			contr_send(3, 0);
		break;
		case 8:
			presetData[d_fed] = i;/*DisplayTask->ParamIndic(53,2,prog_data[d_fed]);*/
			contr_send(3, 2);
		break;
		case 9:
			if(tap_temp_global() && !sys_para[TAP_TYPE])
			{
				delay_time = tap_global / 3.0f / tap_time_coefs[presetData[d_tap_t]];
				if(delay_time < 2731)
				{
					if(!sys_para[TIME_FORMAT])
					{
						contr_send(3, 1);
						if(current_menu_type == MENU_DELAY)
							DisplayTask->DelayTimeInd(53, 1, delay_time);
						if(current_menu_type == MENU_TAP_DELAY)
							DisplayTask->DelayTimeInd(53, 0, delay_time);
					}
					else
					{
						uint8_t temp = 0;
						if(delay_time < 2728 && delay_time > 249)
						{
							while(delay_time < bpm_time[temp++]);

							delay_time = bpm_time[temp];
							contr_send(3, 1);

							if(current_menu_type == MENU_DELAY)
							{
								DisplayTask->ParamIndicNum(53, 1, 60000 / delay_time);
								DisplayTask->StringOut(90, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
							}

							if(current_menu_type == MENU_TAP_DELAY)
							{
								DisplayTask->ParamIndicNum(53, 0, 60000 / delay_time);
								DisplayTask->StringOut(90, 0, TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
							}
						}
					}
				}
				if(tap_trem_fl)
				{
					trem_time = tap_global / 3.0f / tap_time_coefs[presetData[t_tap_t]];
					if(trem_time > 2730)
						trem_time = 2730;
					contr_send(10, 5);
				}
				if(tap_moog_fl)
				{
					moog_time = tap_global / 3.0f;
					contr_send(31, 13);
				}
			}
		break;
//-------------------------------------------------Phazer---------------------------------------------
		case 10:
			if(i < 64.0f)
				presetData[phas] = 0;
			else
				presetData[phas] = 1;
			contr_send(18, 4 | (presetData[phas] << 8));

		break;
		case 11:
			presetData[phaser_vol] = i;
			contr_send(5, 0);
		break;
		case 12:
			presetData[phaser_rate] = i;
			contr_send(5, 1);
		break;
//-------------------------------------------------Flanger--------------------------------------------
		case 13:
			if(i < 64.0f)
				presetData[fl] = 0;
			else
				presetData[fl] = 1;
			contr_send(18, 5 | (presetData[fl] << 8));

		break;
		case 14:
			presetData[fl_v] = i;
			contr_send(8, 0);
		break;
		case 15:
			presetData[fl_r] = i;
			contr_send(8, 2);
		break;
//-------------------------------------------------Chorus---------------------------------------------
		case 16:
			if(i < 64.0f)
				presetData[chor] = 0;
			else
				presetData[chor] = 1;
			contr_send(18, 6 | (presetData[chor] << 8));

		break;
		case 17:
			presetData[chor_volum] = i;/*DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);*/
			contr_send(9, 0);
		break;
		case 18:
			presetData[chor_rat] = i;/*DisplayTask->ParamIndic(53,1,prog_data[chor_volum + 1]);*/
			contr_send(9, 1);
		break;
//-------------------------------------------------Reverb---------------------------------------------
		case 19:
			if(i < 64.0f)
				presetData[reve] = 0;
			else
				presetData[reve] = 1;
			contr_send(18, 8 | (presetData[reve] << 8));

		break;
		case 20:
			presetData[r_vol] = i;
			contr_send(2, 0);
		break;
		case 21:
			presetData[r_time] = i;
			contr_send(2, 2);
		break;
//-------------------------------------------------Tremolo--------------------------------------------
		case 22:
			if(i < 64.0f)
				presetData[trem] = 0;
			else
				presetData[trem] = 1;
			contr_send(18, 9 | (presetData[trem] << 8));

		break;
		case 23:
			presetData[tr_vol] = i;
			contr_send(10, 0);
		break;
		case 24:
			presetData[tr_rate] = i;
			contr_send(10, 1);
		break;
		case 26:
			if(!tap_del_fl)
			{
				if(tap_temp_global() && !sys_para[TAP_TYPE])
				{
					trem_time = tap_global / 3.0f / tap_time_coefs[presetData[t_tap_t]];
					if(trem_time < 2731)
						contr_send(10, 5);
					if(tap_moog_fl)
					{
						moog_time = tap_global / 3.0f;
						contr_send(31, 13);
					}
				}
			}
		break;
//---------------------------------------------------------------------------------------------------
		case 25:
			if(!presetData[vol_contr])
				contr_send(19, i);
		break;
//-------------------------------------------------Compressor--------------------------------------------
		case 27:
			if(i < 64.0f)
				presetData[compr] = 0;
			else
				presetData[compr] = 1;
			contr_send(18, 12 | (presetData[compr] << 8));

		break;
		case 28:
			presetData[comp_thr] = i;
			contr_send(30, 0);
		break;
		case 29:
			presetData[comp_v] = i;
			contr_send(30, 2);
		break;
//-------------------------------------------------Moog filter-------------------------------------------
		case 30:
			if(i < 64.0f)
				presetData[moog] = 0;
			else
				presetData[moog] = 1;
			contr_send(18, 13 | (presetData[moog] << 8));

		break;
		case 31:
			presetData[mog_rate] = i;
			contr_send(31, 3);
		break;
		case 32:
			presetData[mog_ext] = i;
			contr_send(31, 12);
		break;
//--------------------------------------------------Early------------------------------------------------------------
		case 33:
			if(i < 64.0f)
				presetData[early] = 0;
			else
				presetData[early] = 1;
			contr_send(18, 7 | (presetData[early] << 8));

		break;
		case 34:
			presetData[early_vol] = i * 0.5f;
			contr_send(6, 0);
		break;
//---------------------------------------------------Moog Tape-------------------------------------------------------
		case 35:
			if(!tap_del_fl && !tap_trem_fl)
			{
				if(tap_temp_global() && !sys_para[TAP_TYPE])
				{
					moog_time = tap_global / 3.0f;
					contr_send(31, 13);
				}
			}
		break;
		case 36:
			if(i < 64.0f)
				presetData[vol_contr] = 1;
			else
				presetData[vol_contr] = 0;
		break;
		case 37:
			presetData[vol] = i;
			contr_send(7, 0);
		break;
		case 38:
			presetData[cab2_vol] = i;
			contr_send(7, 3);
		break;
		case 39:
			if(i < 64.0f)
				presetData[gate] = 0;
			else
				presetData[gate] = 1;
			contr_send(18, 11 | (presetData[gate] << 8));

		break;
		case 40:
			presetData[gate_thr] = i;
			contr_send(29, 0);
		break;
		case 41:
			presetData[hpf_v] = i;
			contr_send(4, 5);
		break;
		case 42:
			presetData[lpf_v] = i;
			contr_send(4, 6);
		break;
		case 43:
			presetData[pre_v] = i;
			contr_send(4, 7);
		break;
//------------------------------------------------------Preamp parameter--------------------------

		case 44:
			presetData[pre_gain] = i;
			contr_send(27, 0);
		break;
		case 45:
			presetData[pre_volume] = i;
			contr_send(27, 1);
		break;
		case 46:
			presetData[pre_low] = i;
			contr_send(27, 3);
		break;
		case 47:
			presetData[pre_mid] = i;
			contr_send(27, 4);
		break;
		case 48:
			presetData[pre_high] = i;
			contr_send(27, 5);
		break;
//-------------------------------------------------------------Eq band-------------------------------
		case 49:
			presetData[eq1] = i * 0.25f;
			contr_send(4, 0);
		break;
		case 50:
			presetData[eq2] = i * 0.25f;
			contr_send(4, 1);
		break;
		case 51:
			presetData[eq3] = i * 0.25f;
			contr_send(4, 2);
		break;
		case 52:
			presetData[eq4] = i * 0.25f;
			contr_send(4, 3);
		break;
		case 53:
			presetData[eq5] = i * 0.25f;
			contr_send(4, 4);
		break;
//-------------------------------------------------------------Reverb type---------------------------
		case 54:
			if(i < 7)
				presetData[rev_t] = i;
			contr_send(2, 1);
		break;
	}

	currentMenu->refresh();
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
		if(mid_fl && !tuner_use && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(currentPreset.controller[i].src)
					if(midi_b[1] == currentPreset.controller[0].src - 5)
						controllerSetData(i, midi_b[2]);
			}
			mid_fl = 0;
		}

		if(ext_fl && !tuner_use && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(currentPreset.controller[i].src == ext_sourc)
					controllerSetData(0, ext_data);
			}
			ext_fl = 0;
		}
	}
}

