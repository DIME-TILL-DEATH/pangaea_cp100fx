#include "gui_task.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "filt.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "fonts/icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"
#include "spectrum.h"
#include "vdt/vdt.h"

#include "modules.h"

#include "menus/abstractmenu.h"


const uint8_t ef_list       [][8] ={"Cab Sim","Volume ","    Eq ","Phaser ","Chorus "," Delay "," Early ","Reverb "," Name  "," Write ","Write ?"};
const uint8_t del_list      [][8] ={"Mix","Time","F_Back","LPF","HPF","D_Pan","D2 Vol","D2 Pan","D->D2","D_Mod","M_Rate","Direct"};
const uint8_t del_tim_l     [][5] ={"Time","TAP","Tail"};
const uint8_t tap_tim       [][6] ={"1/1  ","1/1.5","1/2  ","1/3  ","1/4  ","2/1  "};
const float   tap_tim_v     [6]   ={1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};
const uint8_t s_t_c_list    [][7] ={"  No  ","Return","   Yes"};
const uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};
const uint8_t cab_list      [][8] ={"Pan","Browser","Volume"};
const uint8_t cab_out_list  [][6] ={"1 L+R","1R AP","2 L+R","1R A ","1R P "," 1 R "};
const uint8_t cab_list_menu [][9] ={"Cabinet1","Cabinet2"};

const uint8_t mas_eq_list   [][9] ={"Low","Mid","Mid Freq","High"};
const uint8_t switch_mod    [][7] ={"Single","Double"};
const uint8_t switch_slow   [][10]={"OnRelease","OnHold"};
const uint8_t out_pc_list   [][8] ={"MIDI IN","  MAP  ","  SET  "};

const uint8_t att_db [][4] = {" +4"," +3"," +2"," +1","  0"," -1"," -2"," -3"," -4"," -5"," -6"," -7"," -8"," -9","-10","-11",
	"-12","-13","-14","-15","-16","-17","-18","-19","-20","-21","-23","-24","-27","-30","-34","-35","-36","-37",
	"-38","-40","-41","-42","-43","-44","-45","-46","-47","-48","-49","-50","-51","-52","-53","-54","-55","-56",
	"-57","-58","-59","-60"};

const uint8_t pres_num_pos[4] = {58,46,37,28};


const uint8_t exit_[]="Exit";
const uint8_t volum[]="Preset Level";
const uint8_t eq_on[]="Eq on";
const uint8_t eq_off[]="Eq off";
const uint8_t write_[]="Save ?";
const uint8_t write__[]=" Save";
const uint8_t name_[]="  Name";
const uint8_t edit_[]="Edit";
const uint8_t push[] ="Push to write";
const uint8_t tap[]="Edit -> Tap";
const uint8_t s_to_ch[]="      Save?";
const uint8_t tremol[]="Tremolo";
const uint8_t controll[]="Controllers";
const uint8_t com[]     ="Compressor";
const uint8_t cc[]="CC#";
const uint8_t com_val[] = "Value";
const uint8_t thr_by[] = "TRUE BYPASS";
const uint8_t cab_s [] = "Cabinet simulator";
const uint8_t calib []   = "Calibration";
const uint8_t set_min [] = "Set to min";
const uint8_t set_max [] = "Set to max";
const uint8_t ok [] = "Ok";
const uint8_t sd_nr [] = "MicroSD is not ready";
const uint8_t imp_dir_n [] = "There is no directory";
const uint8_t imp_dir_no [ ]= "IMPULSE";
const uint8_t sd_lo [] = "MicroSD is loading..";
const uint8_t dw1[][6] = {"D<--W","D-->W","D<->W"};
const uint8_t decib[] = "dB";

const uint8_t master_vo[][14] = {"Master Volume","Phones Volume"};
const uint8_t master_eq_of[] = {"Master EQ Bypass"};
const uint8_t master_eq_on[] = {"EQ on/off press EDIT"};
const uint8_t mode_[] = "Mode";

uint16_t midi_clk_send;
uint8_t num_men = 0;
uint8_t num_men_temp = 0;
volatile uint8_t prog2;
uint8_t tim5_fl2;
volatile uint8_t preset_edited = 0;
volatile uint8_t par_num = 0;
volatile uint8_t par_num1 = 0;
volatile uint8_t par_num2 = 0;
volatile uint8_t prog_old;
volatile uint8_t calib_fl = 0;
gui_menu_type current_menu;
uint8_t copy_temp = 0;
extern uint8_t cab_type;

extern uint8_t k_up;
extern uint8_t k_down;
extern uint8_t k_att;

extern uint8_t k_master_eq;
extern uint8_t name_run_fl;
extern uint8_t name_temp_po;
extern uint32_t name_temp_po_time;
extern volatile uint8_t prog_sym_cur;
extern volatile uint8_t master_volume_controller;

volatile uint8_t encoder_knob_selected;
volatile uint16_t count_enc = 0xf000;
volatile uint8_t write_fl = 0;
volatile uint8_t cab_n_fl1 = 0;
volatile uint8_t cab_n_fl2 = 0;
volatile uint8_t tim_fl = 0;

uint16_t att_temp;
uint32_t send_buf;
uint32_t del_time = 48000;
uint16_t trem_time;
uint16_t moog_time;
uint32_t del_ind;
uint32_t del_ind1;
uint32_t del_ind2;
uint32_t tap_temp;
uint32_t tap_temp1;
uint32_t tap_temp2;
uint32_t tap_global;
uint8_t tuner_use;
uint8_t key_shift = 0;
uint8_t temp_sys;
uint8_t control_destin;
volatile int8_t temp;
volatile uint8_t sub_men = 0;
char str_temp[22];
volatile uint8_t nam_sym_temp;
volatile tr_t trem_temp;

//inline uint8_t tap_temp_global(void);


void str_pres_init(char* adr , uint16_t val)
{
	if(val < 10)ksprintf( adr , "0%d" , val);
	else ksprintf( adr , "%d" , val);
}

void tim5_start(uint8_t val)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_SetCounter(TIM4, 0xffff);
	tim5_fl = val;
}

void t_restart(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_SetCounter(TIM4, 0);
	tim5_fl = 1;
	CSTask->Give();
}

void clean_flag (void)
{
	k_up = k_down = k_sys = k_att = k_master = k_master_eq = encoder_knob_pressed =
	encoder_state_updated = k_tuner = 0;
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
}

extern volatile uint8_t pc_mute_fl;
void prog_ch(void)
{
	gui_send(32,prog);
	eepr_read_prog_data(prog);
	eepr_read_prog(prog);
	pc_mute_fl = 0;

    MidiSendTask->prog_ch_midi_start();
    MidiSendTask->Give();

    for(uint8_t i = 0 ; i < 3 ; i++)
    {
    	contr_kn[i] = prog_data[fo1 + i];
    	contr_kn1[i] = prog_data[fo11 + i];
    }
	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);

	if(sys_para[sav_exp])adc_proc();

    pc_mute_fl = 1;
    tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
    for(uint8_t i = 0 ; i < 32 ; i++)
    {
  	  if(control[i*4])
  	  {
  		  if(control[i*4 + 1] == 9)tap_del_fl = 1;
  		  if(control[i*4 + 1] == 26)tap_trem_fl = 1;
  		  if(control[i*4 + 1] == 35)tap_moog_fl = 1;
  	  }
    }

}

uint8_t d_w_init(uint8_t val)
{
	  uint8_t b;
	  if(val < 63)
		  {
		  	  b = 0;
		  	  val = 63 - val;
		  }
	  else {
		  b = 1;
		  val = val - 63;
		  if(!val)b = 2;
	  }
	  return b;
}
void write_sys(void)
{
   	 FATFS fs;
   	 FIL file;
   	 UINT f_size;
   	 f_mount ( &fs , "1:",  1);
   	 f_open(&file, "1:system.pan" , FA_WRITE ) ;
   	 f_write(&file, sys_para , 512 , &f_size);
   	 f_close(&file);
   	 f_mount(0, "1:", 0);
}

volatile uint16_t dsaf = 0x8000;
//---------------------------------------------------------GUI----------------------------------------------------
void GUI_return_to_main_menu()
{
	current_menu = MENU_MAIN;
	encoder_knob_selected = 0;
	DisplayTask->Menu_init();
	tim5_start(0);
	clean_flag();
}

void gui(void)
{
	currentMenu->task();

	if(encoder_knob_pressed) currentMenu->encoderPressed();
	if(encoder_state_updated)
	{
		if(encoder_state == ENC_COUNTERCLOCKWISE_STEP)
		{
			currentMenu->encoderCounterClockwise();
		}

		if(encoder_state == ENC_CLOCKWISE_STEP)
		{
			currentMenu->encoderClockwise();
		}
	}
	if(k_up) currentMenu->keyUp();
	if(k_down) currentMenu->keyDown();
	if(k_att) currentMenu->key1();
	if(k_master) currentMenu->key2();
	if(k_master_eq) currentMenu->key3();
	if(k_sys) currentMenu->key4();
	if(k_tuner) currentMenu->key5();

//		clean_flag();
  switch (current_menu){

//---------------------------------------------------------------------Menu Save change-----------------------------------------------------
      case MENU_PRESET_SAVE:
    	if(tim5_fl == 0)DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num*7);
        else DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,3,(uint8_t*)s_t_c_list + par_num*7);
        if(encoder_state_updated == 1)
		{
			if(encoder_state == 1)
			{
				if(par_num > 0)
				{
				DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num--*7);
				DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num*7);
				tim5_start(0);
				}
			}

			if(encoder_state == 2)
			{
				if(par_num < 2)
				{
					DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num++*7);
					DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num*7);
					tim5_start(0);
				}
			}

			clean_flag();
		}
        if(encoder_knob_pressed == 1)
        {
        	switch (par_num){
        	case 2:
				current_menu = MENU_PRESET_WRITE;
				eepr_read_imya(prog1);
				DisplayTask->Main_scr();
				DisplayTask->Prog_ind(prog1);
				DisplayTask->StringOut(40,3,TDisplayTask::fntSystem,0,(uint8_t*)write_);
				break;
        	case 0:
                 vol_fl = 0;

                 num_men = 0;
                 current_menu = MENU_MAIN; // edit_modules_fl = 0;
                 prog_ch();
                 prog_cur = 0;
                 DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
                 eepr_read_imya(prog1);
                 DisplayTask->Main_scr();
                 DisplayTask->Prog_ind(prog1);
       	         if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
       		     if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
       		     if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
        	 break;
			 case 1:
                 current_menu = MENU_MAIN;
                 DisplayTask->Menu_init();
        	 break;
        	}
        	tim5_start(0);
        	clean_flag();
        }
        if(k_up == 1)
        {
            vol_fl = 0;
            num_men = 0;
            current_menu = MENU_MAIN; //edit_modules_fl = 0;
            prog_ch();
            prog_cur = 0;

            eepr_read_imya(prog1);
            DisplayTask->Main_scr();
            DisplayTask->Prog_ind(prog1);
  	        if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
  		    if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
  		    if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
            tim5_start(0);
            clean_flag();
        }
        break;
//--------------------------------------------------------------------------Menu Volume------------------------------------------------------
        case MENU_VOLUME:
		if(encoder_knob_selected == 0)
		{
			if(par_num)
			{
				if(tim5_fl)
				{
					DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_out_strings + 7);
					vol_fl = 1;
				}
				else {
					DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
					vol_fl = 0;
				}
			}
			else {
				if(tim5_fl)DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
				else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
			}
		}

          if(encoder_state_updated == 1)
            {
              if(encoder_state == 1)
                {
            	  if(!encoder_knob_selected)
            	  {
            		  if(par_num)
            		  {
            			  DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
            			  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
            			  par_num--;
            		  }
            	  }
            	  else {
            		  if(par_num)
            		  {
            			  if(prog_data[pres_lev] > 0)
            			  {
            				  prog_data[pres_lev] = enc_speed_dec(prog_data[pres_lev],0);
            				  DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
            				  vol_vol = prog_data[pres_lev];
            				  gui_send(0,prog_data[pres_lev]);
            			  }
            		  }
            		  else {
            			  if(prog_data[vol_contr])
            			  {
            				  prog_data[vol_contr] = 0;
            				  DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"On ");
            			  }
            		  }
            	  }
                }
              if(encoder_state == 2)
                {
            	  if(!encoder_knob_selected)
            	  {
            		  if(!par_num)
            		  {
            			  DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
            			  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_out_strings + 7);
            			  par_num++;
            		  }
            	  }
            	  else {
            		  if(par_num)
            		  {
            			  if(prog_data[pres_lev] < 127)
            			  {
            				  prog_data[pres_lev] = enc_speed_inc(prog_data[pres_lev],127);
            				  DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
            				  vol_vol = prog_data[pres_lev];
            				  gui_send(0,prog_data[pres_lev]);
            			  }
            		  }
            		  else {
            			  if(!prog_data[vol_contr])
            			  {
            				  prog_data[vol_contr] = 1;
            				  DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Off");
            			  }
            		  }
                  }
                }
              clean_flag();
            }
          if(encoder_knob_pressed == 1)
            {
                  if(!encoder_knob_selected)
                  {
                    encoder_knob_selected = 1;
                    if(par_num)DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_out_strings + 7);
                    else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
                  }
                  else {
                    if(par_num)DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
                    else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
                    encoder_knob_selected = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
          if((k_att == 1) || (k_sys == 1) || (k_tuner == 1) || (k_master_eq == 1))
          {
        	  if(k_tuner == 1)num_men = 13;
              current_menu = MENU_MAIN;
              encoder_knob_selected = 0;
              vol_fl = 0;
              tim5_start(0);
          	  CSTask->Give();
          	  break;
          }
          if((k_up == 1) || (k_master == 1))
            {
        	  sub_men = 0;
              current_menu = MENU_MAIN;
              encoder_knob_selected = 0;
              vol_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//-------------------------------------------------------------------------Eq Band-----------------------------
//        case MENU_EQ_BAND:
//        	volatile int8_t a;
//            if(encoder_knob_selected == 0)
//              {
//                if(tim5_fl == 1)DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num*2);
//                else DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num*2);
//              }
//            if(encoder_state_updated == 1)
//            {
//            	if(encoder_state == 1)
//            	{
//            		if(!encoder_knob_selected)
//            		{
//            			if(par_num > 0)
//            			{
//            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num-- * 2);
//            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num * 2);
//            				tim5_start(0);
//            			}
//            		}
//            		else {
//            			a = prog_data[f1 + eq_num + par_num*5];
//            			if(par_num)
//            			{
//            				if(a > -60)
//            				{
//            					a = enc_speed_dec(a,-60);
//            					prog_data[f1 + eq_num + par_num*5] = a;
//            					DisplayTask->EqPar( 40 , 1 , a,par_num,eq_num);
//            					gui_send(22 , eq_num + par_num*5);
//            				}
//            			}
//            			else {
//            				if(a > -100)
//            				{
//            					a = enc_speed_dec(a,-100);
//            					prog_data[f1 + eq_num + par_num*5] = a;
//            					DisplayTask->EqPar( 40 , 0 , a,par_num,eq_num);
//            					DisplayTask->StringOut(80,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
//            					gui_send(22 , eq_num + par_num*5);
//            				}
//            			}
//            		}
//            	}
//            	if(encoder_state == 2)
//            	{
//            		if(!encoder_knob_selected)
//            		{
//            			if(par_num < 1)
//            			{
//            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num++ * 2);
//            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num * 2);
//            				tim5_start(0);
//            			}
//            		}
//            		else {
//            			a = prog_data[f1 + eq_num + par_num*5];
//            			if(par_num)
//            			{
//            				if(a < 120)
//            				{
//            					a = enc_speed_inc(a,120);
//            					prog_data[f1 + eq_num + par_num*5] = a;
//            					DisplayTask->EqPar( 40 , 1 , a,par_num,eq_num);
//            					gui_send(22 , eq_num + par_num*5);
//            				}
//            			}
//            			else {
//            				if(a < 100)
//            				{
//            					a = enc_speed_inc(a,100);
//            					prog_data[f1 + eq_num + par_num*5] = a;
//            					DisplayTask->EqPar( 40 , 0 , a,par_num,eq_num);
//            					DisplayTask->StringOut(80,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
//            					gui_send(22 , eq_num + par_num*5);
//            				}
//            			}
//            		}
//            	}
//                clean_flag();
//            }
//            if(encoder_knob_pressed)
//            {
//                if(encoder_knob_selected == 0)
//                {
//                  encoder_knob_selected = 1;
//                  DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num*2);
//                }
//                else {
//                	DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num*2);
//                    encoder_knob_selected = 0;
//                }
//                tim5_start(0);
//                clean_flag();
//            }
//            if(k_master_eq)
//            {
//            	prog_data[f1 + eq_num] = 0;
//            	prog_data[f1 + eq_num + 5] = 0;
//				DisplayTask->EqPar( 40 , 0 , prog_data[f1 + eq_num] , 0 , eq_num);
//				DisplayTask->StringOut(80,0,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
//				DisplayTask->EqPar( 40 , 1 , prog_data[f1 + eq_num + 5] , 1 , eq_num);
//				gui_send(22 , eq_num);
//				gui_send(22 , eq_num + 5);
//		        clean_flag();
//            }
//            if(k_up == 1)
//              {
//            	encoder_knob_selected = 0;
//                current_menu = MENU_EQ;
//                DisplayTask->EqInit();
//                DisplayTask->Icon_Strel(ICON_EQ, STRELKA_DOWN);
//                tim5_start(0);
//                clean_flag();
//              }
//        	break;

//----------------------------------------------------------------------Menu Cab Sim-----------------------------------------------
                  case MENU_CABSIM:
                    if(encoder_knob_selected == 0)
                      {
                        if(tim5_fl == 1)DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,2,(uint8_t*)cab_list + par_num*8);
                        else DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,0,(uint8_t*)cab_list + par_num*8);
                      }
                    else {
                    	if(tim5_fl == 1)
                    	{
                            if (TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)vol_fl = 0;
                            else vol_fl = 1;
                    	}
                    	else vol_fl = 1;
                    }
                  if(encoder_state_updated == 1)
                    {
                      if(encoder_state == 1)
                        {
                    	  if(encoder_knob_selected == 0)
                    	  {
                    		  uint8_t a;
                    		  if(!cab_type)a = 1;
                    		  else a = 0;
                    		  if(par_num > a)
                    		  {
                                  DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,0,(uint8_t*)cab_list + par_num-- * 8);
                                  DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,2,(uint8_t*)cab_list + par_num * 8);

                    		  }
                    	  }
                    	  else {
                    		  switch(par_num){
                    		  case 0:if(cab_type == 2)
                    			  {
                    			  	  if(!cab_num)
                    			  	  {
                    			  	   if(prog_data[cab1_pan] > 0)
                    			  	   {
                    			  		prog_data[cab1_pan] = enc_speed_dec(prog_data[cab1_pan],0);
                    			  	    DisplayTask->ParamIndicPan(72,1,prog_data[cab1_pan]);
                    			  		gui_send(7,1);
                    			  	   }
                    			  	  }
                    			  	  else {
                    			  		  if(prog_data[cab2_pan] > 0)
                    			  		  {
                    			  			  prog_data[cab2_pan] = enc_speed_dec(prog_data[cab2_pan],0);
                    			  			  DisplayTask->ParamIndicPan(72,1,prog_data[cab2_pan]);
                    			  			gui_send(7,4);
                    			  		  }
                    			  	  }
                    			  }break;
                    		  case 2:if(!cab_num)
                    			  {
                    			  	 if(prog_data[vol] > 0)
                    			  	 {
                    			  		 prog_data[vol] = enc_speed_dec(prog_data[vol],0);
                    			  		 DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                    			  		 vol_vol = prog_data[vol];
                    			  		 gui_send(7,0);
                    			  	 }
                    			  }
                    		  	  else {
                     			  	 if(prog_data[cab2_vol] > 0)
                     			  	 {
                     			  		 prog_data[cab2_vol] = enc_speed_dec(prog_data[cab2_vol],0);
                     			  		 DisplayTask->ParamIndicNum(42,3,prog_data[cab2_vol]);
                     			  		 vol_vol = prog_data[cab2_vol];
                     			  		 gui_send(7,3);
                     			  	 }
                    		  	  }break;
                    		  }
                    	  }
                          TIM_SetCounter(TIM6,0);
                          TIM_ClearFlag(TIM6,TIM_FLAG_Update);
                          TIM_Cmd(TIM6,ENABLE);
                          tim5_start(0);
                        }
                      if(encoder_state == 2)
                        {
                    	  if(encoder_knob_selected == 0)
                    	  {
                    		  if(par_num < 2)
                    		  {
                                  DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,0,(uint8_t*)cab_list + par_num++ * 8);
                                  DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,2,(uint8_t*)cab_list + par_num * 8);

                    		  }
                    	  }
                    	  else {
                    		  switch(par_num){
                    		  case 0:if(cab_type == 2)
                    			  {
                    			  	  if(!cab_num)
                    			  	  {
                    			  		  if(prog_data[cab1_pan] < 126)
                    			  		  {
                    			  			  prog_data[cab1_pan] = enc_speed_inc(prog_data[cab1_pan],126);
                    			  			  DisplayTask->ParamIndicPan(72,1,prog_data[cab1_pan]);
                    			  			  gui_send(7,1);
                    			  		  }
                    			  	  }
                    			  	  else {
                    			  		  if(prog_data[cab2_pan] < 126)
                    			  		  {
                    			  			  prog_data[cab2_pan] = enc_speed_inc(prog_data[cab2_pan],126);
                    			  			  DisplayTask->ParamIndicPan(72,1,prog_data[cab2_pan]);
                    			  			gui_send(7,4);
                    			  		  }
                    			  	  }
                    			  }break;
                    		  case 2:if(!cab_num)
                    			  {
                    			     if(prog_data[vol] < 127)
                    			  	 {
                    			    	 prog_data[vol] = enc_speed_inc(prog_data[vol],127);
                    			    	 DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                    			  		 vol_vol = prog_data[vol];
                    			  		 gui_send(7,0);
                    			  	 }
                    			  }
                    		  	  else {
                    		  		 if(prog_data[cab2_vol] < 127)
                     			  	 {
                    		  			 prog_data[cab2_vol] = enc_speed_inc(prog_data[cab2_vol],127);
                    		  			 DisplayTask->ParamIndicNum(42,3,prog_data[cab2_vol]);
                     			  		 vol_vol = prog_data[cab2_vol];
                     			  		 gui_send(7,3);
                     			  	 }
                    		  	  }break;
                    		  }
                    	  }
                        }
                      tim5_start(0);
                    }
                  if(encoder_knob_pressed)
                    {
                      if(encoder_knob_selected == 0)
                        {
                    	  if(par_num == 1) {
                    		  extern uint8_t sd_init_fl;
                    		  if(sd_init_fl == 1)
                    			{
                    			  current_menu = MENU_CABBROWSER;
                    			  DisplayTask->Clear();
                    			  extern volatile uint8_t imp_dir_fl;
                    			  if(imp_dir_fl)
                    			  {
                    				  FSTask->SendCommand( TFsBrowser::bcCurrent );
                    				  FSTask->SendCommand( TFsBrowser::bcLoadImp );
                    			  }
                    			  else {
                        			  DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_n);
                        			  DisplayTask->StringOut(42,3,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_no);
                        			  CSTask->CS_del(1000);
                        			  DisplayTask->Clear();
                                      uint8_t a = 0;
                                      if(name_buf[0] > 41)a = name_buf[0] - 41;
                                      if(name_buf[0] == 0)DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                                      else DisplayTask->StringOut(1,0,TDisplayTask::fntSystem , 0 , (uint8_t*)name_buf + 1 + a);
                                      DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list);
                                      DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                                      DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                                      vol_vol = prog_data[vol];
                                      vol_fl = 1;
                                      par_num = 1;
                                      ind_poin = 500;
                                      current_menu = MENU_CABSIM;
                    			  }
                    			}
                    		  else {
                    			  DisplayTask->Clear();
                    			  if(!sd_init_fl)DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_nr);
                    			  else DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_lo);
                    			  CSTask->CS_del(1000);
                    			  DisplayTask->Clear();
                                  uint8_t a = 0;
                                  if(name_buf[0] > 41)a = name_buf[0] - 41;
                                  if(name_buf[0] == 0)DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                                  else DisplayTask->StringOut(1,0,TDisplayTask::fntSystem , 0 , (uint8_t*)name_buf + 1 + a);
                                  DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                                  DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                                  DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                                  vol_vol = prog_data[vol];
                                  vol_fl = 1;
                                  par_num = 1;
                                  ind_poin = 500;
                                  current_menu = MENU_CABSIM;
                    		  }
                    	  }
                    	  else
                    	  {
                              encoder_knob_selected = 1;
                              DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,2,(uint8_t*)cab_list + par_num * 8);
                    	  }
                        }
                      else {
                          DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,0,(uint8_t*)cab_list + par_num * 8);
                          vol_fl = 1;
                          encoder_knob_selected = 0;
                      }
                      tim5_start(0);
                    }
                  if(k_up == 1)
                  {
                	vol_vol = prog_data[pres_lev];
                	encoder_knob_selected = 0;
                	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
                	if(cab_type != 2)
                	{
                		current_menu = MENU_MAIN;
                		DisplayTask->Menu_init();
                	}
                	else {
                	   DisplayTask->Clear();
                 	   DisplayTask->StringOut(6,0,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu);
                 	   DisplayTask->StringOut(6,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu + 9);
                 	   current_menu = MENU_CABTYPE;
                 	   par_num = cab_num;
                	}
                    tim5_start(0);
                  }
                  clean_flag();
                  break;
//-----------------------------------------------------------------------Cab browser------------------------
                  case MENU_CABBROWSER:
                    if(encoder_state_updated == 1)
                      {
                        if(encoder_state == 1)
                          {
                      	  FSTask->SendCommand( TFsBrowser::bcUp );
                          }
                        if(encoder_state == 2)
                          {
                      	  FSTask->SendCommand( TFsBrowser::bcDown );
                          }
                      }
                    if(encoder_knob_pressed == 1)
                      {
                  	  FSTask->SendCommand( TFsBrowser::bcAction );
                  	  extern volatile uint8_t file_fl;
                  	  extern volatile uint8_t action_fl;
                  	  while(!action_fl);
                  	  action_fl = 0;
                  	  if(file_fl)
                  	  {
                  		  file_fl = 0;
                          DisplayTask->Clear();
                          if(!cab_num)
                          {
                        	 kgp_sdk_libc::memcpy(cab_data,preset_temp,12288);
                             if(cab_type != 2)kgp_sdk_libc::memcpy(cab_data1,preset_temp + 12288,12288);
                          	 send_cab_data(0,0,0);
                             for(uint8_t i = 0 ; i < 64 ; i++)name_buf[i] = name_buf_temp[i];
                             if(name_buf[0] == 0)
                             {
                             	vol_vol = prog_data[pres_lev];
                              	DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
                              	// return to main menu without clean flag
                              	encoder_knob_selected = 0;
                              	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
                        		current_menu = MENU_MAIN;
                        		DisplayTask->Menu_init();
                        		tim5_start(0);
                        		//
                        		prog_data[cab] = 0;
                        		gui_send(18, 0 | (prog_data[cab] << 8));

                             }
                             else name_run_fl = 1;
                             if(cab_type == 2)
                             {
                             	DisplayTask->StringOut(1,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list);
                              	DisplayTask->ParamIndicPan(72,1,prog_data[cab1_pan]);
                             }
                             DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                             vol_vol = prog_data[vol];
                             gui_send(7,0);
                             DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB1);
                          }
                          else {
                        	 kgp_sdk_libc::memcpy(cab_data1,preset_temp,12288);
                             send_cab_data1(0,0);
                             for(uint8_t i = 0 ; i < 64 ; i++)name_buf1[i] = name_buf_temp[i];
                             if(name_buf1[0] == 0)
                             {
                                name_run_fl = 0;
                              	DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                             }
                             else name_run_fl = 1;
                             if(cab_type == 2)
                             {
                             	DisplayTask->StringOut(1,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list);
                              	DisplayTask->ParamIndicPan(72,1,prog_data[cab2_pan]);
                             }
                             DisplayTask->ParamIndicNum(42,3,prog_data[cab2_vol]);
                             vol_vol = prog_data[cab2_vol];
                             gui_send(7,3);
                             DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB2);
                          }
                          DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                          DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                          current_menu = MENU_CABSIM;
                          par_num = 2;
                          ind_poin = 500;
                          vol_fl = 1;
                  	  }
                      }
                    if(k_up == 1)
                    {
                       DisplayTask->Clear();
                       if(!cab_num)
                       {
                          if(name_buf[0] == 0)
                          {
                        	gui_send(16,0);
                           	vol_vol = prog_data[pres_lev];
                           	DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
                           	//return_to_main_menu without clean flag
                           	encoder_knob_selected = 0;
                           	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
                        	current_menu = MENU_MAIN;
                        	DisplayTask->Menu_init();
                        	tim5_start(0);

                        	prog_data[cab] = 0;
                        	gui_send(18, 0 | (prog_data[cab] << 8));
                          }
                          else {
                           	gui_send(16,0);
                           	name_run_fl = 1;
                            if(cab_type == 2)
                            {
                               	DisplayTask->StringOut(1,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list);
                               	DisplayTask->ParamIndicPan(72,1,prog_data[cab1_pan]);
                            }
                            DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                            vol_vol = prog_data[vol];
                            gui_send(7,0);
                            DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB1);
                            DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                            DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                            current_menu = MENU_CABSIM;
                            par_num = 2;
                            ind_poin = 500;
                            vol_fl = 1;
                            tim5_start(0);
                          }
                       }
                       else {
                         gui_send(17,0);
                         if(name_buf1[0] == 0)
                         {
                            name_run_fl = 0;
                            DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                         }
                         else name_run_fl = 1;
                         if(cab_type == 2)
                         {
                           	DisplayTask->StringOut(1,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list);
                           	DisplayTask->ParamIndicPan(72,1,prog_data[cab2_pan]);
                         }
                         DisplayTask->ParamIndicNum(42,3,prog_data[cab2_vol]);
                         vol_vol = prog_data[cab2_vol];
                         gui_send(7,3);
                         DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB2);
                         DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                         DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                         current_menu = MENU_CABSIM;
                         par_num = 2;
                         ind_poin = 500;
                         vol_fl = 1;
                         tim5_start(0);
                       }
                    }
                    if(k_down == 1)
                    {
                    	//FSTask->SendCommand( TFsBrowser::bcBack );
                    }
                    clean_flag();
                    break;
//------------------------------------------------------------------------Menu write-------------------------------------------------------
        case MENU_PRESET_WRITE:
          if(tim5_fl == 0)DisplayTask->Prog_ind(prog1);
          else if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
          if(encoder_state_updated == 1)
            {
              if(encoder_state == 1)
                {
                  if(prog1 > 0)
                    {
                      prog1 -=1;
                    }
                  else prog1 = 98;
                }
              if(encoder_state == 2)
                {
                  if(prog1 < 98)
                    {
                      prog1 +=1;
                    }
                  else prog1 = 0;
                }
              eepr_read_imya(prog1);
              imya_temp = 1;
              DisplayTask->Main_scr();
              DisplayTask->Prog_ind(prog1);
              DisplayTask->StringOut(40,3,TDisplayTask::fntSystem,0,(uint8_t*)write_);
              TIM_SetCounter(TIM6,0x8000);
              TIM_ClearFlag(TIM6,TIM_FLAG_Update);
              TIM_Cmd(TIM6,ENABLE);
              tim5_start(0);
              clean_flag();
            }
          if((k_down == 1) || (encoder_knob_pressed == 1))
            {
              DisplayTask->Clear();
              DisplayTask->StringOut(38,2,TDisplayTask::fnt12x13,0,(uint8_t*)write__);
              prog_data[147] = delay_time;
              prog_data[148] = delay_time >> 8;
              eepr_write(prog1);
              send_cab_data(0,prog1 + 1,0);
              if(cab_type == 2)send_cab_data1(0,prog1 + 1);
              prog1 = prog;
              prog_cur = vol_fl = write_fl = num_men = 0;
              current_menu = MENU_MAIN; // edit_modules_fl = 0
              encoder_knob_selected = 0;
              prog_ch();
              DisplayTask->Main_scr();
              DisplayTask->Prog_ind(prog1);
    	      if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
    		  if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
    		  if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
              tim5_start(0);
              clean_flag();
            }
          if(k_up == 1)
            {
              vol_fl = 0;

              num_men = 0;
              current_menu = MENU_MAIN; //edit_modules_fl = 0;
              prog_ch();
              prog_cur = 0;
              prog1 = prog;
              DisplayTask->Main_scr();
              DisplayTask->Prog_ind(prog1);
    	      if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
    		  if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
    		  if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
              tim5_start(0);
              clean_flag();
            }
          break;
//----------------------------------------------------Menu controllers---------------------------------------------
        case MENU_CONTROLLERS:
          if(encoder_knob_selected == 0)
            {
              if(tim5_fl == 1)
              {
            	  if(par_num < 6)DisplayTask->StringOut(0,par_num % 3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
            	  else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
              }
              else {
            	  if(par_num < 6)DisplayTask->StringOut(0,par_num % 3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
            	  else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
              }
            }
          if(encoder_state_updated == 1)
            {
              if(encoder_state == 1)
                {
                  if(encoder_knob_selected == 0)
                    {
                      if(par_num > 0)
                        {
                    	  if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num-- * 7);
                    	  else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num-- * 7);
                          if(par_num == 2)
                          {
                        	  DisplayTask->Clear();
                        	  DisplayTask->Strel(58,3,0);
                        	  DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)contr_list);
                        	  DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + 7);
                              for(uint8_t i = 0 ; i < 3 ; i++)
                                {
                                  DisplayTask->StringOut(0,i,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_list + i*7);
                                  switch (i){
                                  case 0:DisplayTask->ParamIndicNum(45,0,contrs + 1);break;
                                  case 1:if(!control[contrs*4])DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
                                         else {
                                        	 DisplayTask->Clear_str(45,1,TDisplayTask::fntSystem,13);
                                       	     if(control[contrs*4] > 4)
                                       	     {
                                       		   DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
                                       		   DisplayTask->ParamIndicNum(69,1,control[contrs*4] - 5);
                                       	     }
                                       	     else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[contrs*4] - 1)*12);
                                         }break;
                                  case 2:DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + (control[contrs*4 + 1])*14);break;
                                  }
                                }
                          }
                          DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num * 7);

                        }
                    }
                  else {
                      switch (par_num){
                      case 0:if(contrs > 0)
                    	 {
                    	  DisplayTask->Clear_str(45,1,TDisplayTask::fntSystem,13);
                    	  DisplayTask->ParamIndicNum(45,0,--contrs + 1);
                          if(!control[contrs*4])DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
                          else {
                     	   if(control[contrs*4] > 4)
                     	    {
                     		 DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
                     		 DisplayTask->ParamIndicNum(69,1,control[contrs*4] - 5);
                     	    }
                     	   else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[contrs*4] - 1)*12);
                          }
                          control_destin = dest_tabl_start[control[contrs * 4 + 1]];
                          DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + (control[contrs * 4 + 1])*14);
                    	 }break;
                      case 1:if(control[contrs * 4] > 0)
                        {
                    	  DisplayTask->Clear_str(45,1,TDisplayTask::fntSystem,13);
                    	  control[contrs * 4] = enc_speed_dec(control[contrs * 4],0);
                    	  if(control[contrs * 4] == 4)
                    	  {
                      		if(sys_para[fs3] != 1)control[contrs * 4]--;
                    	  }
                    	  if(control[contrs * 4] == 3)
                    	  {
                      		if(sys_para[fs2] != 1)control[contrs * 4]--;
                    	  }
                    	  if(control[contrs * 4] == 2)
                    	  {
                      		if(sys_para[fs1] != 1)control[contrs * 4]--;
                    	  }
                    	  if(control[contrs * 4] == 1)
                    	  {
                      		if(sys_para[exp_typ] < 3)control[contrs * 4]--;
                    	  }
                          if(!control[contrs*4])DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
                          else {
                        	  if(control[contrs*4] > 4)
                        	  {
                        	    DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
                                DisplayTask->ParamIndicNum(69,1,control[contrs * 4] - 5);
                              }
                        	  else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[contrs*4] - 1)*12);
                          }
                        }break;
                      case 2:
                    	  if(control_destin)
                    	  {
                    		  control[contrs * 4 + 1] = dest_tabl[--control_destin];
                    		  DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + (control[contrs * 4 + 1])*14);
                    	  }break;
                      case 3:if(control[2 + contrs * 4] > 0)
                      {
                    	  control[2 + contrs * 4] = enc_speed_dec(control[2 + contrs * 4],0);
                    	  DisplayTask->ParamIndic(45,0,control[2 + contrs * 4]);
                      }break;
                      case 4:if(control[3 + contrs * 4] > 0)
                      {
                    	  control[3 + contrs * 4] = enc_speed_dec(control[3 + contrs * 4],0);
                    	  DisplayTask->ParamIndic(45,1,control[3 + contrs * 4]);
                      }break;
                      case 5:if(control[510] > 0)DisplayTask->StringOut(45,2,TDisplayTask::fntSystem,0,(uint8_t*)out_pc_list + --control[510] * 8);
                      break;
                      case 6:uint8_t a = 0;if(control[511] & 0x80)a = control[511] & 0x7f;else a = prog;
                      if(a > 0)
                      {
                    	  a = enc_speed_dec(a,0);
                    	  DisplayTask->ParamIndicNum(45,3,a + 1);
                    	  control[511] = a | 0x80;
                      }break;
                  }
                  tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
                  for(uint8_t i = 0 ; i < 32 ; i++)
                   {
                	  if(control[i*4])
                	  {
                		  if(control[i*4 + 1] == 9)tap_del_fl = 1;
                		  if(control[i*4 + 1] == 26)tap_trem_fl = 1;
                		  if(control[i*4 + 1] == 35)tap_moog_fl = 1;
                	  }
                   }
                }
               }
              if(encoder_state == 2)
                {
                  if(encoder_knob_selected == 0)
                    {
                	 if(par_num < 6)
                	 {
                         DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num++ * 7);
                         if(par_num == 3)
                         {
                        	 DisplayTask->Clear();
                        	 DisplayTask->Icon_Strel(ICON_CC, STRELKA_UP);
                        	 DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + (par_num+1) * 7);
                        	 DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + (par_num+2) * 7);
                        	 DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + (par_num+3) * 7);
                        	 DisplayTask->ParamIndic(45,0,control[2 + contrs * 4]);
                        	 DisplayTask->ParamIndic(45,1,control[3 + contrs * 4]);
                        	 DisplayTask->StringOut(45,2,TDisplayTask::fntSystem,0,(uint8_t*)out_pc_list + control[510] * 8);
                        	 if(control[511] & 0x80)DisplayTask->ParamIndicNum(45,3,(control[511] & 0x7f) + 1);
                        	 else DisplayTask->ParamIndicNum(45,3,prog + 1);
                         }
                         if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num * 7);
                         else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num * 7);
                	 }
                    }
                  else {
                      switch (par_num){
                      case 0:if(contrs < 31)
                    	  {
                    	    DisplayTask->ParamIndicNum(45,0,++contrs + 1);
                    	    DisplayTask->Clear_str(45,1,TDisplayTask::fntSystem,13);
                            if(!control[contrs*4])DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc_of);
                            else {
                     	     if(control[contrs*4] > 4)
                     	      {
                     		   DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
                     		   DisplayTask->ParamIndicNum(69,1,control[contrs*4] - 5);
                     	      }
                     	     else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[contrs*4] - 1)*12);
                            }
                           control_destin = dest_tabl_start[control[contrs * 4 + 1]];
                           DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + (control[contrs * 4 + 1])*14);
                    	  }break;
                      case 1:if(control[contrs * 4] < 132)
                      {
                    	DisplayTask->Clear_str(45,1,TDisplayTask::fntSystem,13);
                    	control[contrs * 4] = enc_speed_inc(control[contrs * 4],132);
                    	if(control[contrs * 4] == 1)
                    	{
                    		if(sys_para[exp_typ] < 3)control[contrs * 4]++;
                    	}
                    	if(control[contrs * 4] == 2)
                    	{
                    		if(sys_para[fs1] != 1)control[contrs * 4]++;
                    	}
                    	if(control[contrs * 4] == 3)
                    	{
                    		if(sys_para[fs2] != 1)control[contrs * 4]++;
                    	}
                    	if(control[contrs * 4] == 4)
                    	{
                    		if(sys_para[fs3] != 1)control[contrs * 4]++;
                    	}
                      	if(control[contrs*4] > 4)
                      	  {
                      	    DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cc);
                            DisplayTask->ParamIndicNum(69,1,control[contrs * 4] - 5);
                          }
                      	else DisplayTask->StringOut(45,1,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_ext_l + (control[contrs*4] - 1)*12);
                      }break;
                      case 2:
                    	  if(control_destin < 54)
                    	  {
                    		  control[contrs * 4 + 1] = dest_tabl[++control_destin];
                    		  DisplayTask->StringOut(45,2,TDisplayTask::fntSystem , 0 , (uint8_t*)midi_dect_list + (control[contrs * 4 + 1])*14);
                    	  }break;
                      case 3:if(control[2 + contrs * 4] < 127)
                      {
                    	  control[2 + contrs * 4] = enc_speed_inc(control[2 + contrs * 4],127);
                    	  DisplayTask->ParamIndic(45,0,control[2 + contrs * 4]);
                      }break;
                      case 4:if(control[3 + contrs * 4] < 127)
                      {
                    	  control[3 + contrs * 4] = enc_speed_inc(control[3 + contrs * 4],127);
                    	  DisplayTask->ParamIndic(45,1,control[3 + contrs * 4]);
                      }break;
                      case 5:if(control[510] < 2)DisplayTask->StringOut(45,2,TDisplayTask::fntSystem,0,(uint8_t*)out_pc_list + ++control[510] * 8);
                      break;
                      case 6:uint8_t a = 0;if(control[511] & 0x80)a = control[511] & 0x7f;else a = prog;
                      if(a < 127)
                      {
                    	  a = enc_speed_inc(a,127);
                    	  DisplayTask->ParamIndicNum(45,3,a + 1);
                    	  control[511] = a | 0x80;
                      }break;
                     }
                     tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
                     for(uint8_t i = 0 ; i < 32 ; i++)
                      {
                    	 if(control[i*4])
                    	 {
                    		 if(control[i*4 + 1] == 9)tap_del_fl = 1;
                    		 if(control[i*4 + 1] == 26)tap_trem_fl = 1;
                    		 if(control[i*4 + 1] == 35)tap_moog_fl = 1;
                    	 }
                      }
                  }
               }
              tim5_start(0);
              clean_flag();
            }
          if(encoder_knob_pressed == 1)
            {
                  if(encoder_knob_selected == 0)
                    {
                      encoder_knob_selected = 1;
                      if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
                      else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
                    }
                  else {
                	  if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
                	  else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
                      encoder_knob_selected = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
          if((k_att == 1) || (k_sys == 1) || (k_tuner == 1) || (k_master == 1))
          {
        	  if(k_tuner == 1)num_men = 14;
              current_menu = MENU_MAIN;
              encoder_knob_selected = 0;
              tim5_start(0);
          	  CSTask->Give();
          	  break;
          }
          if((k_up == 1) || (k_master_eq == 1))
          {
        	sub_men = 0;
        	GUI_return_to_main_menu();
          }
        break;
//------------------------------------------System menu---------------------------------------------------------
        case MENU_SYSTEM:
          if(!encoder_knob_selected)DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,tim5_fl * 2,(uint8_t*)sys_menu_list + par_num*12);

          if(encoder_state_updated == 1)
            {
              if(encoder_state == 1)
                {
                  if(encoder_knob_selected == 0)
                    {
                      if(par_num > 0)
                        {
                          DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + par_num--*12);
                          if(par_num == 3)
                          {
                        	  DisplayTask->Clear();
                        	  DisplayTask->Icon_Strel(ICON_SY, STRELKA_DOWN);
                        	  for(uint8_t i = 0 ; i < 4 ; i++)
                        	  {
                        		  DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + i*12);
                        		  switch(i){
                            	  case 0:DisplayTask->StringOut(36,i,TDisplayTask::fntSystem , 0 , (uint8_t*)mode_list + sys_para[i]*12);
                            	  gui_send(14,0);break;
                        	      case 1:DisplayTask->ParamIndicNum(60,i,sys_para[i]+1);break;
                        	      case 2:DisplayTask->StringOut(60,i,TDisplayTask::fntSystem,0,(uint8_t*)cab_out_list + sys_para[i]*6);break;
                              	  case 3:if(!(sys_para[exp_typ] & 0x80))DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
                              	         else DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
                              	  break;
                        		  }
                        	  }
                          }
                          else {
                        	  if(par_num == 7)
                        	  {
                    			  DisplayTask->Clear();
                    			  DisplayTask->Icon_Strel(ICON_SY, STRELKA_UPDOWN);
                    			  DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + 4*12);
                    			  DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + 5*12);
                    			  DisplayTask->StringOut(44,1,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + sys_para[spdif_t]*12);
                    			  DisplayTask->StringOut(3,2,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + 6*12);
                    			  DisplayTask->StringOut(38,3,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + sys_para[tap_typ]*10);
                        	  }
                          }
                          DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + par_num*12);

                        }
                    }
                  else {
                	switch (par_num){
                    case 0:if(sys_para[par_num])
                      {
                     	  DisplayTask->StringOut(36,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)mode_list + --sys_para[par_num]*12);
                      	  gui_send(14,0);
                      }break;
                	case 1:if(sys_para[par_num])DisplayTask->ParamIndicNum(60,par_num,--sys_para[par_num]+1);break;
                	case 2:if(sys_para[par_num])
                		    {
                	 	      DisplayTask->StringOut(60,par_num,TDisplayTask::fntSystem,0,(uint8_t*)cab_out_list + --sys_para[par_num]*6);
                	 	      gui_send(28,sys_para[par_num]);
                		    }
                	       break;
                    case 3:if(sys_para[par_num] & 0x80)
                      	  {
                    	     adc_init(0);
                             DisplayTask->StringOut(78,3,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
                    	     sys_para[par_num] &= 0x7f;
                    	   	 ext_send(127);
                      	  }break;
                    case 5:if(sys_para[spdif_t])
                      	  {
                    	 	 DisplayTask->StringOut(44,1,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + --sys_para[spdif_t]*12);
                    	 	 gui_send(26,0);
                      	  }break;
              	    case 7:if(sys_para[tap_typ])
              	  	  	  {
              		        DisplayTask->StringOut(38,3,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + --sys_para[tap_typ]*10);
              		        sys_para[tap_hi] = 0;
            		        gui_send(33,sys_para[tap_typ] | (sys_para[tap_hi] << 8));
              	  	  	  }break;
              	    case 8:if((sys_para[tun_ext] & 0x80))
              	  	  	  {
              		        sys_para[tun_ext] &= ~0x80;
              		  	  	DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
              	  	  	  }break;
              	    case 9:if(sys_para[tim_type])
              		      {
              		        sys_para[tim_type]--;
              		        DisplayTask->StringOut(39,1,TDisplayTask::fntSystem,0,(uint8_t*)time_type);
              		      }break;
              	    case 10:if(sys_para[swap_switch])
              		  	  {
              		        sys_para[swap_switch]--;
              		        DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
              		  	  }break;
              	    case 11:if(sys_para[speed_tun])
              	  	  	  {
              		  	  	uint8_t a = sys_para[speed_tun];
              		  	    a = enc_speed_dec(a,0);
              		  	    sys_para[speed_tun] = a;
              		  	    DisplayTask->ParamIndicNum(78,3,a);
              		  	    tun_del_val = (127 - sys_para[speed_tun]) * (90.0f/127.0f) + 10.0f;
              	  	  	  }break;
                	}
                  }
                }
              if(encoder_state == 2)
                {
                  if(encoder_knob_selected == 0)
                    {
                	  if(par_num < 11)
                	  {
                		  DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + par_num++*12);
                		  if(par_num == 4)
                		  {
                			  DisplayTask->Clear();
                			  DisplayTask->Icon_Strel(ICON_SY, STRELKA_UPDOWN);
                			  DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 1)*12);
                			  DisplayTask->StringOut(44,1,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + sys_para[spdif_t]*12);
                			  DisplayTask->StringOut(3,2,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 2)*12);
                			  DisplayTask->StringOut(3,3,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 3)*12);
                			  DisplayTask->StringOut(38,3,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + sys_para[tap_typ]*10);
                		  }
                		  else {
                			  if(par_num == 8)
                			  {
                				  DisplayTask->Clear();
                				  DisplayTask->Icon_Strel(ICON_SY, STRELKA_UPDOWN);
                				  DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 1)*12);
                				  DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 1)*12);
                				  DisplayTask->StringOut(39,1,TDisplayTask::fntSystem,0,(uint8_t*)time_type + sys_para[tim_type]*4);
                				  if(sys_para[tun_ext] & 0x80)DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
                				  else DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
                				  DisplayTask->StringOut(3,2,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 2)*12);
                				  if(!sys_para[swap_switch])DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
                				  else DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"On");
                				  DisplayTask->StringOut(3,3,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 3)*12);
                				  DisplayTask->ParamIndicNum(78,3,sys_para[speed_tun]);
                			  }
                		  }
                		  DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + par_num*12);
                	  }
                    }
                  else {
                	  switch (par_num){
                	  case 0:if(sys_para[par_num] < 1)
                		  {
                		  	  DisplayTask->StringOut(36,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)mode_list + ++sys_para[par_num]*12);
                		  	  gui_send(14,0);
                		  }break;
                	  case 1:if(sys_para[par_num] < 15)DisplayTask->ParamIndicNum(60,par_num,++sys_para[par_num]+1);break;
                	  case 2:if(sys_para[par_num] < 5)
                		      {
                		        DisplayTask->StringOut(60,par_num,TDisplayTask::fntSystem,0,(uint8_t*)cab_out_list + ++sys_para[par_num]*6);
                		        gui_send(28,sys_para[par_num]);
                		      }
                	         break;
                	  case 3:if(!(sys_para[par_num] & 0x80))
                		  	  {
                		  	  	DisplayTask->StringOut(78,3,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
                		  	  	sys_para[par_num] |= 0x80;
                		  	    adc_init(1);
                		  	  }break;
                	  case 5:if(!sys_para[spdif_t])
                		  	  {
                		  	    DisplayTask->StringOut(44,1,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + ++sys_para[spdif_t]*12);
                		  	    gui_send(26,1);
                		  	  }break;
                	  case 7:if(sys_para[tap_typ] < 2)
                	  	  	  {
                		        DisplayTask->StringOut(38,3,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + ++sys_para[tap_typ]*10);
                		        gui_send(33,sys_para[tap_typ] | (sys_para[tap_hi] << 8));
                	  	  	  }break;
                	  case 8: if(!(sys_para[tun_ext] & 0x80))
                	  	  	  {
                		        sys_para[tun_ext] |= 0x80;
                		  	  	DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
                	  	  	  }break;
                	  case 9:if(!sys_para[tim_type])
                		      {
                		        sys_para[tim_type]++;
                		        DisplayTask->StringOut(39,1,TDisplayTask::fntSystem,0,(uint8_t*)time_type + 4);
                		      }break;
                	  case 10:if(!sys_para[swap_switch])
                		  	  {
                		        sys_para[swap_switch]++;
                		        DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
                		  	  }break;
                	  case 11:if(sys_para[speed_tun] < 127)
                	  	  	  {
                		  	  	uint8_t a = sys_para[speed_tun];
                		  	    a = enc_speed_inc(a,127);
                		  	    sys_para[speed_tun] = a;
                		  	    DisplayTask->ParamIndicNum(78,3,a);
                		  	    tun_del_val = (127 - sys_para[speed_tun]) * (90.0f/127.0f) + 10.0f;
                	  	  	  }break;
                	  }
                   }
                }
              tim5_start(0);
              clean_flag();
            }
          if(k_down == 1)
          {
        	  if((par_num == 3) && (sys_para[exp_typ] & 0x80))
        	  {
        		  current_menu = MENU_EXPRESSION;
        		  DisplayTask->Clear();
        		  for(uint8_t i = 0 ; i < 4 ; i++)
        		  {
        			  DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + i*10);
        		  }
        	      DisplayTask->StringOut(60,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_type + (sys_para[exp_typ] & 0x7f)*12);
				  if(!sys_para[exp_ccN])DisplayTask->StringOut(84,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
				  else DisplayTask->ParamIndicNum(84,2,sys_para[exp_ccN] - 1);
				  if(!sys_para[sav_exp])DisplayTask->StringOut(84,3,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
				  else DisplayTask->StringOut(84,3,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
				  eq_num = 0;
        	      encoder_knob_selected = 0;
        	  }
        	  else {
        		  if((par_num == 8) && (sys_para[tun_ext] & 0x80))
        		  {
        			  current_menu = MENU_TUNER_EXT;
        			  DisplayTask->Clear();
        			  DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        			  DisplayTask->ParamIndicNum(50,0,sys_para[tun_ext] & 0x7f);
        		  }
        	  }
              clean_flag();
          }
          if(encoder_knob_pressed == 1)
            {
        	  switch(par_num){
        	  case 4:
        		  current_menu = MENU_FSW_TYPE;
        	  	  DisplayTask->Clear();
        	      for(uint8_t i = 0 ; i < 4 ; i++)DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + i*12);
        		  eq_num = 0;encoder_knob_pressed = 0;DisplayTask->ParamIndic(58,3,sys_para[foot_sp]);break;
        	  case 6:
        		  current_menu = MENU_MIDI_PC;
        	  	  DisplayTask->Clear();
        	  	  eq_num = 0;
        	  	  encoder_knob_pressed = 0;
        	      DisplayTask->ParamIndicNum(3,0,eq_num + 1);
        	      DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)"->");
        	      DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);break;
        	  default:if(encoder_knob_selected == 0)
                  {
                	  encoder_knob_selected = 1;
                	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + par_num*12);
                  }
                  else {
                	  encoder_knob_selected = 0;
                	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + par_num*12);
                  }break;
        	  }
        	  tim5_start(0);
              clean_flag();
            }
          if((k_up == 1) || (k_sys == 1))
          {
        	if(temp_sys == 2)
        	{
        		if(temp_sys != sys_para[2])
        		{
            		write_sys();
            		NVIC_SystemReset();
        		}
        	}
        	else {
        		if(sys_para[2] == 2)
        		{
        			write_sys();
        			NVIC_SystemReset();
        		}
        	}
            write_sys();

            encoder_knob_selected = 0;
            eq_num = prog;
            current_menu = MENU_MAIN;//edit_modules_fl = 0;
            DisplayTask->Main_scr();
            DisplayTask->Prog_ind(prog1);
            if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
        	tim5_start(0);
            clean_flag();
          }
        if((k_att == 1) || (k_master == 1) || (k_tuner == 1) || (k_master_eq == 1))
        {
        	if(temp_sys == 2)
        	{
        		if(temp_sys != sys_para[2])
        		{
            		write_sys();
            		NVIC_SystemReset();
        		}
        	}
        	else {
        		if(sys_para[2] == 2)
        		{
        			write_sys();
        			NVIC_SystemReset();
        		}
        	}
        	current_menu = MENU_MAIN; //edit_modules_fl = 0;
        	eq_num = prog;
        	write_sys();
        	encoder_knob_selected = 0;

        	DisplayTask->Clear();
        	tim5_start(0);
        	CSTask->Give();
        	break;
        }
        break;
//-----------------------------------------------------Expression-------------------------------------------
        case MENU_EXPRESSION:
        if(encoder_knob_selected == 0)
        {
          if(tim5_fl == 1)DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);
          else DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num*10);
        }
        if(encoder_state_updated)
        {
        	if(encoder_state == 1)
        	{
        		if(!encoder_knob_selected)
        		{
        			if(eq_num > 0)
        			{
        				DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num--*10);
        				DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);
        				tim5_start(0);
        			}
        		}
        		else {
        			switch(eq_num){
        			case 0:if((sys_para[exp_typ] & 0x7f) > 1)
        					{
        						DisplayTask->StringOut(60,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_type + (--sys_para[exp_typ] & 0x7f)*12);
        						adc_pin_init();
        						if(sys_para[exp_typ] > 2)ext_send(127);
        					}break;
        			case 2:if(sys_para[exp_ccN] > 0)
        				{
        					sys_para[exp_ccN] = enc_speed_dec(sys_para[exp_ccN],0);
        					if(!sys_para[exp_ccN])DisplayTask->StringOut(84,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(84,2,sys_para[exp_ccN] - 1);
        				}break;
        			case 3:if(sys_para[sav_exp])
        				{
        				    sys_para[sav_exp]--;
        					DisplayTask->StringOut(84,3,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        				}break;
        			}
        		}
        	}
        	if(encoder_state == 2)
        	{
        		if(!encoder_knob_selected)
        		{
            		if(eq_num < 3)
            		{
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num++*10);
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);
            			tim5_start(0);
            		}
        		}
        		else {
        			switch(eq_num){
        			case 0:if((sys_para[exp_typ] & 0x7f) < 4)
        					{
        						DisplayTask->StringOut(60,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_type + (++sys_para[exp_typ] & 0x7f)*12);
        						adc_pin_init();
        						if(sys_para[exp_typ] > 2)ext_send(127);
        					}break;
        			case 2:if(sys_para[exp_ccN] < 128)
        				{
        					sys_para[exp_ccN] = enc_speed_inc(sys_para[exp_ccN],100);
        					if(!sys_para[exp_ccN])DisplayTask->StringOut(84,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(84,2,sys_para[exp_ccN] - 1);
        				}break;
        			case 3:if(!sys_para[sav_exp])
        				{
        				    sys_para[sav_exp]++;
        					DisplayTask->StringOut(84,3,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
        				}break;
        			}
        		}
        	}
            clean_flag();
        }
        if(encoder_knob_pressed)
        {
        	if(!encoder_knob_selected)
        	{
        		encoder_knob_selected = 1;
        		switch(eq_num){
        		case 0:DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);break;
        		case 1:DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_min);
        		current_menu = MENU_ADC_CALIBRATE;
        		tim5_start(0);
        		break;
        		case 2:DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);break;
        		}
        	}
        	else {
        		encoder_knob_selected = 0;
        		DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num*10);
        	}
            clean_flag();
        }
        if(k_up == 1)
        {
          current_menu = MENU_SYSTEM;
          encoder_knob_selected = 0;
      	  DisplayTask->Clear();
      	  DisplayTask->Icon_Strel(ICON_SY, STRELKA_DOWN);
      	  for(uint8_t i = 0 ; i < 4 ; i++)
      	  {
      		  DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + i*12);
      		  switch(i){
          	  case 0:DisplayTask->StringOut(36,i,TDisplayTask::fntSystem , 0 , (uint8_t*)mode_list + sys_para[i]*12);
          	  gui_send(14,0);break;
      	      case 1:DisplayTask->ParamIndicNum(60,i,sys_para[i]+1);break;
      	      case 2:DisplayTask->StringOut(60,i,TDisplayTask::fntSystem,0,(uint8_t*)cab_out_list + sys_para[i]*6);break;
          	  case 3:if(!sys_para[exp_typ])DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
          	         else DisplayTask->StringOut(78,i,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
          	  break;
      		  }
      	  }
          clean_flag();
        }
  	    break;
//-----------------------------------------------------Calibrate--------------------------------------------
        case MENU_ADC_CALIBRATE:
   		 if(tim5_fl == 1)
   		 {
   			 if(calib_fl)DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 2 , (uint8_t*)set_max);
   			 else DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 2 , (uint8_t*)set_min);
   		 }
   		 else {
   			 if(calib_fl)DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_max);
   			 else DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_min);
   		 }
    	 if(encoder_knob_pressed == 1)
    	 {
    		 if(calib_fl == 0)
    		 {
    			 sys_para[7] = adc_bu;
    			 sys_para[8] = adc_bu >> 8;
    			 DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 3 , (uint8_t*)set_min);
    			 DisplayTask->StringOut(92,1,TDisplayTask::fntSystem , 0 , (uint8_t*)ok);
    			 dela(0x7fffff);
    			 DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 2 , (uint8_t*)set_max);
    			 calib_fl = 1;
    			 tim5_start(0);
    		 }
    		 else {
    			 sys_para[9] = adc_bu;
    			 sys_para[10] = adc_bu >> 8;
    			 DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 3 , (uint8_t*)set_max);
    			 DisplayTask->StringOut(92,1,TDisplayTask::fntSystem , 0 , (uint8_t*)ok);
    			 dela(0x7fffff);
    			 calib_fl = 0;
    	         current_menu = MENU_EXPRESSION;
    	         encoder_knob_selected = 0;
    		 }
    		 tim5_start(0);
    	     clean_flag();
    	     adc_calib();
    	 }
      	break;
//---------------------------------------------------Attenuator--------------------------------------------------
        case MENU_ATTENUATOR:
            if(encoder_state_updated == 1)
              {
                if(encoder_state == 2)
                {
                	if(sys_para[127] > 0)
                	{
                		sys_para[127] = enc_speed_dec(sys_para[127],0);
                		DisplayTask->Pot_Write();
                		DisplayTask->StringOut(76,0,TDisplayTask::fntSystem,0,(uint8_t*)att_db + sys_para[127]*4);
                	}
                }
                if(encoder_state == 1)
                {
                	if(sys_para[127] < 55)
                	{
                		sys_para[127] = enc_speed_inc(sys_para[127],55);
                		DisplayTask->Pot_Write();
                		DisplayTask->StringOut(76,0,TDisplayTask::fntSystem,0,(uint8_t*)att_db + sys_para[127]*4);
                	}
                }
                tim5_start(0);
                clean_flag();
              }
            if((k_master == 1) || (k_sys == 1) || (k_tuner == 1) || (k_master_eq == 1))
            {
            	current_menu = MENU_MAIN;//edit_modules_fl = 0;
            	write_sys();
            	encoder_knob_selected = 0;

            	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_att == 1))
              {
            	write_sys();
                current_menu = MENU_MAIN; //edit_modules_fl = 0;
                DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
                DisplayTask->Main_scr();
                DisplayTask->Prog_ind(prog1);
            	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
                tim5_start(0);
                clean_flag();
              }
            break;
//----------------------------------------------------Master Volume---------------------------------------------
        case MENU_MASTER_VOLUME:
            if(encoder_knob_selected == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                else DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num*14);
              }
            if(encoder_state_updated == 1)
              {
                if(encoder_state == 1)
                {
                	if(encoder_knob_selected)
                	{
                		if(sys_para[126 - par_num] > 0)
                		{
                			sys_para[126 - par_num] = enc_speed_dec(sys_para[126 - par_num],0);
                			DisplayTask->ParamIndicNum(85,par_num,sys_para[126 - par_num]);
                			if(!par_num)gui_send(1,sys_para[126]);
                			else DisplayTask->Pot_Write();
                		}
                	}
                	else {
                		if(par_num)
                		{
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num--*14);
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                		}
                	}
                }
                if(encoder_state == 2)
                {
                	if(encoder_knob_selected)
                	{
                		if(sys_para[126 - par_num] < 127)
                		{
                			sys_para[126 - par_num] = enc_speed_inc(sys_para[126 - par_num],127);
                			DisplayTask->ParamIndicNum(85,par_num,sys_para[126 - par_num]);
                			if(!par_num)gui_send(1,sys_para[126]);
                			else DisplayTask->Pot_Write();
                		}
                	}
                	else {
                		if(!par_num)
                		{
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num++*14);
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                		}
                	}
                }
                tim5_start(0);
                clean_flag();
              }
            if(encoder_knob_pressed == 1)
              {
                if(encoder_knob_selected == 0)
                {
              	  encoder_knob_selected = 1;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                }
                else {
              	  encoder_knob_selected = 0;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num*14);
                }
                tim5_start(1);
                clean_flag();
              }
            if((k_att == 1) || (k_sys == 1) || (k_tuner == 1) || (k_master_eq == 1))
            {
            	current_menu = MENU_MAIN;//edit_modules_fl = 0;
            	write_sys();
            	encoder_knob_selected = 0;

            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_master == 1))
              {
            	write_sys();
            	encoder_knob_selected = 0;
                current_menu = MENU_MAIN; //edit_modules_fl = 0;
                DisplayTask->Main_scr();
                DisplayTask->Prog_ind(prog1);
            	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
                tim5_start(0);
                clean_flag();
              }
            break;
//----------------------------------------------------Master EQ----------------------------------------
        case MENU_MASTER_EQ:
          if(sys_para[120])
          {
            if(encoder_knob_selected == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
                else DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + par_num*9);
              }
            if(encoder_state_updated == 1)
            {
            	if(encoder_state == 1)
            	{
            		if(encoder_knob_selected)
            		{
            			volatile int8_t a = sys_para[121 + par_num];
            			switch(par_num){
            			case 0:case 1:case 3:if(a > -24)
            				{
            					a = enc_speed_dec(a,-24);
            					sys_para[121 + par_num] = a;
            					DisplayTask->ParamIndicMix(66,par_num,(a * (63.0f/24.0f)) + 63);
            					DisplayTask->Clear_str(103,par_num,TDisplayTask::fntSystem,4);
            					DisplayTask->ParamIndicNum(100,par_num,abs(a)/2);
            					uint8_t b;
            					if(abs(a/2) < 10)b = 112;
            					else b = 115;
            					if(a & 1)DisplayTask->StringOut(b,par_num,TDisplayTask::fntSystem,0,(uint8_t*)".5");
            					else DisplayTask->StringOut(b,par_num,TDisplayTask::fntSystem,0,(uint8_t*)".0");
            					if(a < 0)DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,45);
                				else {
                					if(a > 0)DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,43);
                					else DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,32);
                				}
            					gui_send(25,par_num);
            				}break;
            			case 2:if(mas_eq_fr > 20)
        					{
            					mas_eq_fr = enc_speed_dec(mas_eq_fr,20);
        						uint16_t f = mas_eq_fr;
        						f *= mas_eq_fr * (20.0f/20000.0f) + 1;
        						sys_para[510] = f >> 8;
        						sys_para[511] = f;
        						sys_para[508] = mas_eq_fr >> 8;
        						sys_para[509] = mas_eq_fr;
        						DisplayTask->EqPar(72,2,f,0,5);
        						DisplayTask->StringOut(106,2,TDisplayTask::fntSystem,0,(uint8_t*)"Hz");
        						gui_send(25,par_num);
        					}break;
            			}
            		}
            		else {
            			if(par_num > 0)
            			{
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + par_num--*9);
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
            			}
            		}
            	}
            	if(encoder_state == 2)
            	{
            		if(encoder_knob_selected)
            		{
            			volatile int8_t a = sys_para[121 + par_num];
            			switch(par_num){
            			case 0:case 1:case 3:if(a < 24)
            				{
            					a = enc_speed_inc(a,24);sys_para[121 + par_num] = a;
            					DisplayTask->ParamIndicMix(66,par_num,(a * (63.0f/24.0f)) + 63);
            					DisplayTask->Clear_str(103,par_num,TDisplayTask::fntSystem,4);
            					DisplayTask->ParamIndicNum(100,par_num,abs(a/2));
            					uint8_t b;
            					if(abs(a/2) < 10)b = 112;
            					else b = 115;
            					if(a & 1)DisplayTask->StringOut(b,par_num,TDisplayTask::fntSystem,0,(uint8_t*)".5");
            					else DisplayTask->StringOut(b,par_num,TDisplayTask::fntSystem,0,(uint8_t*)".0");
            					if(a < 0)DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,45);
                				else {
                					if(a > 0)DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,43);
                					else DisplayTask->SymbolOut(58,par_num,TDisplayTask::fntSystem,0,32);
                				}
            					gui_send(25,par_num);
            				}break;
            			case 2:if(mas_eq_fr < 4000)
        					{
            					mas_eq_fr = enc_speed_inc(mas_eq_fr,5411);
        						uint16_t f = mas_eq_fr;
        						f *= mas_eq_fr * (20.0f/20000.0f) + 1;
        						sys_para[510] = f >> 8;
        						sys_para[511] = f;
        						sys_para[508] = mas_eq_fr >> 8;
        						sys_para[509] = mas_eq_fr;
        						DisplayTask->EqPar(72,2,f,0,5);
        						DisplayTask->StringOut(106,2,TDisplayTask::fntSystem,0,(uint8_t*)"Hz");
        						gui_send(25,par_num);
        					}break;
            			}
            		}
            		else {
            			if(par_num < 3)
            			{
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + par_num++*9);
                			DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
            			}
            		}
            	}
            	tim5_start(0);
                clean_flag();
            }
            if(encoder_knob_pressed == 1)
              {
                if(encoder_knob_selected == 0)
                {
              	  encoder_knob_selected = 1;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
                }
                else {
              	  encoder_knob_selected = 0;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + par_num*9);
                }
                tim5_start(1);
                clean_flag();
              }
          }
            if(k_down == 1)
            {
            	if(sys_para[120])sys_para[120] = 0;
            	else sys_para[120] = 1;
            	current_menu = MENU_MAIN;//edit_modules_fl = 0;
            	k_down = 0;
            	k_master_eq = 1;
            	write_sys();
            	encoder_knob_selected = 0;

            	gui_send(18, 14 | (sys_para[120] << 8));
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_att == 1) || (k_sys == 1) || (k_tuner == 1) || (k_master == 1))
            {
            	current_menu = MENU_MAIN;//edit_modules_fl = 0;
            	write_sys();
            	encoder_knob_selected = 0;

            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_master_eq == 1))
              {
            	write_sys();
            	encoder_knob_selected = 0;
                current_menu = MENU_MAIN;//edit_modules_fl = 0;
                DisplayTask->Main_scr();
                DisplayTask->Prog_ind(prog1);
            	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
                tim5_start(0);
                clean_flag();
              }
        	break;
//----------------------------------------------------Tuner-------------------------------------------------------
        case MENU_TUNER:
        	if(encoder_state_updated)
        	{
        		if(encoder_state == 1)
        		{
        			if(SpectrumTask->ref_freq > 430.0f)SpectrumTask->ref_freq -= 1.0f;
        		}
        		if(encoder_state == 2)
        		{
        			if(SpectrumTask->ref_freq < 450.0f)SpectrumTask->ref_freq += 1.0f;
        		}
        		clean_flag();
        	}
        	if((k_up == 1) || (k_tuner == 1))
              {
           	    tuner_use = 0;
                current_menu = MENU_MAIN;
                DisplayTask->Main_scr();
                DisplayTask->Prog_ind(prog1);
            	if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            	if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            	if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
                gui_send(13,1);
                tim5_start(0);
           	    GPIO_ResetBits(GPIOB,GPIO_Pin_11);
                clean_flag();
                send_codec(0xa103);
              }
            if((k_att == 1) || (k_sys == 1) || (k_master == 1) || (k_master_eq == 1))
            {
            	tuner_use = 0;
            	gui_send(13,1);
            	current_menu = MENU_MAIN;//edit_modules_fl = 0;
            	encoder_knob_selected = 0;

            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
            	send_codec(0xa103);
            	break;
            }
            break;
//-------------------------------------------------------Cab Name---------------------------------------------
        case MENU_CABNAME:
        	if(tim5_fl)
        	{
        		if(tim_fl)tim_fl = 0;
        		else {
        			tim_fl = 1;
        			if((cab_n_fl1) && cab_type)
        			{
        				if(name_buf1[0])
        				{
        				  DisplayTask->Clear();
        				  DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)"2 - ");
        				  DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf1 + 1);
            			  cab_n_fl1 = 0;
            			  cab_n_fl2 = 1;
          				  }
        			}
        			else {
        				if(name_buf[0])
        				{
        				  DisplayTask->Clear();
        				  if(cab_type == 2)
        				  {
            				  DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)"1 - ");
            				  DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf + 1);
        				  }
        				  else DisplayTask->StringOut(0,0,TDisplayTask::fntSystem,0,(uint8_t*)name_buf + 1);
            			  cab_n_fl1 = 1;
            			  cab_n_fl2 = 0;
        				}
        			}
        		}
        	}
            if(encoder_state_updated == 1)
            {
        		current_menu = MENU_MAIN;
            	CSTask->Give();
            	break;
            }
        	if(k_up == 1)
        	{
        		current_menu = MENU_MAIN;
        		prog_cur = 0;
        		eepr_read_imya(prog1);
        		DisplayTask->Main_scr();
        		DisplayTask->Prog_ind(prog1);
        		if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
        		if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
        		if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
                clean_flag();
        	}
        	break;
//----------------------------------------------------------Foot Switch type---------------------------------
        case MENU_FSW_TYPE:
            if(!encoder_knob_selected)
              {
            	if(tim5_fl)DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            	else DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + eq_num*12);
              }
            if(encoder_state_updated)
            {
            	if(encoder_state == 1)
            	{
            	  if(!encoder_knob_selected)
            	  {
            		if(eq_num > 0)
            		{
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + eq_num--*12);
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            			tim5_start(0);
            		}
            	  }
            	  else {
                 	 if(sys_para[foot_sp])
                 	  {
                 	     sys_para[foot_sp] = enc_speed_dec(sys_para[foot_sp],0);
           				 DisplayTask->ParamIndic(58,3,sys_para[foot_sp]);
                 	  }
            	  }
            	}
            	if(encoder_state == 2)
            	{
              	  if(!encoder_knob_selected)
              	  {
            		if(eq_num < 3)
            		{
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + eq_num++*12);
            			DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            			tim5_start(0);
            		}
              	  }
              	  else {
                   	 if(sys_para[foot_sp] < 127)
                   	  {
                   	     sys_para[foot_sp] = enc_speed_inc(sys_para[foot_sp],127);
             		     DisplayTask->ParamIndic(58,3,sys_para[foot_sp]);
                   	  }
              	  }
            	}
   	          tim5_start(0);
   	          clean_flag();
            }
            if(encoder_knob_pressed == 1)
            {
              if(eq_num < 3)
              {
            	DisplayTask->Clear();
            	current_menu = MENU_SW_MODE;
            	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + sys_para[fsm1 + eq_num]*7);
            	if(sys_para[fsm1 + eq_num])
            	{
            		DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            		DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            	}
            	par_num1 = 0;
              }
              else {
            	  if(!encoder_knob_selected)
            	  {
            		  encoder_knob_selected = 1;
            		  DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            	  }
            	  else {
            		  encoder_knob_selected = 0;
            		  DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + eq_num*12);
            	  }
              }
             tim5_start(0);
             clean_flag();
            }
        	if(k_up == 1)
        	{
   			    DisplayTask->Clear();
   			    for(uint8_t i = 0 ; i < 4 ; i++)
   			    {
   			    	DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + 48 + i*12);
   			    	if(i == 1)DisplayTask->StringOut(44,i,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + sys_para[spdif_t]*12);
   			    	if(i == 3)DisplayTask->StringOut(44,i,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + sys_para[tap_typ]*7);
   			    }
   	            current_menu = MENU_SYSTEM;
   	            DisplayTask->Icon_Strel(ICON_SY, STRELKA_UP);
   	            encoder_knob_selected = 0;
   	            tim5_start(0);
   	            clean_flag();
        	}
        	break;
//--------------------------------------------------Foot SW mode----------------------------------------------------------------
        case MENU_SW_MODE:
            if(!encoder_knob_selected)
              {
            	if(tim5_fl)
            	{
            		if(!par_num1)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)mode_);
            		else {
            			if(sys_para[fsm1 + eq_num])DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,2,(uint8_t*)switch_slow + ((par_num1-1) * 10));
            			else DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,2,(uint8_t*)switch_mod);
            		}
            	}
            	else {
            		if(!par_num1)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            		else {
            			if(sys_para[fsm1 + eq_num])DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + ((par_num1-1) * 10));
            			else DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod);
            		}
            	}
              }
            if(encoder_state_updated)
            {
            	if(encoder_state == 1)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(sys_para[fsm1 + eq_num])
            			{
            				if(par_num1)
            				{
            					DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + ((par_num1-- -1) * 10));
            					if(par_num1)DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,2,(uint8_t*)switch_slow + (par_num1-1)*10);
            					else DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)mode_);
            					tim5_start(0);
            				}
            			}
            			else {
            				if(par_num1)
            				{
            					DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + --par_num1);
            					DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)mode_);
            					tim5_start(0);
            				}
            			}
            		}
            		else {
            			if(sys_para[fsm1 + eq_num])
            			{
            				DisplayTask->Clear_str(10,2,TDisplayTask::fntSystem,35);
            				DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + --sys_para[fsm1 + eq_num]);
            			}
            		}
            	}
            	if(encoder_state == 2)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(sys_para[fsm1 + eq_num])
            			{
            				if(par_num1 < 2)
            				{
            					if(!par_num1) {DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_ + par_num1++);}
            					else          {DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + ((par_num1++ -1) * 10));}
            					DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,2,(uint8_t*)switch_slow + (par_num1-1)*10);
            					tim5_start(0);
            				}
            			}
            			else {
            				if(!par_num1)
            				{
            					if(!par_num1)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_ + par_num1++);
            					else DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,2,(uint8_t*)switch_mod);
            					tim5_start(0);
            				}
            			}
            		}
            		else {
            			if(!sys_para[fsm1 + eq_num])
            			{
            				DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + ++sys_para[fsm1 + eq_num]*7);
            				DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            				DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            			}
            		}
            	}
                clean_flag();
            }
            if(encoder_knob_pressed == 1)
            {
            	if(encoder_knob_selected)
            	{
            		encoder_knob_selected = 0;
            		if(!par_num1)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            		else DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + ((par_num1-1) * 10));
            	}
            	else {
            		if(!par_num1)
            		{
            			DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)mode_);
                		encoder_knob_selected = 1;
            		}
            		else {
                    	DisplayTask->Clear();
                    	par_num2 = 0;
                    	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
                       if(par_num1 == 1)
                       {
                       	current_menu = MENU_SW_SELECT0;
                    	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs1 + eq_num] * 12);
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				uint8_t a = sys_para[fs1 + eq_num] - 3;
                			str_pres_init(str_temp , sys_para[pr11 + eq_num * 4] + 1);
                			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                			if(a)str_pres_init(str_temp + 4 , sys_para[pr11 + eq_num * 4 + 1] + 1);
                			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr11 + eq_num * 4 + 2] + 1);
                			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr11 + eq_num * 4 + 3] + 1);
                			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            			else {
            				if(sys_para[fs1 + eq_num] == 1)
            				{
            					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
            					if(!sys_para[k1_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
            					else DisplayTask->ParamIndicNum(76,2,sys_para[k1_cc + eq_num] - 1);
            				}
            			}
                       }
                       else {
                       	current_menu = MENU_SW_SELECT1;
                       	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs11 + eq_num] * 12);
               			if(sys_para[fs11 + eq_num] > 2)
               			{
               				uint8_t a = sys_para[fs11 + eq_num] - 3;
                   			str_pres_init(str_temp , sys_para[pr111 + eq_num * 4] + 1);
                   			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                   			if(a)str_pres_init(str_temp + 4 , sys_para[pr111 + eq_num * 4 + 1] + 1);
                   			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                   			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr111 + eq_num * 4 + 2] + 1);
                   			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                   			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr111 + eq_num * 4 + 3] + 1);
                   			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
               			}
               			else {
               				if(sys_para[fs11 + eq_num] == 1)
               				{
               					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
               					if(!sys_para[k11_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
               					else DisplayTask->ParamIndicNum(76,2,sys_para[k11_cc + eq_num] - 1);
               				}
               			}
                       }
            		}
            	}
            	tim5_start(0);
            	clean_flag();
            }
        	if(k_up == 1)
        	{
   			    DisplayTask->Clear();
   			    for(uint8_t i = 0 ; i < 4 ; i++)DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + i*12);
   			    DisplayTask->ParamIndic(58,3,sys_para[foot_sp]);
   			    current_menu = MENU_FSW_TYPE;
   	            encoder_knob_selected = 0;
   	            tim5_start(0);
   	            clean_flag();
        	}
        	break;
//--------------------------------------------------Foot SW set-----------------------------------------------------------------
        case MENU_SW_SELECT0:
            if(!encoder_knob_selected)
              {
            	if(tim5_fl)
            	{
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				str_temp[(par_num2 - 1) * 4] |= 0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	else {
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				str_temp[(par_num2 - 1) * 4] &= ~0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] &= ~0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
              }
            if(encoder_state_updated)
            {
            	if(encoder_state == 1)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(par_num2 > 0)
            			{
            				if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
                			str_temp[(par_num2 - 1) * 4] &= ~0x80;
                			str_temp[(par_num2-- - 1) * 4 + 1] &= ~0x80;
                			DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                			str_temp[(par_num2 - 1) * 4] |= 0x80;
                			str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
                			DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                			if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
                			tim5_start(0);
            			}
            		}
            		else {
            		  if(!par_num2)
            		  {
            			if(sys_para[fs1 + eq_num] > 0)sys_para[fs1 + eq_num]--;
            			if(sys_para[fs1 + eq_num] == 1)
            			{
            				//if(sys_para[fs11 + eq_num] == 1)sys_para[fs1 + eq_num]--;
            			}
            			DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs1 + eq_num] * 12);
            			DisplayTask->Clear_str(0,2,TDisplayTask::fntSystem,21);
            			kgp_sdk_libc::memset(str_temp,0,22);
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				uint8_t a = sys_para[fs1 + eq_num] - 3;
                			str_pres_init(str_temp , sys_para[pr11 + eq_num * 4] + 1);
                			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                			if(a)str_pres_init(str_temp + 4 , sys_para[pr11 + eq_num * 4 + 1] + 1);
                			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr11 + eq_num * 4 + 2] + 1);
                			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr11 + eq_num * 4 + 3] + 1);
                			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            			if(sys_para[fs1 + eq_num] == 1)
            			{
        					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        					if(!sys_para[k1_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(76,2,sys_para[k1_cc + eq_num] - 1);
            			}
            		  }
            		  else {
            			  if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))
            			  {
                			  uint8_t a = sys_para[k1_cc + eq_num];
                			  if(a > 0)a = enc_speed_dec(a,0);
                			  sys_para[k1_cc + eq_num] = a;
              				  if(!a)DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
              				  else DisplayTask->ParamIndicNum(76,2,a - 1);
            			  }
            			  if(sys_para[fs1 + eq_num] > 2)
            			  {
            				  int8_t a = sys_para[pr11 + eq_num * 4 + par_num2 - 1];
            				  a = enc_speed_dec(a,-1);
            				  if(a < 0)a = 98;
            				  sys_para[pr11 + eq_num * 4 + par_num2 - 1] = a++;
            				  str_temp[(par_num2 - 1) * 4] = ('0' + a/10) | 0x80;
            				  str_temp[(par_num2 - 1) * 4 + 1] = ('0' + a%10) | 0x80;
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1 + eq_num]-3]+(par_num2-1)*24,2,TDisplayTask::fntSystem,2,'0'+a/10);
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1 + eq_num]-3]+(par_num2-1)*24+6,2,TDisplayTask::fntSystem,2,'0'+a%10);
            			  }
            		  }
            		}
            	}
            	if(encoder_state == 2)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(sys_para[fs1 + eq_num] == 1)
            			{
            				if(!par_num2)
            				{
            					par_num2++;
            					DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            					tim5_start(0);
            				}
            			}
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            				if(par_num2 < (sys_para[fs1 + eq_num] - 2))
            				{
                    			str_temp[(par_num2 - 1) * 4] &= ~0x80;
                    			str_temp[(par_num2++ - 1) * 4 + 1] &= ~0x80;
                    			DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                    			str_temp[(par_num2 - 1) * 4] |= 0x80;
                    			str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
                    			DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            				}
            				tim5_start(0);
            			}
            		}
            		else {
            		  if(!par_num2)
            		  {
            			if(sys_para[fs1 + eq_num] < 6)sys_para[fs1 + eq_num]++;
            			if(sys_para[fs1 + eq_num] == 1)
            			{
            				//if(sys_para[fs11 + eq_num] == 1)sys_para[fs1 + eq_num]++;
            			}
            			DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs1 + eq_num] * 12);
            			DisplayTask->Clear_str(0,2,TDisplayTask::fntSystem,21);
            			kgp_sdk_libc::memset(str_temp,0,22);
            			if(sys_para[fs1 + eq_num] > 2)
            			{
            				uint8_t a = sys_para[fs1 + eq_num] - 3;
                			str_pres_init(str_temp , sys_para[pr11 + eq_num * 4] + 1);
                			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                			if(a)str_pres_init(str_temp + 4 , sys_para[pr11 + eq_num * 4 + 1] + 1);
                			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr11 + eq_num * 4 + 2] + 1);
                			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr11 + eq_num * 4 + 3] + 1);
                			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            			if(sys_para[fs1 + eq_num] == 1)
            			{
        					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        					if(!sys_para[k1_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(76,2,sys_para[k1_cc + eq_num] - 1);
            			}
            		  }
            		  else {
            			  if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))
            			  {
            				  uint8_t a = sys_para[k1_cc + eq_num];
            				  if(a < 128)a = enc_speed_inc(a,128);
            				  sys_para[k1_cc + eq_num] = a;
          					  if(!a)DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
          					  else DisplayTask->ParamIndicNum(76,2,a - 1);
            			  }
            			  if(sys_para[fs1 + eq_num] > 2)
            			  {
            				  uint8_t a = sys_para[pr11 + eq_num * 4 + par_num2 - 1];
            				  a = enc_speed_inc(a,98);
            				  if(a > 98)a = 0;
            				  sys_para[pr11 + eq_num * 4 + par_num2 - 1] = a++;
            				  str_temp[(par_num2 - 1) * 4] = ('0' + a/10) | 0x80;
            				  str_temp[(par_num2 - 1) * 4 + 1] = ('0' + a%10) | 0x80;
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1 + eq_num]-3]+(par_num2-1)*24,2,TDisplayTask::fntSystem,2,'0'+a/10);
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1 + eq_num]-3]+(par_num2-1)*24+6,2,TDisplayTask::fntSystem,2,'0'+a%10);
            			  }
            		  }
            		}
            	}
                clean_flag();
            }
            if(encoder_knob_pressed)
            {
            	if(encoder_knob_selected)
            	{
            		encoder_knob_selected = 0;
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
            			else {
            				str_temp[(par_num2 - 1) * 4] &= ~0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] &= ~0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	else {
            		encoder_knob_selected = 1;
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs1 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            			else {
            				str_temp[(par_num2 - 1) * 4] |= 0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs1 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	tim5_start(0);
                clean_flag();
            }
            if(k_up == 1)
            {
            	DisplayTask->Clear();
            	current_menu = MENU_SW_MODE;
            	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + sys_para[fsm1 + eq_num]*7);
            	if(sys_para[fsm1 + eq_num])
            	{
            		DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            		DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            	}
            	tim5_start(0);
                clean_flag();
                encoder_knob_selected = 0;
            }
        	break;
//-----------------------------------------------------SW sel1--------------------------------------------------
        case MENU_SW_SELECT1:
            if(!encoder_knob_selected)
              {
            	if(tim5_fl)
            	{
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            			if(sys_para[fs11 + eq_num] > 2)
            			{
            				str_temp[(par_num2 - 1) * 4] |= 0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	else {
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
            			if(sys_para[fs11 + eq_num] > 2)
            			{
            				str_temp[(par_num2 - 1) * 4] &= ~0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] &= ~0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
              }
            if(encoder_state_updated)
            {
            	if(encoder_state == 1)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(par_num2 > 0)
            			{
            				if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
                			str_temp[(par_num2 - 1) * 4] &= ~0x80;
                			str_temp[(par_num2-- - 1) * 4 + 1] &= ~0x80;
                			DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                			str_temp[(par_num2 - 1) * 4] |= 0x80;
                			str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
                			DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                			if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
                			tim5_start(0);
            			}
            		}
            		else {
            		  if(!par_num2)
            		  {
            			if(sys_para[fs11 + eq_num] > 0)sys_para[fs11 + eq_num]--;
            			if(sys_para[fs11 + eq_num] == 1)
            			{
            				//if(sys_para[fs1 + eq_num] == 1)sys_para[fs11 + eq_num]--;
            			}
            			DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs11 + eq_num] * 12);
            			DisplayTask->Clear_str(0,2,TDisplayTask::fntSystem,21);
            			kgp_sdk_libc::memset(str_temp,0,22);
            			if(sys_para[fs11 + eq_num] > 2)
            			{
            				uint8_t a = sys_para[fs11 + eq_num] - 3;
                			str_pres_init(str_temp , sys_para[pr111 + eq_num * 4] + 1);
                			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                			if(a)str_pres_init(str_temp + 4 , sys_para[pr111 + eq_num * 4 + 1] + 1);
                			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr111 + eq_num * 4 + 2] + 1);
                			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr111 + eq_num * 4 + 3] + 1);
                			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            			if(sys_para[fs11 + eq_num] == 1)
            			{
        					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        					if(!sys_para[k11_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(76,2,sys_para[k11_cc + eq_num] - 1);
            			}
            		  }
            		  else {
            			  if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))
            			  {
                			  uint8_t a = sys_para[k11_cc + eq_num];
                			  if(a > 0)a = enc_speed_dec(a,0);
                			  sys_para[k11_cc + eq_num] = a;
              				  if(!a)DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
              				  else DisplayTask->ParamIndicNum(76,2,a - 1);
            			  }
            			  if(sys_para[fs11 + eq_num] > 2)
            			  {
            				  int8_t a = sys_para[pr111 + eq_num * 4 + par_num2 - 1];
            				  a = enc_speed_dec(a,-1);
            				  if(a < 0)a = 98;
            				  sys_para[pr111 + eq_num * 4 + par_num2 - 1] = a++;
            				  str_temp[(par_num2 - 1) * 4] = ('0' + a/10) | 0x80;
            				  str_temp[(par_num2 - 1) * 4 + 1] = ('0' + a%10) | 0x80;
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11 + eq_num]-3]+(par_num2-1)*24,2,TDisplayTask::fntSystem,2,'0'+a/10);
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11 + eq_num]-3]+(par_num2-1)*24+6,2,TDisplayTask::fntSystem,2,'0'+a%10);
            			  }
            		  }
            		}
            	}
            	if(encoder_state == 2)
            	{
            		if(!encoder_knob_selected)
            		{
            			if(sys_para[fs11 + eq_num] == 1)
            			{
            				if(!par_num2)
            				{
            					par_num2++;
            					DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            					tim5_start(0);
            				}
            			}
            			if(sys_para[fs11 + eq_num] > 2)
            			{
            				if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            				if(par_num2 < (sys_para[fs11 + eq_num] - 2))
            				{
                    			str_temp[(par_num2 - 1) * 4] &= ~0x80;
                    			str_temp[(par_num2++ - 1) * 4 + 1] &= ~0x80;
                    			DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
                    			str_temp[(par_num2 - 1) * 4] |= 0x80;
                    			str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
                    			DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            				}
            				tim5_start(0);
            			}
            		}
            		else {
            		  if(!par_num2)
            		  {
            			if(sys_para[fs11 + eq_num] < 6)sys_para[fs11 + eq_num]++;
            			if(sys_para[fs11 + eq_num] == 1)
            			{
            				//if(sys_para[fs1 + eq_num] == 1)sys_para[fs11 + eq_num]++;
            			}
            			DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)fsw_t + sys_para[fs11 + eq_num] * 12);
            			DisplayTask->Clear_str(0,2,TDisplayTask::fntSystem,21);
            			kgp_sdk_libc::memset(str_temp,0,22);
            			if(sys_para[fs11 + eq_num] > 2)
            			{
            				uint8_t a = sys_para[fs11 + eq_num] - 3;
                			str_pres_init(str_temp , sys_para[pr111 + eq_num * 4] + 1);
                			if(a)kgp_sdk_libc::memcpy(str_temp + 2,(uint8_t*)"->",2);
                			if(a)str_pres_init(str_temp + 4 , sys_para[pr111 + eq_num * 4 + 1] + 1);
                			if(a > 1)kgp_sdk_libc::memcpy(str_temp + 6,(uint8_t*)"->",2);
                			if(a > 1)str_pres_init(str_temp + 8 , sys_para[pr111 + eq_num * 4 + 2] + 1);
                			if(a > 2)kgp_sdk_libc::memcpy(str_temp + 10,(uint8_t*)"->",2);
                			if(a > 2)str_pres_init(str_temp + 12 , sys_para[pr111 + eq_num * 4 + 3] + 1);
                			DisplayTask->StringOut(pres_num_pos[a],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            			if(sys_para[fs11 + eq_num] == 1)
            			{
        					DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        					if(!sys_para[k11_cc + eq_num])DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        					else DisplayTask->ParamIndicNum(76,2,sys_para[k11_cc + eq_num] - 1);
            			}
            		  }
            		  else {
            			  if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))
            			  {
            				  uint8_t a = sys_para[k11_cc + eq_num];
            				  if(a < 128)a = enc_speed_inc(a,128);
            				  sys_para[k11_cc + eq_num] = a;
          					  if(!a)DisplayTask->StringOut(76,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
          					  else DisplayTask->ParamIndicNum(76,2,a - 1);
            			  }
            			  if(sys_para[fs11 + eq_num] > 2)
            			  {
            				  uint8_t a = sys_para[pr111 + eq_num * 4 + par_num2 - 1];
            				  a = enc_speed_inc(a,98);
            				  if(a > 98)a = 0;
            				  sys_para[pr111 + eq_num * 4 + par_num2 - 1] = a++;
            				  str_temp[(par_num2 - 1) * 4] = ('0' + a/10) | 0x80;
            				  str_temp[(par_num2 - 1) * 4 + 1] = ('0' + a%10) | 0x80;
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11 + eq_num]-3]+(par_num2-1)*24,2,TDisplayTask::fntSystem,2,'0'+a/10);
            				  DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11 + eq_num]-3]+(par_num2-1)*24+6,2,TDisplayTask::fntSystem,2,'0'+a%10);
            			  }
            		  }
            		}
            	}
                clean_flag();
            }
            if(encoder_knob_pressed)
            {
            	if(encoder_knob_selected)
            	{
            		encoder_knob_selected = 0;
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
            			else {
            				str_temp[(par_num2 - 1) * 4] &= ~0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] &= ~0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	else {
            		encoder_knob_selected = 1;
            		if(!par_num2)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu);
            		else {
            			if((sys_para[fs11 + eq_num] == 1) && (par_num2 == 1))DisplayTask->StringOut(52,2,TDisplayTask::fntSystem,2,(uint8_t*)"CC#");
            			else {
            				str_temp[(par_num2 - 1) * 4] |= 0x80;
            				str_temp[(par_num2 - 1) * 4 + 1] |= 0x80;
            				DisplayTask->StringOut(pres_num_pos[sys_para[fs11 + eq_num] - 3],2,TDisplayTask::fntSystem,0,(uint8_t*)str_temp);
            			}
            		}
            	}
            	tim5_start(0);
                clean_flag();
            }
            if(k_up == 1)
            {
            	DisplayTask->Clear();
            	current_menu = MENU_SW_MODE;
            	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + sys_para[fsm1 + eq_num]*7);
            	if(sys_para[fsm1 + eq_num])
            	{
            		DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            		DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            	}
            	tim5_start(0);
                clean_flag();
                encoder_knob_selected = 0;
            }
        	break;
//----------------------------------------------------------------Default preset----------------------------------
        case MENU_ERASE:
        	if(par_num)DisplayTask->StringOut(78,0,TDisplayTask::fntSystem , tim5_fl*2 , (uint8_t*)"Yes");
        	else DisplayTask->StringOut(30,0,TDisplayTask::fntSystem , tim5_fl*2 , (uint8_t*)"No");
        	if(encoder_state_updated)
        	{
        		if(encoder_state == 1)
        		{
        			if(par_num)
        			{
        				par_num--;
        				DisplayTask->StringOut(30,0,TDisplayTask::fntSystem , 2 , (uint8_t*)"No");
        				DisplayTask->StringOut(78,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"Yes");
        			}
        		}
        		if(encoder_state == 2)
        		{
        			if(!par_num)
        			{
        				par_num++;
        				DisplayTask->StringOut(30,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"No");
        				DisplayTask->StringOut(78,0,TDisplayTask::fntSystem , 2 , (uint8_t*)"Yes");
        			}
        		}
            	tim5_start(0);
                clean_flag();
        	}
        	if(encoder_knob_pressed)
        	{
            	if(par_num)
            	{
            		preset_erase(prog);
            		kgp_sdk_libc::memcpy(prog_data,prog_data_init,512);
            		prog_data[delay_tim_lo] = 0xf4;
            		prog_data[delay_tim_hi] = 1;
            		cab_data_ready = false;
            		send_cab_data(1,prog + 1,0);
            		if(cab_num == 2)send_cab_data1(1,prog + 1);
            		prog_ch();
                    current_menu = MENU_MAIN;
                    DisplayTask->Menu_init();
            	}
            	else {
                    current_menu = MENU_MAIN;
                    DisplayTask->Menu_init();
            	}
            	tim5_start(0);
                clean_flag();
        	}
            if(k_up == 1)
              {
                current_menu = MENU_MAIN;
                DisplayTask->Menu_init();
                tim5_start(0);
                clean_flag();
              }
        	break;
//------------------------------------------------------MIDI Map----------------------------------------------
        case MENU_MIDI_PC:
            if(!encoder_knob_selected)
              {
                if(tim5_fl)DisplayTask->Clear_str(3,0,TDisplayTask::fntSystem,3);
                else DisplayTask->ParamIndicNum(3,0,eq_num + 1);
              }
            else {
                if(tim5_fl)DisplayTask->Clear_str(36,0,TDisplayTask::fntSystem,3);
                else DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
              }
        if(encoder_state_updated == 1)
        {
           if(encoder_state == 1)
           {
              if(encoder_knob_selected == 0)
              {
            	  if(eq_num > 0)
            	  {
            		  eq_num = enc_speed_dec(eq_num,0);
            		  DisplayTask->ParamIndicNum(3,0,eq_num + 1);
            		  DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
            	  }
              }
              else {
            	  if(sys_para[eq_num + 128] > 0)
            	  {
            		 sys_para[eq_num + 128] = enc_speed_dec(sys_para[eq_num + 128],0);
            		 DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
            	  }
              }
           }
           if(encoder_state == 2)
           {
               if(encoder_knob_selected == 0)
               {
             	  if(eq_num < 127)
             	  {
             		 eq_num = enc_speed_inc(eq_num,127);
             		 DisplayTask->ParamIndicNum(3,0,eq_num + 1);
             		 DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
             	  }
               }
               else {
            	  if(sys_para[eq_num + 128] < 98)
            	  {
            		 sys_para[eq_num + 128] = enc_speed_inc(sys_para[eq_num + 128],98);
            		 DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
            	  }
               }
           }
           tim5_start(1);
           clean_flag();
        }
        if(encoder_knob_pressed == 1)
        {
           if(encoder_knob_selected == 0)
           {
             encoder_knob_selected = 1;
             DisplayTask->ParamIndicNum(3,0,eq_num + 1);
           }
           else {
        	  DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
              encoder_knob_selected = 0;
           }
           tim5_start(0);
           clean_flag();
        }
        if(k_up == 1)
        {
   		    DisplayTask->Clear();
   		    for(uint8_t i = 0 ; i < 4 ; i++)
   		    {
   		    	DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + 48 + i*12);
   		    	if(i == 1)DisplayTask->StringOut(44,i,TDisplayTask::fntSystem,0,(uint8_t*)spdif_type + sys_para[spdif_t]*12);
   		    	if(i == 3)DisplayTask->StringOut(44,i,TDisplayTask::fntSystem,0,(uint8_t*)tempo_type + sys_para[tap_typ]*7);
   		    }
   	        current_menu = MENU_SYSTEM;
   	        DisplayTask->Icon_Strel(ICON_SY, STRELKA_UP);
   	        encoder_knob_selected = 0;
   	        tim5_start(0);
   	        clean_flag();
        }
        break;
//-----------------------------------------------------Tuner ext controller----------------------------
        case MENU_TUNER_EXT:
        if(encoder_state_updated == 1)
        {
        	uint8_t a = sys_para[tun_ext] & 0x7f;
        	if(encoder_state == 1)
        	{
        		if(a > 1)
        		{
        			a = enc_speed_dec(a,1);
        			DisplayTask->ParamIndicNum(50,0,a);
                	sys_para[tun_ext] = (sys_para[tun_ext] & 0x80) + a;
        		}
        	}
        	if(encoder_state == 2)
        	{
        		uint8_t a = sys_para[tun_ext] & 0x7f;
        		if(a < 127)
        		{
        			a = enc_speed_inc(a,128);
        			DisplayTask->ParamIndicNum(50,0,a);
                	sys_para[tun_ext] = (sys_para[tun_ext] & 0x80) + a;
        		}
        	}
            tim5_start(0);
            clean_flag();
        }
        if(k_up == 1)
         {
           current_menu = MENU_SYSTEM;
           DisplayTask->Clear();
           encoder_knob_selected = 0;
 		   DisplayTask->Icon_Strel(ICON_SY, STRELKA_UP);
 		   DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 1)*12);
 		   if(sys_para[tun_ext] & 0x80)DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off);
 		   else DisplayTask->StringOut(78,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_on_off + 5);
 		   DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 1)*12);
 		   DisplayTask->StringOut(39,1,TDisplayTask::fntSystem,0,(uint8_t*)time_type + sys_para[tim_type]*4);
 		   DisplayTask->StringOut(3,2,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 2)*12);
 		   if(!sys_para[swap_switch])DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
 		   else DisplayTask->StringOut(78,2,TDisplayTask::fntSystem,0,(uint8_t*)"On");
 		   DisplayTask->StringOut(3,3,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + (par_num + 3)*12);
 		   DisplayTask->ParamIndicNum(78,3,sys_para[speed_tun]);
           tim5_start(0);
           clean_flag();
         }
        break;
   }
   if(sys_para[tap_typ] == 2)
   {
   	 extern volatile uint16_t midi_clk_buf[];
   	 uint16_t mediann_tap( uint16_t* array, int length);
   	 uint16_t c = mediann_tap((uint16_t*)midi_clk_buf , 64);
   	 c >>= 4;
   	 if((c < (midi_clk_send - 7)) || (c > (midi_clk_send + 7)))
     {
   		 midi_clk_send = c;
   		 void send_midi_temp(uint16_t* c);
   		 send_midi_temp(&c);
   	 }
   }
}

uint8_t tap_temp_global(void)
{
	uint8_t a = 0;
	if (tap_temp <= 131070)
	{
		tap_global = tap_temp >> 4;
		if(sys_para[tap_typ]) // global temp On
		{
			sys_para[tap_hi] = (tap_global / 3) >> 8;
			gui_send(33,tap_global / 3);
			moog_time = tap_global / 3.0f;
			gui_send(31,13);
			delay_time = tap_global / 3.0f / tap_tim_v[prog_data[d_tap_t]];
			if(delay_time < 2731)gui_send(3,1);
			trem_time = tap_global / 3.0f / tap_tim_v[prog_data[t_tap_t]];
			if(trem_time < 2731)gui_send(10,5);
		}
		a = 1;
	}
	tap_temp = 0;
	return a;
}
//---------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler()
{
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  if(tim5_fl)tim5_fl = 0;
  else tim5_fl = 1;
  if(!tuner_use)CSTask->Give();
}
