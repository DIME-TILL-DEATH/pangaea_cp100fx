#include "appdefs.h"
#include "gu.h"
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

uint16_t midi_clk_send;
uint8_t num_men = 0;
uint8_t num_men_temp = 0;
volatile uint8_t prog2;
volatile uint8_t comp_fl = 0;
uint8_t tim5_fl2;
volatile uint8_t edit_fl1 = 0;
volatile uint8_t par_num = 0;
volatile uint8_t par_num1 = 0;
volatile uint8_t par_num2 = 0;
volatile uint8_t prog_old;
volatile uint8_t calib_fl = 0;
uint8_t condish = 0;
uint8_t copy_temp = 0;

extern uint8_t k_up;
extern uint8_t k_down;
extern uint8_t k_att;
extern uint8_t cab_type;
extern uint8_t k_master_eq;
extern uint8_t name_run_fl;
extern uint8_t name_temp_po;
extern uint32_t name_temp_po_time;
extern volatile uint8_t prog_sym_cur;
extern volatile uint8_t master_volume_controller;

volatile uint8_t enc_knob_fl;
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

inline uint8_t tap_temp_global(void);

const uint8_t ef_list       [][8] ={"Cab Sim","Volume ","    Eq ","Phaser ","Chorus "," Delay "," Early ","Reverb "," Name  "," Write ","Write ?"};
const uint8_t phas_list     [][8] ={"Mix","Rate","Center","Width","F_Back","Stage","HPF","Pos"};
const uint8_t chor_list     [][8] ={"Mix","Rate","Width","Delay","Type","HPF"};
const uint8_t chor_list1    [][8] ={"Mix","Detune"," --"," --","Type","HPF"};
const uint8_t chor_list2    [][8] ={"Mix"," --"," --","Delay","Type","HPF"};
const uint8_t fl_list       [][8] ={"Mix","LFO","Rate","Width","Delay","F_Back","HPF","Pos"};
const uint8_t fl_t          [][9] ={"Triangle","Sinus   "};
const uint8_t ch_list_t     [][12]={"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"};
const uint8_t del_list      [][8] ={"Mix","Time","F_Back","LPF","HPF","D_Pan","D2 Vol","D2 Pan","D->D2","D_Mod","M_Rate","Direct"};
const uint8_t del_tim_l     [][5] ={"Time","TAP","Tail"};
const uint8_t tap_tim       [][6] ={"1/1  ","1/1.5","1/2  ","1/3  ","1/4  ","2/1  "};
const float   tap_tim_v     [6]   ={1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};
const uint8_t rev_list      [][7] ={"Mix","Type","Time","Size","Damp","LPF","HPF","Detune","Diffus","PreD","Tail"};
const uint8_t rev_type_list [][8] ={"Default","Hall   ","Room   ","Plate  ","Spring ","Gate   ","Reverse"};
const uint8_t ear_list      [][5] ={"Mix","Size"};
const uint8_t s_t_c_list    [][7] ={"  No  ","Return","   Yes"};
const uint8_t tr_list       [][8] ={"Intens","Rate","LFOtype","LFO mod","Type","TAP"};
const uint8_t tr_t_list     [][7] ={"Sin   ","Square","Mono  ","Stereo"};
const uint8_t tr_lfo_t_list [][9] ={"Sin     ","Square  ","Sawtooth"};
const uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};
const uint8_t cab_list      [][8] ={"Pan","Browser","Volume"};
const uint8_t cab_out_list  [][6] ={"1 L+R","1R AP","2 L+R","1R A ","1R P "," 1 R "};
const uint8_t cab_list_menu [][9] ={"Cabinet1","Cabinet2"};
const uint8_t lpf_hpf       [][9] ={"HPF","LPF","Presence","Position"};
const uint8_t eq_pre_post   [][5] ={"Post","Pre "};
const uint8_t del_dir       [][8] ={"Forward","Reverse"};
const uint8_t amp_para      [][9] ={"Volume","Presence","Slave","Type"};
const uint8_t pre_para      [][7] ={"Gain","Volume"," ","Low","Mid","High"};
const uint8_t mas_eq_list   [][9] ={"Low","Mid","Mid Freq","High"};
const uint8_t gate_list     [][7] ={"Thresh","Attack","Decay"};
const uint8_t comp_list     [][8] ={"Thresh","Ratio","Volume","Attack","Release"};
const uint8_t switch_mod    [][7] ={"Single","Double"};
const uint8_t switch_slow   [][10]={"OnRelease","OnHold"};
const uint8_t out_pc_list   [][8] ={"MIDI IN","  MAP  ","  SET  "};
const uint8_t moog_list     [][8] ={"Mix","F Type","F Mod","LFO r","Lo freq","Hi freq","Res","Dyn th","Dyn att","Dyn rel","Volume","LFO_Typ"};
const uint8_t moog_typ_list [][4] ={"LPF","HPF","BPF"};
const uint8_t moog_mod_list [][4] ={"LFO","Dyn","Ext"};
const uint8_t moog_gen_type [][7] ={"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"};
const uint8_t ascii_low1     []= " abcdefghijklmnopqrst";
const uint8_t ascii_low2     []= "uvwxyz0123456789!@#$%";
const uint8_t ascii_hig1     []= "ABCDEFGHIJKLMNOPQRSTU";
const uint8_t ascii_hig2     []= "VWXYZ{}()-+_=<>?*.,/&";
const uint8_t att_db [][4] = {" +4"," +3"," +2"," +1","  0"," -1"," -2"," -3"," -4"," -5"," -6"," -7"," -8"," -9","-10","-11",
"-12","-13","-14","-15","-16","-17","-18","-19","-20","-21","-23","-24","-27","-30","-34","-35","-36","-37",
"-38","-40","-41","-42","-43","-44","-45","-46","-47","-48","-49","-50","-51","-52","-53","-54","-55","-56",
"-57","-58","-59","-60"};
const uint16_t att_db_num [56] = {510,459,421,390,368,344,331,320,311,303,297,291,287,283,279,277,274,272,269,268,
266,264,263,262,261,260,259,258,257,256,253,249,227,207,182,144,128,105,100,89,80,70,62,55,48,43,38,34,30,27,24,22,19,17,15,13};
const uint8_t amp_t_list[][11] = {"  PP 6L6  ","  PP EL34 ","  SE 6L6  ","  SE EL34 ",
								  " AMT TC-3 ","California","British M ","British L "," Default  ","Calif mod ","Calif vint",
                                  "PVH PR0RS0","PVH PR5RS5","PVH PR8RS7","PVH PR9RS8","PA Modern ","PP Amp 6L6","PP AmpEL34",
								  "SE Amp 6L6","PVH IC V1 ","PVH IC V2 ","PVH PR00  ","PVH PR98  "};
const uint8_t pres_num_pos[4] = {58,46,37,28};
const uint8_t def_eq_band [][10]={"F 120Hz ","F 360Hz ","F 800Hz ","F 2000Hz","F 6000Hz"};

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
const uint8_t in_outp [][7] = {"Input ","Output"};
const uint8_t sd_nr [] = "MicroSD is not ready";
const uint8_t imp_dir_n [] = "There is no directory";
const uint8_t imp_dir_no [ ]= "IMPULSE";
const uint8_t sd_lo [] = "MicroSD is loading..";
const uint8_t dw1[][6] = {"D<--W","D-->W","D<->W"};
const uint8_t atten[] = {"Attenuator"};
const uint8_t decib[] = "dB";
const uint8_t gerz[] = "Hz";
const uint8_t master_vo[][14] = {"Master Volume","Phones Volume"};
const uint8_t eq_p_l[][2] = {"F","Q"};
const uint8_t master_eq_of[] = {"Master EQ Bypass"};
const uint8_t master_eq_on[] = {"EQ on/off press EDIT"};
const uint8_t mode_[] = "Mode";

void str_pres_init(char* adr , uint16_t val)
{
	if(val < 10)ksprintf( adr , "0%d" , val);
	else ksprintf( adr , "%d" , val);
}
void tim5_start(uint8_t val)
{
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    TIM_SetCounter(TIM4,0xffff);
    tim5_fl = val;
}
void t_restart(void)
{
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  TIM_SetCounter(TIM4,0);
  tim5_fl = 1;
  CSTask->Give();
}
void clean_flag (void)
{
	k_up = k_down = k_sys = k_att = k_master = k_master_eq = encoder_knob =
	encoder_state1 = tuner_flag = 0;
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
void icon_refresh(uint8_t num)
{
	switch (num){
	case 0:DisplayTask->Ef_icon(2,0,(uint8_t*)rf,prog_data[moog]);break;
	case 1:DisplayTask->Ef_icon(20,0,(uint8_t*)gt,prog_data[gate]);break;
	case 2:DisplayTask->Ef_icon(38,0,(uint8_t*)cm,prog_data[compr]);break;
	case 3:DisplayTask->Ef_icon(56,0,(uint8_t*)pr,prog_data[pream]);break;
	case 4:DisplayTask->Ef_icon(74,0,(uint8_t*)am,prog_data[amp]);break;
	case 5:DisplayTask->Ef_icon(92,0,(uint8_t*)cs,prog_data[cab]);break;
	case 6:DisplayTask->Ef_icon(110,0,(uint8_t*)equ,prog_data[eq]);break;
	case 7:DisplayTask->Ef_icon(2,2,(uint8_t*)ph,prog_data[phas]);break;
	case 8:DisplayTask->Ef_icon(20,2,(uint8_t*)rm,prog_data[fl]);break;
	case 9:DisplayTask->Ef_icon(38,2,(uint8_t*)ch,prog_data[chor]);break;
	case 10:DisplayTask->Ef_icon(56,2,(uint8_t*)dl,prog_data[deley]);break;
	case 11:DisplayTask->Ef_icon(74,2,(uint8_t*)er,prog_data[early]);break;
	case 12:DisplayTask->Ef_icon(92,2,(uint8_t*)rv,prog_data[reve]);break;
	case 13:DisplayTask->Ef_icon(110,2,(uint8_t*)tr,prog_data[trem]);break;
	}
}

volatile uint16_t dsaf = 0x8000;
//---------------------------------------------------------GUI----------------------------------------------------
void gui (void)
{
      switch (condish){
//---------------------------------------------------------------------------------Program change------------------------------------------
      case menu_main:
    	if(prog1 != prog)
          {
           if(tim5_fl == 0)
           {
        	   DisplayTask->Prog_ind(prog1);
        	   if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
           }
           else {
        	   if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)
        	   {
        		   DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
        		   if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
        	   }
           }
          }
        if(edit_fl){
        switch (num_men){
        case 0:
          if(tim5_fl == 0)DisplayTask->Ef_icon(2,0,(uint8_t*)rf,prog_data[moog]);
          else DisplayTask->Ef_icon(2,0,(uint8_t*)rf,2);
          break;
        case 1:
          if(tim5_fl == 0)DisplayTask->Ef_icon(20,0,(uint8_t*)gt,prog_data[gate]);
          else DisplayTask->Ef_icon(20,0,(uint8_t*)gt,2);
          break;
        case 2:
          if(tim5_fl == 0)DisplayTask->Ef_icon(38,0,(uint8_t*)cm,prog_data[compr]);
          else DisplayTask->Ef_icon(38,0,(uint8_t*)cm,2);
          break;
        case 3:
          if(tim5_fl == 0)DisplayTask->Ef_icon(56,0,(uint8_t*)pr,prog_data[pream]);
          else DisplayTask->Ef_icon(56,0,(uint8_t*)pr,2);
          break;
        case 4:
          if(tim5_fl == 0)DisplayTask->Ef_icon(74,0,(uint8_t*)am,prog_data[amp]);
          else DisplayTask->Ef_icon(74,0,(uint8_t*)am,2);
          break;
        case 5:
          if(tim5_fl == 0)DisplayTask->Ef_icon(92,0,(uint8_t*)cs,prog_data[cab]);
          else DisplayTask->Ef_icon(92,0,(uint8_t*)cs,2);
          break;
        case 6:
          if(tim5_fl == 0)DisplayTask->Ef_icon(110,0,(uint8_t*)equ,prog_data[eq]);
          else DisplayTask->Ef_icon(110,0,(uint8_t*)equ,2);
          break;
        case 7:
          if(tim5_fl == 0)DisplayTask->Ef_icon(2,2,(uint8_t*)ph,prog_data[phas]);
          else DisplayTask->Ef_icon(2,2,(uint8_t*)ph,2);
          break;
        case 8:
          if(tim5_fl == 0)DisplayTask->Ef_icon(20,2,(uint8_t*)rm,prog_data[fl]);
          else DisplayTask->Ef_icon(20,2,(uint8_t*)rm,2);
          break;
        case 9:
          if(tim5_fl == 0)DisplayTask->Ef_icon(38,2,(uint8_t*)ch,prog_data[chor]);
          else DisplayTask->Ef_icon(38,2,(uint8_t*)ch,2);
          break;
        case 10:
          if(tim5_fl == 0)DisplayTask->Ef_icon(56,2,(uint8_t*)dl,prog_data[deley]);
          else DisplayTask->Ef_icon(56,2,(uint8_t*)dl,2);
          break;
        case 11:
          if(tim5_fl == 0)DisplayTask->Ef_icon(74,2,(uint8_t*)er,prog_data[early]);
          else DisplayTask->Ef_icon(74,2,(uint8_t*)er,2);
          break;
        case 12:
          if(tim5_fl == 0)DisplayTask->Ef_icon(92,2,(uint8_t*)rv,prog_data[reve]);
          else DisplayTask->Ef_icon(92,2,(uint8_t*)rv,2);
          break;
        case 13:
          if(tim5_fl == 0)DisplayTask->Ef_icon(110,2,(uint8_t*)tr,prog_data[trem]);
          else DisplayTask->Ef_icon(110,2,(uint8_t*)tr,2);
          break;
        }
      }
//--------------------------------------------------------------------
        if(encoder_state1 == 1)
          {
            if((encoder_state == 1) && (comp_fl == 0))
              {
                if(edit_fl == 0)
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
                else {
                	icon_refresh(num_men);
                    if(num_men > 0)num_men--;
                    else num_men = 13;
                    //if(sys_para[0] != 0)if(num_men == 9)num_men--;
                    tim5_start(0);
                }
              }
            if((encoder_state == 2) && (comp_fl == 0))
              {
                if(edit_fl == 0)
                  {
                    if(prog1 == 98)prog1 = 0;
                    else prog1 += 1;
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
                else {
                	icon_refresh(num_men);
                	if(num_men < 13)num_men++;
                	else num_men = 0;
                    //if(sys_para[0] != 0)if(num_men == 9)num_men++;
                    tim5_start(0);
                }
              }
            clean_flag();
          }
//------------------------------------------------------------------------
        if(encoder_knob == 1)
          {
           if(edit_fl == 1)
           {
            switch (num_men){
            case 0:
              if(prog_data[moog] == 0)prog_data[moog] = 1;
              else prog_data[moog] = 0;
              gui_send(18, 13 | (prog_data[moog] << 8));
              break;
            case 1:
              if(prog_data[gate] == 0)prog_data[gate] = 1;
              else prog_data[gate] = 0;
              gui_send(18, 11 | (prog_data[gate] << 8));
              break;
            case 2:
              if(prog_data[compr] == 0)prog_data[compr] = 1;
              else prog_data[compr] = 0;
              gui_send(18, 12 | (prog_data[compr] << 8));
              break;
            case 3:
              if(prog_data[pream] == 0)prog_data[pream] = 1;
              else prog_data[pream] = 0;
              gui_send(18, 10 | (prog_data[pream] << 8));
              break;
            case 4:
              if(prog_data[amp] == 0)prog_data[amp] = 1;
              else prog_data[amp] = 0;
              gui_send(18, 0 | (prog_data[amp] << 8));
              break;
            case 5:
              if(prog_data[cab] == 0)
              {
            	  prog_data[cab] = 1;
                  if(name_buf[0] == 0)
                  {
                	  kgp_sdk_libc::memset(preset_temp,0,24576);
                	  kgp_sdk_libc::memset(name_buf_temp,0,64);
                	  cab_num = 0;
            		  extern uint8_t sd_init_fl;
            		  if(sd_init_fl == 1)
            		  {
            			  condish = cabinet_browser;
            			  DisplayTask->Clear();
            			  extern volatile uint8_t imp_dir_fl;
            			  if(imp_dir_fl)
            			  {
                              vol_vol = prog_data[vol];
                              gui_send(7,0);
                              gui_send(15,1);
                              ind_poin = 500;
                              vol_fl = 1;
                              inp_ind_fl = 0;
                              out_ind_fl = 1;
            				  FSTask->SendCommand( TFsBrowser::bcCurrent );
            				  FSTask->SendCommand( TFsBrowser::bcLoadImp );
            			  }
            			  else {
                			  DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_n);
                			  DisplayTask->StringOut(42,3,TDisplayTask::fntSystem,0,(uint8_t*)imp_dir_no);
                			  CSTask->CS_del(1000);
                			  prog_data[cab] = 0;
                			  condish = menu_main;
                	          enc_knob_fl = 0;
                	          DisplayTask->Menu_init();
            			  }
            		  }
            		  else {
            			  DisplayTask->Clear();
            			  if(!sd_init_fl)DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_nr);
            			  else DisplayTask->StringOut(0,1,TDisplayTask::fntSystem,0,(uint8_t*)sd_lo);
            			  CSTask->CS_del(1000);
            			  prog_data[cab] = 0;
            			  condish = menu_main;
            	          enc_knob_fl = 0;
            	          DisplayTask->Menu_init();
            		  }
                  }
              }
              else prog_data[cab] = 0;
              gui_send(18, 1 | (prog_data[cab] << 8));
              break;
            case 6:
              if(prog_data[eq] == 0)prog_data[eq] = 1;
              else prog_data[eq] = 0;
              gui_send(18, 2 | (prog_data[eq] << 8));
              break;
            case 7:
              if(prog_data[phas] == 0)prog_data[phas] = 1;
              else prog_data[phas] = 0;
              gui_send(18, 4 | (prog_data[phas] << 8));
              break;
            case 8:
              if(prog_data[fl] == 0)prog_data[fl] = 1;
              else prog_data[fl] = 0;
              gui_send(18, 5 | (prog_data[fl] << 8));
              break;
            case 9:
              if(prog_data[chor] == 0)prog_data[chor] = 1;
              else prog_data[chor] = 0;
              gui_send(18, 6 | (prog_data[chor] << 8));
              break;
            case 10:
              if(prog_data[deley] == 0)prog_data[deley] = 1;
              else prog_data[deley] = 0;
              gui_send(18, 3 | (prog_data[deley] << 8));
              break;
            case 11:
              if(prog_data[early] == 0)prog_data[early] = 1;
              else prog_data[early] = 0;
              gui_send(18, 7 | (prog_data[early] << 8));
              break;
            case 12:
              if(prog_data[reve] == 0)prog_data[reve] = 1;
              else prog_data[reve] = 0;
              gui_send(18, 8 | (prog_data[reve] << 8));
              break;
            case 13:
              if(prog_data[trem] == 0)prog_data[trem] = 1;
              else prog_data[trem] = 0;
              gui_send(18, 9 | (prog_data[trem] << 8));
              break;
            }
           clean_flag();
           tim5_start(1);
          }
         else {

            prog = prog1;
		    eepr_read_imya(prog1);
		    imya_temp = 1;
		    DisplayTask->Main_scr();
            DisplayTask->Prog_ind(prog1);
            sys_para[31] = prog;
            prog_ch();
            num_men = 0;
           }
           clean_flag();
           tim5_start(1);
         }
//------------------------------------------------------------------------------
        if((k_down == 1) && (comp_fl == 0) && (prog1 == prog))
          {
            if(edit_fl == 1){
            switch (num_men){
            case 0:
              if(prog_data[moog] == 1){
              condish = moog_menu;
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(15,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + i*8);
            	  switch(i){
            	  case 0:DisplayTask->ParamIndicMix(53,i,prog_data[mog_vol]);break;
            	  case 1:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_typ_list + prog_data[mog_type]*4);break;
            	  case 2:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_mod_list + prog_data[mog_mod]*4);break;
            	  case 3:DisplayTask->ParamIndic(53,i,prog_data[mog_rate]);break;
            	  }
                }
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 1:
              if(prog_data[gate] == 1){
              condish = gate_menu;
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(14,0);
              for(uint8_t i = 0 ; i < 3 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)gate_list + i*7);
            	  DisplayTask->ParamIndic(53,i,prog_data[gate_thr + i]);
                }
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 2:
              if(prog_data[compr] == 1){
              condish = compr_menu;
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(13,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)comp_list + i*8);
                  DisplayTask->ParamIndic(53,i,prog_data[comp_thr + i]);
                }
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 3:
              if(prog_data[pream] == 1){
              condish = pre_menu;
              gui_send(15,8);
              DisplayTask->Clear();
      		  DisplayTask->Icon_Strel(12,2);
              for(uint8_t i = 0 ; i < 2 ; i++)
              {
            	  DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para + i*7);
            	  DisplayTask->ParamIndic(58,i,prog_data[pre_gain + i]);
              }
          	  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
          	  inp_ind_fl = 0;
          	  out_ind_fl = 1;
          	  vol_fl = 0;
              ind_poin = 500;
              par_num = 0;
             }
              tim5_start(0);
              clean_flag();
             break;
            case 4:
              if(prog_data[amp] == 1){
              condish = power_amplif;
              gui_send(15,2);
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(8,2);
              for(uint8_t i = 0 ; i < 3 ; i++)
              {
            	  DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)amp_para + i*9);
            	  if(i == 1)DisplayTask->ParamIndic(58,i,prog_data[pre_v]);
            	  else DisplayTask->ParamIndic(58,i,prog_data[am_v + i]);
              }
          	  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
          	  inp_ind_fl = 0;
          	  out_ind_fl = 1;
          	  vol_fl = 0;
              ind_poin = 500;
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 5:
                if(prog_data[cab])
          	    {
                 DisplayTask->Clear();
                 if(cab_type != 2)
                 {
                   name_run_fl = 1;
                   DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                   DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                   DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                   vol_vol = prog_data[vol];
                   gui_send(7,0);
                   condish = cabinet_sim;
                   gui_send(15,1);
                   ind_poin = 500;
                   vol_fl = 1;
                   inp_ind_fl = 0;
                   out_ind_fl = 1;
                   par_num = 1;
                 }
                 else {
              	   DisplayTask->StringOut(6,0,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu);
              	   DisplayTask->StringOut(6,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu + 9);
              	   condish = cabinet_type;
              	   par_num = 0;
                 }
               }
                tim5_start(0);
                clean_flag();
              break;
            case 6:
              if(prog_data[eq] == 1)
              {
               condish = equaliser;
               eq_num = 0;
               DisplayTask->EqInit();
               DisplayTask->Icon_Strel(4,2);
              }
              tim5_start(0);
              clean_flag();
              break;
            case 7:
              if(prog_data[phas] == 1){
              condish = phazer;
              gui_send(15,7);
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(9,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)phas_list + i*8);
                  if(i)DisplayTask->ParamIndic(53,i,prog_data[phaser_vol + i]);
                  else DisplayTask->ParamIndicMix(53,0,prog_data[phaser_vol]);
                }
              par_num = 0;
             }
              tim5_start(0);
              clean_flag();
              break;
            case 8:
                if(prog_data[fl] == 1){
                condish = flanger;
                gui_send(15,4);
                DisplayTask->Clear();
                DisplayTask->Icon_Strel(10,2);
                for(uint8_t i = 0 ; i < 4 ; i++)
                  {
              	    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)fl_list + i*8);
                    if(!i)DisplayTask->ParamIndicMix(53,0,prog_data[fl_v]);
                    else {
                    	if(i == 1)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem,0,(uint8_t*)fl_t + prog_data[fl_lfo]*9);
                    	else DisplayTask->ParamIndic(53,i,prog_data[fl_v + i]);
                    }
                  }
                par_num = 0;
               }
              tim5_start(0);
              clean_flag();
              break;
            case 9:
              if(prog_data[chor] == 1){
              condish = chorus;
              gui_send(15,5);
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(5,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list + i*8);
            	  else {
            		  if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list1 + i*8);
            		  else DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list2 + i*8);
            	  }
            	  if(i)
            	  {
            		  if(prog_data[chor_typ] < 4)DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
            		  else {
            			  if((i == 1) && (prog_data[chor_typ] == 4))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
            			  else if((i == 3) && (prog_data[chor_typ] == 5))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
            		  }
            	  }
                  else DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);
                }
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 10:
              if(prog_data[deley] == 1){
              condish = diley;
              DisplayTask->Clear();
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + i*8);
            	  if(i)
            	  {
            		  if(i > 1)DisplayTask->ParamIndic(53,i,prog_data[d_vol + i - 1]);
            		  else {
            			  if(!sys_para[tim_type])DisplayTask->DelayTimeInd(53,i,delay_time);
            			  else {
            				  DisplayTask->ParamIndicNum(53,i,60000/delay_time);
            				  DisplayTask->StringOut(90,i,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
            			  }
            		  }
            	  }
            	  else DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);
                }
              DisplayTask->Icon_Strel(0,2);
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            case 11:
              if(prog_data[early] == 1){
              condish = early_menu;
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(3,0);
              for(uint8_t i = 0 ; i < 2 ; i++)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)ear_list + i*5);
              DisplayTask->ParamIndicMix(53,0,prog_data[early_vol]);
              DisplayTask->ParamIndic(53,1,prog_data[early_vol + 1]);
              par_num = 0;
             }
              tim5_start(0);
              clean_flag();
              break;
            case 12:
              if(prog_data[reve] == 1){
              condish = rever;
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(2,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
            	  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_list + i*7);
                  if(i != 1)
                  {
                	if(i)
                	{
                		DisplayTask->ParamIndic(53,i,prog_data[r_vol + i - 1]);
                		if(i == 3)
                		{
                			if(prog_data[rev_t] == 4)DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                		}
                		else {
                			if(i == 2)
                			{
                				if(prog_data[rev_t] > 4)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                			}
                		}
                	}
                    else DisplayTask->ParamIndicMix(53,0,prog_data[r_vol]);
                  }
                  else  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_type_list + prog_data[rev_t] * 8);
                }
              par_num = 0;
             }
              tim5_start(0);
              clean_flag();
              break;
            case 13:
              if(prog_data[trem] == 1){
              condish = tremolo;
              gui_send(15,6);
              DisplayTask->Clear();
              DisplayTask->Icon_Strel(1,2);
              for(uint8_t i = 0 ; i < 4 ; i++)
                {
                  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_list + i*8);
                  if(i == 2)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_lfo_t_list + prog_data[tr_lfo_t] * 9);
                  else {
                	  if(i < 3)DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i]);
                	  else DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i - 1]);
                  }
                }
              par_num = 0;
              }
              tim5_start(0);
              clean_flag();
              break;
            }
          }
         else {
        	eq_num = prog;
            edit_fl = 1;
            edit_fl1 = 1;
            prog_cur = 1;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
           }
         }
//-----------------------------------------------------------------------------------------------------------
        if((k_up == 1)  && (comp_fl == 0))
        {
          if(edit_fl == 1)
            {
              if(edit_fl1 == 1)
                {
                  condish = save_pres;
                  DisplayTask->Clear();
                  DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)s_to_ch);
                  for(uint8_t i = 0 ; i < 3 ; i++)DisplayTask->StringOut(i*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + i*7);
                  par_num = 2;
                  tim5_start(1);
                }
              else {
                  vol_fl = 0;
                  edit_fl = 0;
                  prog_cur = 0;
                  num_men = 0;
                  gui_send(15,0);
                  DisplayTask->Menu_init();
                  tim5_start(0);
              }
            }
          else {
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
        	  else {
        		  if((!prog_sym_cur) && prog_data[cab])
        		  {
        			  DisplayTask->Clear();
        			  condish = cab_name;
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
        			  else {
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
          }
          clean_flag();
        }
//-------------------------------------------------------------------------------------------------------------
        if(tuner_flag)
        {
        	clean_flag();
        	if(!edit_fl)
        	{
        		send_codec(0xa102);
        		gui_send(13,0);
        		tun_base_old = 0.0f;
        		tun_ini();
        		tuner_use = 1;
        		condish = tuner_menu;
        		tim5_start(0);
        		gui_send(15,9);
        	}
        	else {
        		condish = copy_men;
        		read_prog_temp(copy_temp);
                imya_temp = 1;
                for(uint8_t i = 0 ; i < 15 ; i++)imya_t[i] = preset_temp[i];
                for(uint8_t i = 0 ; i < 15 ; i++)imya1_t[i] = preset_temp[15 + i];
                DisplayTask->Main_scr();
                DisplayTask->Prog_ind(copy_temp);
        		DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");
        	}
        }
        if(k_sys)
        {
        	clean_flag();
        	if(!edit_fl)
        	{
        		temp_sys = sys_para[2];
        		DisplayTask->Clear();
                condish = sys_menu;
                DisplayTask->Clear();
                DisplayTask->Icon_Strel(11,2);
                for(uint8_t i = 0 ; i < 4 ; i++)
                {
                	if(!i)DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + i*12);
                	else DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + i*12);
                	switch(i){
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
        	else {
        		sub_men = 3;
            	DisplayTask->Clear();
            	condish = name_edit;
            	key_shift = 0;
            	eq_num = 0;
            	par_num = 0;
            	nam_sym_temp = 32;
            	DisplayTask->StringOut(2,0,TDisplayTask::fntSystem,0,(uint8_t*)imya);
            	DisplayTask->StringOut(2,1,TDisplayTask::fntSystem,0,(uint8_t*)imya1);
            	DisplayTask->SymbolOut(86,0,TDisplayTask::fntSystem,0,46);
            	DisplayTask->SymbolOut(86,1,TDisplayTask::fntSystem,0,46);
            	DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low1);
            	DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low2);
            	num_men_temp = num_men;
        	}
        }
        if(k_att)
        {
        	clean_flag();
        	if(!edit_fl)
        	{
        		DisplayTask->Clear();
        		condish = att_menu;
        		DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)atten);
        		DisplayTask->StringOut(76,0,TDisplayTask::fntSystem,0,(uint8_t*)att_db + sys_para[127]*4);
        		DisplayTask->StringOut(95,0,TDisplayTask::fntSystem,0,(uint8_t*)decib);
        		DisplayTask->StringOut(2,2,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp);
        		edit_fl = 1;
                ind_poin = 500;
                inp_ind_fl = 1;
                out_ind_fl = 0;
                tim5_start(0);
                gui_send(15,9);
        	}
        	else {
        		condish = erase_menu;
                par_num = 0;
                tim5_start(0);
        		DisplayTask->Clear();
        		DisplayTask->StringOut(36,2,TDisplayTask::fntSystem,0,(uint8_t*)"Preset");
        		DisplayTask->StringOut(12,3,TDisplayTask::fntSystem,0,(uint8_t*)"Default Settings?");
        		DisplayTask->StringOut(30,0,TDisplayTask::fntSystem,0,(uint8_t*)"No      Yes");
        	}
        }
        if(k_master)
        {
        	clean_flag();
        	if(!edit_fl)
        	{
      		DisplayTask->Clear();
        		condish = master_v;
        		DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)master_vo);
        		DisplayTask->ParamIndicNum(85,0,sys_para[126]);
        		DisplayTask->StringOut(3,1,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + 14);
        		DisplayTask->ParamIndicNum(85,1,sys_para[125]);
        		par_num = 0;
        		tim5_start(0);
        	}
        	else {
        		sub_men = 1;
            	condish = volume;
            	DisplayTask->Clear();
            	DisplayTask->StringOut(28,0,TDisplayTask::fntSystem , 0 , (uint8_t*)volum);
            	DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
            	if(prog_data[vol_contr])DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Off");
            	else DisplayTask->StringOut(83,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"On ");
            	DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
            	DisplayTask->ParamIndicNum(42,3,prog_data[pres_lev]);
            	vol_vol = prog_data[pres_lev];
            	gui_send(15,0);
            	inp_ind_fl = 0;
            	out_ind_fl = 1;
                ind_poin = 500;
                vol_fl = 1;
                par_num = 0;
                tim5_start(0);
                num_men_temp = num_men;
        	}
        }
        if(k_master_eq)
        {
        	clean_flag();
        	if(!edit_fl)
        	{
        		DisplayTask->Clear();
        		condish = master_eq;
        		if(!sys_para[120])
        		{
        			DisplayTask->StringOut(12,1,TDisplayTask::fntSystem,0,(uint8_t*)master_eq_of);
        			DisplayTask->StringOut(1,3,TDisplayTask::fntSystem,0,(uint8_t*)master_eq_on);
        		}
        		else {
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
        	else {
        		sub_men = 2;
                condish = controllers;
                control_destin = dest_tabl_start[control[1]];
                contrs = 0;
                DisplayTask->Clear();
                for(uint8_t i = 0 ; i < 3 ; i++)
                  {
                    DisplayTask->StringOut(0,i,TDisplayTask::fntSystem , 0 , (uint8_t*)contr_list + i*7);
                    switch (i){
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
                num_men_temp = num_men;
        	}
        }
        break;

//---------------------------------------------------------------------Menu Save change-----------------------------------------------------
      case save_pres:
    	if(tim5_fl == 0)DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,0,(uint8_t*)s_t_c_list + par_num*7);
        else DisplayTask->StringOut(par_num*42,0,TDisplayTask::fntSystem,3,(uint8_t*)s_t_c_list + par_num*7);
        if(encoder_state1 == 1)
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
        if(encoder_knob == 1)
          {
        	switch (par_num){
        	case 2:
        	  condish = preset_write;
        	  eepr_read_imya(prog1);
        	  DisplayTask->Main_scr();
        	  DisplayTask->Prog_ind(prog1);
        	  DisplayTask->StringOut(40,3,TDisplayTask::fntSystem,0,(uint8_t*)write_);
        	 break;
        	case 0:
                 vol_fl = 0;
                 edit_fl = 0;
                 num_men = 0;
                 condish = menu_main;
                 prog_ch();
                 prog_cur = 0;
                 gui_send(15,0);
                 eepr_read_imya(prog1);
                 DisplayTask->Main_scr();
                 DisplayTask->Prog_ind(prog1);
       	         if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
       		     if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
       		     if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
        	 break;
        	case 1:
                 condish = menu_main;
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
            condish = menu_main;
            prog_ch();
            prog_cur = 0;
            edit_fl = 0;
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
        case volume:
        	if(enc_knob_fl == 0)
        	{
        		if(par_num)
        		{
        			if(tim5_fl)
        			{
        				DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_outp + 7);
        				vol_fl = 1;
        			}
        			else {
        				DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
        				vol_fl = 0;
        			}
        		}
        		else {
        			if(tim5_fl)DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
        			else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
        		}
        	}
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
            	  if(!enc_knob_fl)
            	  {
            		  if(par_num)
            		  {
            			  DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
            			  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
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
            	  if(!enc_knob_fl)
            	  {
            		  if(!par_num)
            		  {
            			  DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
            			  DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_outp + 7);
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
          if(encoder_knob == 1)
            {
                  if(!enc_knob_fl)
                  {
                    enc_knob_fl = 1;
                    if(par_num)DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 2 , (uint8_t*)in_outp + 7);
                    else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 2 , (uint8_t*)"Control");
                  }
                  else {
                    if(par_num)DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
                    else DisplayTask->StringOut(28,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Control");
                    enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
          if((k_att == 1) || (k_sys == 1) || (tuner_flag == 1) || (k_master_eq == 1))
          {
        	  if(tuner_flag == 1)num_men = 13;
              condish = menu_main;
              enc_knob_fl = 0;
              vol_fl = 0;
              tim5_start(0);
          	  CSTask->Give();
          	  break;
          }
          if((k_up == 1) || (k_master == 1))
            {
        	  sub_men = 0;
              condish = menu_main;
              enc_knob_fl = 0;
              vol_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//-------------------------------------------------------------------------Menu Eq-Edit-----------------------------------------
        case equaliser:
        	if(enc_knob_fl == 0)
        	{
        		if(eq_num < 5)
        		{
        			if(tim5_fl == 0)DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],0);
        			else DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
        		}
        		else {
        			if(tim5_fl == 0)DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,0,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
        			else DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
        		}
        	}
        	if(encoder_state1 == 1)
        		{
        		if(encoder_state == 1)
        		{
        			if(enc_knob_fl == 0)
        			{
        				if(eq_num == 5)
        				{
        					DisplayTask->EqInit();
        					DisplayTask->Icon_Strel(4,2);
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
        			else {
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
        		}
        		if(encoder_state == 2)
        		{
        			if(enc_knob_fl == 0)
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
        					DisplayTask->Icon_Strel(4,1);
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
        		}
                clean_flag();
        	}
            if(encoder_knob == 1)
              {
                    if(enc_knob_fl == 0)
                      {
                        enc_knob_fl = 1;
                        if(eq_num < 5)DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
                        else DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
                      }
                    else {
                        if(eq_num < 5)DisplayTask->EqIndic(27 + eq_num*14,0,prog_data[eq1 + eq_num],1);
                        else DisplayTask->StringOut(6,eq_num - 5,TDisplayTask::fntSystem,2,(uint8_t*)lpf_hpf + (eq_num - 5)*9);
                        enc_knob_fl = 0;
                    }
                    tim5_start(1);
                    clean_flag();
              }
            if((k_down == 1) && (!encoder_knob) && (eq_num < 5))
            {
            	condish = eq_band;
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
            	enc_knob_fl = 0;
            	tim5_start(0);
                clean_flag();
            }
            if(k_up == 1)
              {
            	eq_num = prog;
                condish = menu_main;
                enc_knob_fl = 0;
                DisplayTask->Menu_init();
                tim5_start(0);
                clean_flag();
              }
          break;
//-------------------------------------------------------------------------Eq Band-----------------------------
        case eq_band:
        	volatile int8_t a;
            if(enc_knob_fl == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num*2);
                else DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num*2);
              }
            if(encoder_state1 == 1)
            {
            	if(encoder_state == 1)
            	{
            		if(!enc_knob_fl)
            		{
            			if(par_num > 0)
            			{
            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num-- * 2);
            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num * 2);
            				tim5_start(0);
            			}
            		}
            		else {
            			a = prog_data[f1 + eq_num + par_num*5];
            			if(par_num)
            			{
            				if(a > -60)
            				{
            					a = enc_speed_dec(a,-60);
            					prog_data[f1 + eq_num + par_num*5] = a;
            					DisplayTask->EqPar( 40 , 1 , a,par_num,eq_num);
            					gui_send(22 , eq_num + par_num*5);
            				}
            			}
            			else {
            				if(a > -100)
            				{
            					a = enc_speed_dec(a,-100);
            					prog_data[f1 + eq_num + par_num*5] = a;
            					DisplayTask->EqPar( 40 , 0 , a,par_num,eq_num);
            					DisplayTask->StringOut(80,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
            					gui_send(22 , eq_num + par_num*5);
            				}
            			}
            		}
            	}
            	if(encoder_state == 2)
            	{
            		if(!enc_knob_fl)
            		{
            			if(par_num < 1)
            			{
            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num++ * 2);
            				DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num * 2);
            				tim5_start(0);
            			}
            		}
            		else {
            			a = prog_data[f1 + eq_num + par_num*5];
            			if(par_num)
            			{
            				if(a < 120)
            				{
            					a = enc_speed_inc(a,120);
            					prog_data[f1 + eq_num + par_num*5] = a;
            					DisplayTask->EqPar( 40 , 1 , a,par_num,eq_num);
            					gui_send(22 , eq_num + par_num*5);
            				}
            			}
            			else {
            				if(a < 100)
            				{
            					a = enc_speed_inc(a,100);
            					prog_data[f1 + eq_num + par_num*5] = a;
            					DisplayTask->EqPar( 40 , 0 , a,par_num,eq_num);
            					DisplayTask->StringOut(80,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
            					gui_send(22 , eq_num + par_num*5);
            				}
            			}
            		}
            	}
                clean_flag();
            }
            if(encoder_knob)
            {
                if(enc_knob_fl == 0)
                {
                  enc_knob_fl = 1;
                  DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)eq_p_l + par_num*2);
                }
                else {
                	DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)eq_p_l + par_num*2);
                    enc_knob_fl = 0;
                }
                tim5_start(0);
                clean_flag();
            }
            if(k_master_eq)
            {
            	prog_data[f1 + eq_num] = 0;
            	prog_data[f1 + eq_num + 5] = 0;
				DisplayTask->EqPar( 40 , 0 , prog_data[f1 + eq_num] , 0 , eq_num);
				DisplayTask->StringOut(80,0,TDisplayTask::fntSystem,0,(uint8_t*)gerz);
				DisplayTask->EqPar( 40 , 1 , prog_data[f1 + eq_num + 5] , 1 , eq_num);
				gui_send(22 , eq_num);
				gui_send(22 , eq_num + 5);
		        clean_flag();
            }
            if(k_up == 1)
              {
            	enc_knob_fl = 0;
                condish = equaliser;
                DisplayTask->EqInit();
                DisplayTask->Icon_Strel(4,2);
                tim5_start(0);
                clean_flag();
              }
        	break;
//--------------------------------------------------------------------------Phaser----------------------------------
        case phazer:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)phas_list + par_num*8);
              else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)phas_list + par_num*8);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num > 0)
                        {
                   	      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)phas_list + par_num-- * 8);
                          if(par_num == 3)
                           {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(9,2);
                       	    for(uint8_t i = 0 ; i < 4 ; i++)
                       		{
                       		    if(i)DisplayTask->ParamIndic(53,i,prog_data[phaser_vol + i]);
                       		    else DisplayTask->ParamIndicMix(53,0,prog_data[phaser_vol]);
                       		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)phas_list + i*8);
                       		}
                           }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)phas_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num)
                	  {
                	  case 0:case 1:case 2:case 3:case 4:case 5:
                		if(prog_data[phaser_vol + par_num] > 0)
                		{
                            if(par_num == 5)
                          	  {
                                prog_data[phaser_type] -= 2;
                                DisplayTask->ParamIndicNum(62,1,prog_data[phaser_type] + 4);
                          	  }
                            else {
                    			prog_data[phaser_vol + par_num] = enc_speed_dec(prog_data[phaser_vol + par_num],0);
                    			if(par_num)DisplayTask->ParamIndic(53,par_num & 3,prog_data[phaser_vol + par_num]);
                    			else DisplayTask->ParamIndicMix(53,0,prog_data[phaser_vol]);
                            }
                            gui_send(5,par_num);
                		}break;
                	  case 6:if(prog_data[hpf_ph] > 0)
                	  	  	  {
                		  	  	  prog_data[hpf_ph] = enc_speed_dec(prog_data[hpf_ph],0);
                		  	  	  DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_ph]);
                	  	  	  }gui_send(5,par_num);break;
                	  case 7:if(prog_data[phas_pos])
                	  	  	  {
                		          DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + --prog_data[phas_pos] * 5);
                		          gui_send(5,par_num);
                	  	  	  }break;
                	  }
                  }
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num < 7)
                        {
                   	      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)phas_list + par_num++ * 8);
                          if(par_num == 4)
                          {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(9,1);
                       	    for(uint8_t i = 0 ; i < 4 ; i++)
                       	    {
                       	    	if(i)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)phas_list + (par_num + i)*8);
                       	    	switch(i){
                       	    	case 0:DisplayTask->ParamIndic(53,0,prog_data[phaser_fed]);break;
                       	    	case 1:DisplayTask->ParamIndicNum(62,1,prog_data[phaser_type] + 4);break;
                       	    	case 2:DisplayTask->ParamIndic(53,2,prog_data[hpf_ph]);break;
                       	    	case 3:DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + prog_data[phas_pos] * 5);break;
                       	    	}
                       	    }
                          }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)phas_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num)
                	  {
                	  case 0:case 1:case 2:case 3:case 4:case 5:
                		if(prog_data[phaser_vol + par_num] < 127)
                		{
                			if(par_num == 5)
                			{
                           	 if(prog_data[phaser_type] < 4)
                                {
                                   prog_data[phaser_type] += 2;
                                   DisplayTask->ParamIndicNum(62,1,prog_data[phaser_type] + 4);
                                   gui_send(5,par_num);
                                }
                			}
                			else {
                				prog_data[phaser_vol + par_num] = enc_speed_inc(prog_data[phaser_vol + par_num],127);
                    			if(par_num)DisplayTask->ParamIndic(53,par_num & 3,prog_data[phaser_vol + par_num]);
                    			else DisplayTask->ParamIndicMix(53,0,prog_data[phaser_vol]);
                			}
                			gui_send(5,par_num);
                		}break;
                	  case 6:if(prog_data[hpf_ph] < 127)
                	  	  	  {
                 				prog_data[hpf_ph] = enc_speed_inc(prog_data[hpf_ph],127);
                 				DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_ph]);
                 				gui_send(5,par_num);
                	  	  	  }break;
                	  case 7:if(!prog_data[phas_pos])
                	  	  	  {
                		          DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + ++prog_data[phas_pos] * 5);
                		          gui_send(5,par_num);
                	  	  	  }break;
                	  }
                   }
                }
              clean_flag();
            }
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                  {
                    enc_knob_fl = 1;
                    DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)phas_list + par_num*8);
                  }
                  else {
                  	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)phas_list + par_num*8);
                    enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
        if(k_up == 1)
          {
            condish = menu_main;
            gui_send(15,0);
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//----------------------------------------------------------------Chorus--------------------
          case chorus:
            if(enc_knob_fl == 0)
              {
            	if(prog_data[chor_typ] < 4)
            	{
                    if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list + par_num*8);
                    else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list + par_num*8);
            	}
            	else {
            		if(prog_data[chor_typ] == 4)
            		{
                        if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list1 + par_num*8);
                        else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list1 + par_num*8);
            		}
            		else {
                        if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list2 + par_num*8);
                        else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list2 + par_num*8);
            		}
            	}
              }
            if(encoder_state1 == 1)
              {
                if(encoder_state == 1)
                  {
                    if(!enc_knob_fl)
                      {
                        if(par_num > 0)
                          {
                        	if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list + par_num-- * 8);
                        	else {
                        		if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list1 + par_num-- * 8);
                        		else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list2 + par_num-- * 8);
                        	}
                            if(par_num == 3)
                             {
                              DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(5,2);
                         	  for(uint8_t i = 0 ; i < 4 ; i++)
                         	  {
                            	  if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list + i*8);
                            	  else {
                            		  if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list1 + i*8);
                            		  else DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)chor_list2 + i*8);
                            	  }
                            	  if(i)
                            	  {
                            		  if(prog_data[chor_typ] < 4)DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
                            		  else {
                            			  if((i == 1) && (prog_data[chor_typ] == 4))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
                            			  else {
                            				  if((i == 3) && (prog_data[chor_typ] == 5))DisplayTask->ParamIndic(53,i,prog_data[chor_volum + i]);
                            			  }
                            		  }
                            	  }
                                  else DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);
                         	  }
                             }
                            if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list + par_num * 8);
                            else {
                            	if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list1 + par_num * 8);
                            	else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list2 + par_num * 8);
                            }
                            tim5_start(0);
                          }
                      }
                    else {
                    	switch(par_num){
                    	case 0:case 1:case 2:case 3:case 4:
                    		if(prog_data[chor_volum + par_num] > 0)
                    		{
                    			if(par_num == 4)
                    			{
                    				DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)ch_list_t + --prog_data[chor_typ]*12);
                    			}
                    			else {
                    				prog_data[chor_volum + par_num] = enc_speed_dec(prog_data[chor_volum + par_num],0);
                    				if(par_num)
                    				{
                        				if(prog_data[chor_typ] < 4)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                       	  		        else {
                       	  		    	   if(prog_data[chor_typ] == 4)
                       	  		    	   {
                       	  		    		   if(par_num == 1)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                       	  		    	   }
                       	  		    	   else {
                       	  		    		if(par_num == 3)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                       	  		    	   }
                       	  		       }
                    				}
                    				else DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);
                    			}
                    			gui_send(9,par_num);
                    		}break;
                    	case 5:if(prog_data[hpf_ch] > 0)
                    		{
                    			prog_data[hpf_ch] = enc_speed_dec(prog_data[hpf_ch],0);
                    			DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_ch]);
                    			gui_send(9,par_num);
                    		}break;
                    	}
                    }
                  }
                if(encoder_state == 2)
                  {
                    if(enc_knob_fl == 0)
                      {
                        if(par_num < 5)
                          {
                        	if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list + par_num++ * 8);
                        	else {
                        		if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list1 + par_num++ * 8);
                        		else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list2 + par_num++ * 8);
                        	}
                        	if(par_num == 4)
                            {
                              DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(5,1);
                         	  DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)chor_list + (par_num + 1) * 8);
                         	  DisplayTask->ParamIndic(53,1,prog_data[hpf_ch]);
                         	  DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)ch_list_t + prog_data[chor_typ]*12);
                            }
                        	if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list + par_num * 8);
                        	else {
                        		if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list1 + par_num * 8);
                        		else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list2 + par_num * 8);
                        	}
                        	tim5_start(0);
                          }
                      }
                    else {
                    	switch(par_num)
                    	{
                    	case 0:case 1:case 2:case 3:case 4:
                    		if(par_num == 4)
                    		{
                    			if(prog_data[chor_typ] < 5)
                    			{
                    				DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)ch_list_t + ++prog_data[chor_typ]*12);
                    				gui_send(9,par_num);
                    			}
                    		}
                    		else {
                    			if(prog_data[chor_volum + par_num] < 127)
                    			{
                    				prog_data[chor_volum + par_num] = enc_speed_inc(prog_data[chor_volum + par_num],127);
                      	  		    if(par_num)
                      	  		    {
                      	  		       if(prog_data[chor_typ] < 4)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                      	  		       else {
                      	  		    	   if(prog_data[chor_typ] == 4)
                      	  		    	   {
                      	  		    		   if(par_num == 1)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                      	  		    	   }
                      	  		    	   else {
                      	  		    		if(par_num == 3)DisplayTask->ParamIndic(53,par_num & 3,prog_data[chor_volum + par_num]);
                      	  		    	   }
                      	  		       }
                      	  		    }
                    				else DisplayTask->ParamIndicMix(53,0,prog_data[chor_volum]);
                      	  		    gui_send(9,par_num);
                    			}
                    		}break;
                    	case 5:if(prog_data[hpf_ch] < 127)
                    			{
                    				prog_data[hpf_ch] = enc_speed_inc(prog_data[hpf_ch],127);
                    				DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_ch]);
                    				gui_send(9,par_num);
                    			}break;
                    	}
                     }
                  }
                clean_flag();
              }
            if(encoder_knob == 1)
              {
                    if(enc_knob_fl == 0)
                      {
                        if(prog_data[chor_typ] < 4)
                        {
                        	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list + par_num * 8);
                            enc_knob_fl = 1;
                        }
                        else {
                        	if(prog_data[chor_typ] == 4)
                        	{
                        		if((par_num < 2) || (par_num > 3))
                        		{
                        			enc_knob_fl = 1;
                        			DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list1 + par_num * 8);
                        		}
                        	}
                        	else {
                        		if((par_num < 1) || (par_num > 2))
                        		{
                        			enc_knob_fl = 1;
                        			DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)chor_list2 + par_num * 8);
                        		}
                        	}
                        }
                      }
                    else {
                    	if(prog_data[chor_typ] < 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list + par_num * 8);
                    	else {
                    		if(prog_data[chor_typ] == 4)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list1 + par_num * 8);
                    		else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)chor_list2 + par_num * 8);
                    	}
                    	enc_knob_fl = 0;
                    }
                    tim5_start(1);
                    clean_flag();
              }
          if(k_up == 1)
            {
              condish = menu_main;
              gui_send(15,0);
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//----------------------------------------------------------------------Delay--------------------------------
        case diley:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)del_list + par_num*8);
              else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)del_list + par_num*8);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                	  if(par_num > 0)
                	  {
                	   DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)del_list + par_num-- * 8);
                       if(par_num == 3)
                        {
                    	  DisplayTask->Clear();
                    	  DisplayTask->Icon_Strel(0,2);
                    	  for(uint8_t i = 0 ; i < 4 ; i++)
                    		  {
                    		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + i*8);
                    		    if(i)
                    		    {
                    		    	if(i > 1)DisplayTask->ParamIndic(53,i,prog_data[d_vol + i - 1]);
                    		    	else DisplayTask->DelayTimeInd(53,i,delay_time);
                    		    }
                    		    else DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);
                    		  }
                        }
                       else {
                    	   if(par_num == 7)
                    	   {
                         	  DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(0,3);
                        	  for(uint8_t i = 0 ; i < 4 ; i++)
                        		{
                        	     DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + (i+4)*8);
                        	     if(i & 1)DisplayTask->ParamIndicPan(53,i,prog_data[d_vol + 4 + i - 1]);
                        	     else DisplayTask->ParamIndic(53,i,prog_data[d_vol + 4 + i - 1]);
                        	    }
                    	   }
                       }
                       DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)del_list + par_num * 8);
                       tim5_start(0);
                	  }
                    }
                  else {
                    	  switch(par_num){
                    	  case 0:if(prog_data[d_vol + par_num] > 0)
                    		  {
                    		     prog_data[d_vol + par_num] = enc_speed_dec(prog_data[d_vol + par_num],0);
                    		     DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);
                    		  }break;
                    	  case 2:case 3:case 4:case 6:case 8:case 9:case 10:if(prog_data[d_vol + par_num - 1] > 0)
                    		  {
                    		     prog_data[d_vol + par_num - 1] = enc_speed_dec(prog_data[d_vol + par_num - 1],0);
                    		     DisplayTask->ParamIndic(53,par_num & 3,prog_data[d_vol + par_num - 1]);
                    		  }break;
                    	  case 5:case 7:if(prog_data[d_vol + par_num - 1] > 0)
                    		  {
                    		     prog_data[d_vol + par_num - 1] = enc_speed_dec(prog_data[d_vol + par_num - 1],0);
                    		     DisplayTask->ParamIndicPan(53,par_num & 3,prog_data[d_vol + par_num - 1]);
                    		  }break;
                    	  case 11:if(prog_data[d_vol + par_num - 1] > 0)
                    		   DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)del_dir + --prog_data[d_dir]*8);
							   break;
        				  case 1:
                          if(delay_time > 10)
                            {
                              if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)delay_time--;
                              else {
                                  if (TIM_GetCounter(TIM6) > 0x1fff)
                                  {
                                	  if(delay_time > 20)delay_time -= 10;
                                	  else delay_time--;
                                  }
                                  else {
                                      if (TIM_GetCounter(TIM6) > 0xfff)
                                      {
                                    	  if(delay_time > 40)delay_time -= 30;
                                    	  else delay_time--;
                                      }
                                      else {
                                          if (TIM_GetCounter(TIM6) > 0x7ff)
                                          {
                                        	  if(delay_time > 70)delay_time -= 60;
                                        	  else delay_time--;
                                          }
                                          else {
                                        	  if(delay_time > 110)delay_time -= 100;
                                        	  else delay_time--;
                                          }
                                      }
                                  }
                              }
                              DisplayTask->DelayTimeInd(53,1,delay_time);
                              TIM_SetCounter(TIM6,0);
                              TIM_ClearFlag(TIM6,TIM_FLAG_Update);
                              TIM_Cmd(TIM6,ENABLE);
                            }break;
                        }
                      gui_send(3,par_num);
                  }
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                	  if(par_num < 11)
                	  {
                	   DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)del_list + par_num++ * 8);
                       if(par_num == 4)
                       {
                    	  DisplayTask->Clear();
                    	  DisplayTask->Icon_Strel(0,3);
                    	  for(uint8_t i = 0 ; i < 4 ; i++)
                    		  {
                    		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + (i+par_num)*8);
                    		    if(i & 1)DisplayTask->ParamIndicPan(53,i,prog_data[d_vol + par_num + i - 1]);
                    		    else DisplayTask->ParamIndic(53,i,prog_data[d_vol + par_num + i - 1]);
                    		  }
                       }
                       else {
                    	   if(par_num == 8)
                    	   {
                         	  DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(0,1);
                         	  for(uint8_t i= 1 ; i < 4 ; i++)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem,0,(uint8_t*)del_list + (par_num + i) * 8);
                         	  for(uint8_t i= 0 ; i < 3 ; i++)DisplayTask->ParamIndic(53,i,prog_data[d_vol + par_num + i - 1]);
                         	  DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)del_dir + prog_data[d_dir]*8);
                    	   }
                       }
                       DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)del_list + par_num * 8);
                       tim5_start(0);
                	  }
                    }
                  else {
                	  switch(par_num){
                	  case 0:if(prog_data[d_vol + par_num] < 127)
                	  {
                		  prog_data[d_vol + par_num] = enc_speed_inc(prog_data[d_vol + par_num],127);
                		  DisplayTask->ParamIndicMix(53,par_num & 3,prog_data[d_vol]);
                	  }break;
                	  case 2:case 3:case 4:case 6:case 8:case 9:case 10:if(prog_data[d_vol + par_num - 1] < 127)
                	  {
                		  prog_data[d_vol + par_num - 1] = enc_speed_inc(prog_data[d_vol + par_num - 1],127);
                		  DisplayTask->ParamIndic(53,par_num & 3,prog_data[d_vol + par_num - 1]);
                	  }break;
                	  case 5:case 7:if(prog_data[d_vol + par_num - 1] < 126)
                	  {
                		  prog_data[d_vol + par_num - 1] = enc_speed_inc(prog_data[d_vol + par_num - 1],126);
                		  DisplayTask->ParamIndicPan(53,par_num & 3,prog_data[d_vol + par_num - 1]);
                	  }break;
                	  case 11:if(prog_data[d_vol + par_num - 1] < 1)
                		  DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)del_dir + ++prog_data[d_dir]*8);
                	   break;
                	  }
                	  gui_send(3,par_num);
                }
            }
            clean_flag();
          }
          if(encoder_knob == 1)
            {
        	  if(par_num != 1)
        	  {
                  if(enc_knob_fl == 0)
                    {
                      enc_knob_fl = 1;
                	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)del_list + par_num * 8);
                    }
                  else {
                      enc_knob_fl = 0;
                      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)del_list + par_num * 8);
                  }
        	  }
        	  else {
        		  DisplayTask->Clear();
        		  condish = tap_del_menu;
        		  eq_num = 0;
        		  DisplayTask->StringOut(6,0,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l);
        		  DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + 5);
        		  DisplayTask->StringOut(6,2,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + 10);
        		  if(!sys_para[tim_type])DisplayTask->DelayTimeInd(53,0,delay_time);
        		  else {
        			  DisplayTask->ParamIndicNum(53,0,60000/delay_time);
        			  DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
        		  }
        		  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + prog_data[d_tap_t]*6);
        		  if(prog_data[d_tail])DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
        		  else DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
        	  }
              tim5_start(1);
              clean_flag();
            }
          if(k_down == 1)
             {
        	  if(tap_temp_global() && !sys_para[tap_typ])
        	  {
        		  delay_time = tap_global / 3.0f / tap_tim_v[prog_data[d_tap_t]];
          		  if(delay_time < 2731)
          		  {
          			  uint8_t temp = 0;
          			  if(!sys_para[tim_type])
          			  {
          				  if(par_num < 4)DisplayTask->DelayTimeInd(53,1,delay_time);
              			  gui_send(3,1);
          			  }
          			  else {
          				  if(delay_time < 2728 && delay_time > 249)
          				  {
          					  while(delay_time < bpm_time[temp++]);
          					  delay_time = bpm_time[temp];
          					  prog_data[bpm_del] = 60000 / delay_time;
          					  DisplayTask->ParamIndicNum(53,1,prog_data[bpm_del]);
          					  DisplayTask->StringOut(90,1,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
          	      			  gui_send(3,1);
          				  }
          			  }
          		  }
        	  }
              clean_flag();
             }
          if(k_up == 1)
            {
              condish = menu_main;
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
        break;
//---------------------------------------------------------TAP Delay type---------------------------
        case tap_del_menu:
        if(enc_knob_fl == 0)
          {
            if(tim5_fl == 1)DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)del_tim_l + eq_num*5);
            else DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + eq_num*5);
          }
        if(encoder_state1 == 1)
          {
            if(encoder_state == 1)
              {
                if(enc_knob_fl == 0)
                  {
                	if(eq_num)
                	{
                		DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + eq_num-- * 5);
                		DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)del_tim_l + eq_num*5);
                	}
                  }
                else {
                	switch (eq_num){
                	case 0:if(delay_time > 10)
                    {
                		if(!sys_para[tim_type])
                		{
                			if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)delay_time--;
                			else {
                				if (TIM_GetCounter(TIM6) > 0x1fff)
                				{
                					if(delay_time > 20)delay_time -= 10;
                					else delay_time--;
                				}
                				else {
                					if (TIM_GetCounter(TIM6) > 0xfff)
                					{
                						if(delay_time > 40)delay_time -= 30;
                						else delay_time--;
                					}
                					else {
                						if (TIM_GetCounter(TIM6) > 0x7ff)
                						{
                							if(delay_time > 70)delay_time -= 60;
                							else delay_time--;
                						}
                						else {
                							if(delay_time > 110)delay_time -= 100;
                							else delay_time--;
                						}
                					}
                				}
                			}
                			DisplayTask->DelayTimeInd(53,0,delay_time);
                			TIM_SetCounter(TIM6,0);
                			TIM_ClearFlag(TIM6,TIM_FLAG_Update);
                			TIM_Cmd(TIM6,ENABLE);
                		}
                		else {
                			uint16_t temp = 60000 / delay_time;
                			if(temp > 22)
                			{
                				temp = enc_speed_dec(temp , 22);
                				delay_time = 60000 / temp;
                				DisplayTask->ParamIndicNum(53,0,temp);
                				DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
                			}
                			else {
                				temp = 22;
                				delay_time = 60000 / temp;
                			}
                		}
                      gui_send(3,1);
                    }break;
                	case 1:if(prog_data[d_tap_t])DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + --prog_data[d_tap_t]*6);break;
                	case 2:if(prog_data[d_tail])
            		    {
            		        prog_data[d_tail]--;
            		        DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
            		        gui_send(3,13);
            		    }break;
                	}
                }
              }
            if(encoder_state == 2)
              {
                if(enc_knob_fl == 0)
                  {
                	if(eq_num < 2)
                	{
                		DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + eq_num++ * 5);
                		DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)del_tim_l + eq_num*5);
                	}
                  }
                else {
                	switch (eq_num){
                	case 0:if(delay_time < 2730)
            		  {
                		if(!sys_para[tim_type])
                		{
                          if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)delay_time++;
                          else {
                              if (TIM_GetCounter(TIM6) > 0x1fff)
                              {
                            	  if(delay_time < 2720)delay_time += 10;
                            	  else delay_time++;
                              }
                              else {
                                  if (TIM_GetCounter(TIM6) > 0xfff)
                                  {
                                	  if(delay_time < 2700)delay_time += 30;
                                	  else delay_time++;
                                  }
                                  else {
                                      if (TIM_GetCounter(TIM6) > 0x7ff)
                                      {
                                    	  if(delay_time < 2670)delay_time += 60;
                                    	  else delay_time++;
                                      }
                                      else {
                                    	  if(delay_time < 2630)delay_time += 100;
                                    	  else delay_time++;
                                      }
                                  }
                              }
                          }
                          DisplayTask->DelayTimeInd(53,0,delay_time);
                          TIM_SetCounter(TIM6,0);
                          TIM_ClearFlag(TIM6,TIM_FLAG_Update);
                          TIM_Cmd(TIM6,ENABLE);
                		}
                		else {
                			uint16_t temp = 60000 / delay_time;
                			if(temp < 240)
                			{
                				temp = enc_speed_inc(temp , 240);
                    			delay_time = 60000 / temp;
                    			DisplayTask->ParamIndicNum(53,0,temp);
                    			DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
                			}
                			else {
                				temp = 240;
                				delay_time = 60000 / temp;
                			}
                		}
                		gui_send(3,1);
            		  }break;
                	case 1:if(prog_data[d_tap_t] < 5)DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + ++prog_data[d_tap_t]*6);break;
                	case 2:if(!prog_data[d_tail])
                		{
                		   prog_data[d_tail]++;
                		   DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
                		   gui_send(3,13);
                		}break;
                	}
                }
              }
            clean_flag();
            tim5_start(0);
          }
        if(encoder_knob == 1)
          {
            if(enc_knob_fl == 0)
             {
                enc_knob_fl = 1;
                DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)del_tim_l + eq_num*5);
             }
            else {
                enc_knob_fl = 0;
                DisplayTask->StringOut(6,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)del_tim_l + eq_num*5);
            }
            tim5_start(1);
            clean_flag();
          }
        if(k_down == 1)
         {
      	  if(tap_temp_global() && !sys_para[tap_typ])
      	  {
      		  delay_time = tap_global / 3.0f / tap_tim_v[prog_data[d_tap_t]];
        		  if(delay_time < 2731)
        		  {
        			  uint8_t temp = 0;
        			  if(!sys_para[tim_type])
        			  {
        				  if(par_num < 4)DisplayTask->DelayTimeInd(53,0,delay_time);
            			  gui_send(3,1);
        			  }
        			  else {
        				  if(delay_time < 2728 && delay_time > 249)
        				  {
        					  while(delay_time < bpm_time[temp++]);
        					  delay_time = bpm_time[temp];
        					  prog_data[bpm_del] = 60000 / delay_time;
        					  DisplayTask->ParamIndicNum(53,0,prog_data[bpm_del]);
        					  DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
        	      			  gui_send(3,1);
        				  }
        			  }
        		  }
      	  }
      	  clean_flag();
         }
        if(k_up == 1)
          {
            condish = diley;
            DisplayTask->Clear();
            for(uint8_t i = 0 ; i < 4 ; i++)
              {
            	DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)del_list + i*8);
            	if(i)
            	{
          		  if(i > 1)DisplayTask->ParamIndic(53,i,prog_data[d_vol + i - 1]);
          		  else {
          			  if(!sys_para[tim_type])DisplayTask->DelayTimeInd(53,i,delay_time);
          			  else {
          				  DisplayTask->ParamIndicNum(53,i,60000/delay_time);
          				  DisplayTask->StringOut(90,i,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
          			  }
          		  }
            	}
            	else DisplayTask->ParamIndicMix(53,0,prog_data[d_vol]);
              }
            DisplayTask->Icon_Strel(0,2);
            enc_knob_fl = 0;
            tim5_start(0);
            clean_flag();
          }
        break;
//----------------------------------------------------------------------Flanger---------------------
        case flanger:
            if(enc_knob_fl == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)fl_list + par_num*8);
                else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)fl_list + par_num*8);
              }
            if(encoder_state1 == 1)
              {
                if(encoder_state == 1)
                  {
                    if(enc_knob_fl == 0)
                      {
                        if(par_num > 0)
                          {
                        	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)fl_list + par_num-- * 8);
                            if(par_num == 3)
                             {
                              DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(10,2);
                         	  for(uint8_t i = 0 ; i < 4 ; i++)
                         		  {
                         		    if(!i)DisplayTask->ParamIndicMix(53,0,prog_data[fl_v]);
                         		    else {
                         		    	if(i == 1)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem,0,(uint8_t*)fl_t + prog_data[fl_lfo]*9);
                         		    	else DisplayTask->ParamIndic(53,i,prog_data[fl_v + i]);
                         		    }
                         		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)fl_list + i*8);
                         		  }
                             }
                        	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)fl_list + par_num * 8);
                        	tim5_start(0);
                          }
                      }
                    else {
                    	switch(par_num){
                    	case 0:case 1:case 2:case 3:case 4:case 5:
                    		if(prog_data[fl_v + par_num] > 0)
                    		{
                    			if(par_num == 1)DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)fl_t + --prog_data[fl_lfo]*12);
                    			else {
                    				prog_data[fl_v + par_num] = enc_speed_dec(prog_data[fl_v + par_num],0);
                            		if(par_num)DisplayTask->ParamIndic(53,par_num & 3,prog_data[fl_v + par_num]);
                            		else DisplayTask->ParamIndicMix(53,0,prog_data[fl_v]);
                    			}
                    			gui_send(8,par_num);
                    		}break;
                    	case 6:if(prog_data[hpf_fl] > 0)
                    			{
                    				prog_data[hpf_fl] = enc_speed_dec(prog_data[hpf_fl],0);
                    				DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_fl]);
                    				gui_send(8,par_num);
                    			}break;
                    	case 7:if(prog_data[flan_pos])
                    			{
                    		        DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + --prog_data[flan_pos] * 5);
                    		        gui_send(8,par_num);
                    			}break;
                    	}
                    }
                  }
                if(encoder_state == 2)
                  {
                    if(enc_knob_fl == 0)
                      {
                        if(par_num < 7)
                          {
                        	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)fl_list + par_num++ * 8);
                            if(par_num == 4)
                            {
                                DisplayTask->Clear();
                           	    DisplayTask->Icon_Strel(10,1);
                            	for(uint8_t i = 0 ; i < 4 ; i++)
                            	{
                            		if(i)DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)fl_list + (par_num + i)*8);
                            		if(i < 2)DisplayTask->ParamIndic(53,i,prog_data[fl_d + i]);
                            		else {
                            			if(i == 2)DisplayTask->ParamIndic(53,2,prog_data[hpf_fl]);
                            			else DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + prog_data[flan_pos] * 5);
                            		}
                            	}
                            }
                        	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)fl_list + par_num * 8);
                        	tim5_start(0);
                          }
                      }
                    else {
                    	switch(par_num){
                    	case 0:case 1:case 2:case 3:case 4:case 5:
                    		if(prog_data[fl_v + par_num] < 127)
                    		{
                    			if(par_num == 1)
                    			{
                    				if(prog_data[fl_v + par_num] < 1)
                    				{
                              		  prog_data[fl_v + par_num]++;
                              		  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)fl_t + prog_data[fl_lfo]*9);
                              		  gui_send(8,par_num);
                    				}
                    			}
                    			else {
                    				prog_data[fl_v + par_num] = enc_speed_inc(prog_data[fl_v + par_num],127);
                          		    if(par_num)DisplayTask->ParamIndic(53,par_num & 3,prog_data[fl_v + par_num]);
                          		    else DisplayTask->ParamIndicMix(53,0,prog_data[fl_v]);
                          		    gui_send(8,par_num);
                    			}
                    		}break;
                    	case 6:if(prog_data[hpf_fl] < 127)
                    			{
                    				prog_data[hpf_fl] = enc_speed_inc(prog_data[hpf_fl],127);
                    				DisplayTask->ParamIndic(53,par_num & 3,prog_data[hpf_fl]);
                    				gui_send(8,par_num);
                    			}
                    		break;
                    	case 7:if(!prog_data[flan_pos])
                    			{
                    		        DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)eq_pre_post + ++prog_data[flan_pos] * 5);
                    		        gui_send(8,par_num);
                    			}break;
                    	}
                     }
                  }
                clean_flag();
              }
            if(encoder_knob == 1)
              {
                    if(enc_knob_fl == 0)
                      {
                        enc_knob_fl = 1;
                        DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)fl_list + par_num * 8);
                      }
                    else {
                    	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)fl_list + par_num * 8);
                        enc_knob_fl = 0;
                    }
                    tim5_start(1);
                    clean_flag();
              }
          if(k_up == 1)
            {
              condish = menu_main;
              gui_send(15,0);
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//--------------------------------------------------------------Early Reflection---------------------------------
        case early_menu:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)ear_list + par_num*5);
              else DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)ear_list + par_num*5);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num > 0)
                        {
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)ear_list + par_num-- * 5);
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)ear_list + par_num * 5);
                          tim5_start(0);
                        }
                    }
                  else {
                      if(prog_data[early_vol + par_num] > 0)
                        {
                    	  if(!par_num)
                    	  {
                    		prog_data[early_vol] = enc_speed_dec(prog_data[early_vol],0);
                    		DisplayTask->ParamIndicMix(53,0,prog_data[early_vol]);
                    	  }
                    	  else {
                    		  prog_data[early_vol + par_num] = enc_speed_dec(prog_data[early_vol + par_num],0);
                    		  DisplayTask->ParamIndic(53,par_num,prog_data[early_vol + par_num]);
                    	  }
                    	  gui_send(6,par_num);
                        }
                  }
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num < 1)
                        {
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)ear_list + par_num++ * 5);
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)ear_list + par_num * 5);
                          tim5_start(0);
                        }
                    }
                  else {
                	  if(!par_num)
                	  {
                		 if(prog_data[early_vol] < 127)
                		 {
                			 prog_data[early_vol] = enc_speed_inc(prog_data[early_vol],127);
                			 DisplayTask->ParamIndicMix(53,0,prog_data[early_vol]);
                			 gui_send(6,par_num);
                		 }
                	  }
                	  else {
                		  if(prog_data[early_vol + 1] < 127)
                		  {
                			  prog_data[early_vol + 1] = enc_speed_inc(prog_data[early_vol + 1],127);
                			  DisplayTask->ParamIndic(53,par_num,prog_data[early_vol + par_num]);
                			  gui_send(6,par_num);
                		  }
                	  }

                  }
                }
              clean_flag();
            }
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                    {
                      enc_knob_fl = 1;
                      DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)ear_list + par_num * 5);
                    }
                  else {
                      DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)ear_list + par_num * 5);
                      enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
        if(k_up == 1)
          {
            condish = menu_main;
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//------------------------------------------------------------------Reverb-------------------------------------------------
        case rever:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num*7);
              else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + par_num*7);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(!enc_knob_fl)
                    {
                      if(par_num)
                        {
                    	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + par_num-- * 7);
                          if(par_num == 3)
                           {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(2,2);
                       	    for(uint8_t i = 0 ; i < 3 ; i++)
                       		 {
                       	    	DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_list + i*7);
                       		    if(i != 1)
                       		    {
                       		    	if(i)
                       		    	{
                       		    		if(i != 2)DisplayTask->ParamIndic(53,i,prog_data[r_vol + i - 1]);
                       		    		else {
                       		    			if(prog_data[rev_t] > 4)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                       		    			else DisplayTask->ParamIndic(53,i,prog_data[r_vol + i - 1]);
                       		    		}
                       		    	}
                       		    	else DisplayTask->ParamIndicMix(53,i,prog_data[r_vol]);
                       		    }
                       		    else DisplayTask->StringOut(53,1,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_type_list + prog_data[rev_t] * 8);
                       		 }
                       	     if(prog_data[rev_t] != 4)DisplayTask->ParamIndic(53,3,prog_data[r_size]);
                       	     else DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                           }
                          else {
                        	  if(par_num == 7)
                        	  {
                                 DisplayTask->Clear();
                             	 DisplayTask->Icon_Strel(2,3);
                             	 for(uint8_t i = 0 ; i < 3 ; i++)
                             	 {
                             	  	DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_list + (i + 4)*7);
                               	    if(!i)
                               	    {
                                 	  	if(prog_data[rev_t] != 4)DisplayTask->ParamIndic(53,0,prog_data[r_dump]);
                                   	    else DisplayTask->StringOut(53,0,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                               	    }
                               	    else DisplayTask->ParamIndic(53,i,prog_data[r_dump + i]);
                             	 }
                    	    	 if(prog_data[rev_t] < 4)DisplayTask->ParamIndic(53,3,prog_data[r_dump + 3]);
                    	    	 else DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                        	  }
                          }
                    	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7 + 4);
                    	  tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num){
                	  case 0:if(prog_data[r_vol])
        			  {
        				  prog_data[r_vol] = enc_speed_dec(prog_data[r_vol],0);
            			  DisplayTask->ParamIndicMix(53,0,prog_data[r_vol]);
            			  gui_send(2,par_num);
        			  }break;
                	  case 1:if(prog_data[rev_t])
                	  {
                		  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_type_list + --prog_data[rev_t] * 8);
                		  if(prog_data[rev_t] == 4)DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                		  else DisplayTask->ParamIndic(53,3,prog_data[r_size]);
                		  if(prog_data[rev_t] > 4)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                		  else DisplayTask->ParamIndic(53,2,prog_data[r_time]);
                		  gui_send(2,par_num);
                	  }break;
                	  case 8:if(prog_data[rev_di] < 127)
        			  {
        				  prog_data[rev_di] = enc_speed_inc(prog_data[rev_di],127);
        				  DisplayTask->ParamIndic(53,par_num & 3,127 - prog_data[rev_di]);
        				  gui_send(2,par_num);
        			  }break;
                	  case 9:if(prog_data[r_pre])
        			  {
        				  prog_data[r_pre] = enc_speed_dec(prog_data[r_pre],0);
        				  DisplayTask->ParamIndic(53,par_num & 3,prog_data[r_pre]);
            			  gui_send(2,par_num);
        			  }break;
                	  case 10:if(prog_data[r_tail])
                	  {
                		  prog_data[r_tail]--;
                		  DisplayTask->StringOut(53,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"On ");
                		  gui_send(2,par_num);
                	  }break;
                	  default:uint8_t a = r_vol + par_num - 1;if(prog_data[a])
                	  {
                		  prog_data[a] = enc_speed_dec(prog_data[a],0);
                		  DisplayTask->ParamIndic(53,par_num & 3,prog_data[a]);
                		  gui_send(2,par_num);
                	  }break;
                	  }
                  }
                }
              if(encoder_state == 2)
                {
                  if(!enc_knob_fl)
                    {
                      if(par_num < 10)
                        {
                    	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + par_num++ * 7);
                          if(par_num == 4)
                          {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(2,3);
                       	    for(uint8_t i = 1 ; i < 4 ; i++)
                       	    {
                       	    	DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_list + (par_num + i)*7);
                       	    	if(i == 3)
                       	    	{
                       	    		if(prog_data[rev_t] < 4)DisplayTask->ParamIndic(53,i,prog_data[r_dump + i]);
                       	    		else DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                       	    	}
                       	    	else DisplayTask->ParamIndic(53,i,prog_data[r_dump + i]);
                       	    }
                       	    if(prog_data[rev_t] != 4)DisplayTask->ParamIndic(53,0,prog_data[r_dump]);
                       	    else DisplayTask->StringOut(53,0,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                          }
                          else {
                        	 if(par_num == 8)
                        	 {
                                 DisplayTask->Clear();
                            	 DisplayTask->Icon_Strel(2,1);
                            	 if(prog_data[rev_t] && (prog_data[rev_t] != 4))DisplayTask->ParamIndic(53,0,127 - prog_data[rev_di]);
                            	 else DisplayTask->StringOut(53,0,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                            	 DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + (par_num + 1) * 7);
                            	 DisplayTask->StringOut(6,2,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + (par_num + 2) * 7);
                            	 DisplayTask->ParamIndic(53,1,prog_data[r_pre]);
                            	 if(prog_data[r_tail])DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"Off");
                            	 else DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)"On ");
                        	 }
                          }
                    	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7);
                    	  tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num){
                	  case 0:if(prog_data[r_vol] < 127)
        			  {
        				  prog_data[r_vol] = enc_speed_inc(prog_data[r_vol],127);
            			  DisplayTask->ParamIndicMix(53,0,prog_data[r_vol]);
            			  gui_send(2,par_num);
        			  }break;
                	  case 1:if(prog_data[rev_t] < 6)
                	  {
                		  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem , 0 , (uint8_t*)rev_type_list + ++prog_data[rev_t] * 8);
                		  if(prog_data[rev_t] == 4)DisplayTask->StringOut(53,3,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                		  else DisplayTask->ParamIndic(53,3,prog_data[r_size]);
                		  if(prog_data[rev_t] > 4)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)" ---     ");
                		  else DisplayTask->ParamIndic(53,2,prog_data[r_time]);
                		  gui_send(2,par_num);
                	  }break;
                	  case 8:if(prog_data[rev_di])
        			  {
        				  prog_data[rev_di] = enc_speed_dec(prog_data[rev_di],0);
        				  DisplayTask->ParamIndic(53,par_num & 3,127 - prog_data[rev_di]);
            			  gui_send(2,par_num);
        			  }break;
                	  case 9:if(prog_data[r_pre] < 127)
        			  {
        				  prog_data[r_pre] = enc_speed_inc(prog_data[r_pre],127);
        				  DisplayTask->ParamIndic(53,par_num & 3,prog_data[r_pre]);
            			  gui_send(2,par_num);
        			  }break;
                	  case 10:if(!prog_data[r_tail])
                	  {
                		  prog_data[r_tail]++;
                		  DisplayTask->StringOut(53,2,TDisplayTask::fntSystem , 0 , (uint8_t*)"Off");
                		  gui_send(2,par_num);
                	  }break;
                	  default:uint8_t a = r_vol + par_num - 1;if(prog_data[a] < 127)
                	  {
                		  prog_data[a] = enc_speed_inc(prog_data[a],127);
                		  DisplayTask->ParamIndic(53,par_num & 3,prog_data[a]);
                		  gui_send(2,par_num);
                	  }break;
                	  }
                   }
                }
              clean_flag();
            }
          if(encoder_knob == 1)
            {
              if(!enc_knob_fl)
              {
            	switch(par_num){
            	case 0:case 1:case 5:case 6:case 9:case 10:enc_knob_fl = 1;
            	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7);break;
            	case 2:case 7:if(prog_data[rev_t] < 5)
            	{
            		enc_knob_fl = 1;
            		DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7);
            	}break;
            	case 3:case 4:if(prog_data[rev_t] != 4)
            	{
            		enc_knob_fl = 1;
            		DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7);
            	}
            	break;
            	case 8:if((prog_data[rev_t] != 4) && prog_data[rev_t])
            	{
            		enc_knob_fl = 1;
            		DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)rev_list + par_num * 7);
            	}break;
            	}
              }
              else {
                DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)rev_list + par_num * 7);
                enc_knob_fl = 0;
              }
              tim5_start(1);
              clean_flag();
            }
        if(k_up == 1)
          {
            condish = menu_main;
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//---------------------------------------------------------------------Menu Tremolo-----------------------------
        case tremolo:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)tr_list + par_num*8);
              else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_list + par_num*8);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num)
                        {
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_list + par_num-- * 8);
                          if(par_num == 3)
                          {
                              DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(1,2);
                         	  for(uint8_t i = 0 ; i < 3; i++)
                         	  {
                                  DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_list + i*8);
                                  if(i == 2)DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)tr_lfo_t_list + prog_data[tr_lfo_t] * 9);
                                  else {
                                	  if(i < 3)DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i]);
                                	  else DisplayTask->ParamIndic(53,i,prog_data[tr_vol + i - 1]);
                                  }
                         	  }
                         	  DisplayTask->ParamIndic(53,3,prog_data[tr_lfo]);
                          }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)tr_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num){
                	  case 0:if(prog_data[tr_vol])prog_data[tr_vol + par_num] = enc_speed_dec(prog_data[tr_vol + par_num],0);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_vol]);break;
                	  case 1:if(prog_data[tr_rate])prog_data[tr_vol + par_num] = enc_speed_dec(prog_data[tr_vol + par_num],0);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_rate]);break;
                	  case 2:if(prog_data[tr_lfo_t])DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)tr_lfo_t_list + --prog_data[tr_lfo_t]*9);break;
                	  case 3:if(prog_data[tr_lfo])prog_data[tr_vol + par_num - 1] = enc_speed_dec(prog_data[tr_vol + par_num - 1],0);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_lfo]);break;
                	  case 4:if(prog_data[tr_m_s])DisplayTask->StringOut(53,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_t_list + --prog_data[tr_m_s]*7 + 14);break;
                	  case 5:if(prog_data[t_tap_t])DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + --prog_data[t_tap_t]*6);break;
                	  }
                	  if(par_num != 5)gui_send(10,par_num);
                  }
                  clean_flag();
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num < 5)
                        {
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_list + par_num++ * 8);
                          if(par_num == 4)
                          {
                              DisplayTask->Clear();
                         	  DisplayTask->Icon_Strel(1,1);
                         	  DisplayTask->StringOut(53,0,TDisplayTask::fntSystem,0,(uint8_t*)tr_t_list + prog_data[tr_m_s]*7 + 14);
                         	  DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + prog_data[t_tap_t]*6);
                         	  DisplayTask->StringOut(6,1,TDisplayTask::fntSystem,0,(uint8_t*)tr_list + (par_num + 1) * 8);
                          }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)tr_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                	  switch(par_num){
                	  case 0:if(prog_data[tr_vol] < 127)prog_data[tr_vol + par_num] = enc_speed_inc(prog_data[tr_vol + par_num],127);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_vol]);break;
                	  case 1:if(prog_data[tr_rate] < 127)prog_data[tr_vol + par_num] = enc_speed_inc(prog_data[tr_vol + par_num],127);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_rate]);break;
                	  case 2:if(prog_data[tr_lfo_t] < 2)DisplayTask->StringOut(53,2,TDisplayTask::fntSystem,0,(uint8_t*)tr_lfo_t_list + ++prog_data[tr_lfo_t]*9);break;
                	  case 3:if(prog_data[tr_lfo] < 127)prog_data[tr_vol + par_num - 1] = enc_speed_inc(prog_data[tr_vol + par_num - 1],127);DisplayTask->ParamIndic(53,par_num & 3,prog_data[tr_lfo]);break;
                	  case 4:if(prog_data[tr_m_s] < 1)prog_data[tr_m_s]++;DisplayTask->StringOut(53,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_t_list + prog_data[tr_m_s]*7 + 14);break;
                	  case 5:if(prog_data[t_tap_t] < 4)DisplayTask->StringOut(53,1,TDisplayTask::fntSystem,0,(uint8_t*)tap_tim + ++prog_data[t_tap_t]*6);break;
                	  }
                	  if(par_num != 5)gui_send(10,par_num);
                  }
                  clean_flag();
                }
            }
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                    {
                      enc_knob_fl = 1;
                      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)tr_list + par_num * 8);
                    }
                  else {
                      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)tr_list + par_num * 8);
                      enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
          if(k_down == 1)
           {
        	  if(tap_temp_global() && !sys_para[tap_typ])
        	  {
        		  trem_time = tap_global / 48.0f / tap_tim_v[prog_data[t_tap_t]];
        		  if(trem_time < 2731)
        		  {
        			  if(!sys_para[tim_type])gui_send(10,5);
        			  else {
          				  if(trem_time < 2728 && trem_time > 249)
          				  {
          					  while(trem_time < bpm_time[temp++]);
          					  trem_time = bpm_time[temp];
          	      			  gui_send(10,5);
          				  }
        			  }
        		  }

        	  }
              clean_flag();
           }
        if(k_up == 1)
          {
            condish = menu_main;
            gui_send(15,0);
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//----------------------------------------------------------------------Cabinet Type------------------------------

                case cabinet_type:
                    if(tim5_fl == 1)DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)cab_list_menu + par_num*9);
                    else DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)cab_list_menu + par_num*9);
                    if(encoder_state1 == 1)
                    {
                    	if(encoder_state == 1)
                    	{
                    		if(par_num)
                    		{
                                DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)cab_list_menu + par_num--*9);
                                DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)cab_list_menu + par_num*9);
                                tim5_start(0);
                    		}
                    	}
                    	if(encoder_state == 2)
                    	{
                    		if(!par_num)
                    		{
                                DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)cab_list_menu + par_num++*9);
                                DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)cab_list_menu + par_num*9);
                                tim5_start(0);
                    		}
                    	}
                    	clean_flag();
                    }
                    if((k_down == 1) || (encoder_knob == 1))
                    {
                    	cab_num = par_num;
                    	DisplayTask->Clear();
                    	if(!par_num)
                    	{
                          if(name_buf[0] == 0)
                          {
                        	  name_run_fl = 0;
                        	  DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                          }
                          else name_run_fl = 1;
                          DisplayTask->ParamIndicPan(72,1,prog_data[cab1_pan]);
                          DisplayTask->ParamIndicNum(42,3,prog_data[vol]);
                          vol_vol = prog_data[vol];
                          gui_send(15,1);
                    	}
                    	else {
                            if(name_buf1[0] == 0)
                            {
                          	  name_run_fl = 0;
                          	  DisplayTask->StringOut(38,0,TDisplayTask::fntSystem , 0 , (uint8_t*)no_loaded);
                            }
                            else name_run_fl = 1;
                            DisplayTask->ParamIndicPan(72,1,prog_data[cab2_pan]);
                            DisplayTask->ParamIndicNum(42,3,prog_data[cab2_vol]);
                            vol_vol = prog_data[cab2_vol];
                            gui_send(15,3);
                    	}
                    	for(uint8_t i = 0 ; i < 3 ; i++)DisplayTask->StringOut(1,i + 1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + i * 8);
                        condish = cabinet_sim;
                        ind_poin = 500;
                        vol_fl = 1;
                        inp_ind_fl = 0;
                        out_ind_fl = 1;
                        if(cab_num && !name_buf1[0])
                        {
                      	   name_run_fl = 0;
                      	   encoder_knob = 1;
                      	   k_down = 0;
                      	   par_num = 1;
                      	   CSTask->Give();
                           tim5_start(0);
                      	   break;
                        }
                        if(!cab_num && !name_buf[0])
                        {
                      	   name_run_fl = 0;
                      	   encoder_knob = 1;
                      	   k_down = 0;
                      	   par_num = 1;
                      	   CSTask->Give();
                           tim5_start(0);
                      	   break;
                        }
                        tim5_start(0);
                        clean_flag();
                    }
                    if(k_up == 1)
                      {
                        condish = menu_main;
                        gui_send(15,0);
                        enc_knob_fl = 0;
                        DisplayTask->Menu_init();
                        tim5_start(0);
                        clean_flag();
                      }
                break;
//----------------------------------------------------------------------Menu Cab Sim-----------------------------------------------
                  case cabinet_sim:
                    if(enc_knob_fl == 0)
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
                  if(encoder_state1 == 1)
                    {
                      if(encoder_state == 1)
                        {
                    	  if(enc_knob_fl == 0)
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
                    	  if(enc_knob_fl == 0)
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
                  if(encoder_knob == 1)
                    {
                      if(enc_knob_fl == 0)
                        {
                    	  if(par_num == 1) {
                    		  extern uint8_t sd_init_fl;
                    		  if(sd_init_fl == 1)
                    			{
                    			  condish = cabinet_browser;
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
                                      condish = cabinet_sim;
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
                                  condish = cabinet_sim;
                    		  }
                    	  }
                    	  else
                    	  {
                              enc_knob_fl = 1;
                              DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,2,(uint8_t*)cab_list + par_num * 8);
                    	  }
                        }
                      else {
                          DisplayTask->StringOut(1,par_num + 1,TDisplayTask::fntSystem,0,(uint8_t*)cab_list + par_num * 8);
                          vol_fl = 1;
                          enc_knob_fl = 0;
                      }
                      tim5_start(0);
                    }
                  if(k_up == 1)
                  {
                	vol_vol = prog_data[pres_lev];
                	enc_knob_fl = 0;
                	gui_send(15,0);
                	if(cab_type != 2)
                	{
                		condish = menu_main;
                		DisplayTask->Menu_init();
                	}
                	else {
                	   DisplayTask->Clear();
                 	   DisplayTask->StringOut(6,0,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu);
                 	   DisplayTask->StringOut(6,1,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list_menu + 9);
                 	   condish = cabinet_type;
                 	   par_num = cab_num;
                	}
                    tim5_start(0);
                  }
                  clean_flag();
                  break;
//-----------------------------------------------------------------------Cab browser------------------------
                  case cabinet_browser:
                    if(encoder_state1 == 1)
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
                    if(encoder_knob == 1)
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
                              	enc_knob_fl = 0;
                              	gui_send(15,0);
                        		condish = menu_main;
                        		prog_data[cab] = 0;
                        		gui_send(18, 0 | (prog_data[cab] << 8));
                        		DisplayTask->Menu_init();
                        		tim5_start(0);
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
                             gui_send(15,1);
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
                             gui_send(15,3);
                          }
                          DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                          DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                          condish = cabinet_sim;
                          par_num = 2;
                          ind_poin = 500;
                          vol_fl = 1;
                          inp_ind_fl = 0;
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
                           	enc_knob_fl = 0;
                           	gui_send(15,0);
                        	condish = menu_main;
                        	prog_data[cab] = 0;
                        	gui_send(18, 0 | (prog_data[cab] << 8));
                        	DisplayTask->Menu_init();
                        	tim5_start(0);
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
                            gui_send(15,1);
                            DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                            DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                            condish = cabinet_sim;
                            par_num = 2;
                            ind_poin = 500;
                            vol_fl = 1;
                            inp_ind_fl = 0;
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
                         gui_send(15,3);
                         DisplayTask->StringOut(1,2,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 8);
                         DisplayTask->StringOut(1,3,TDisplayTask::fntSystem , 0 , (uint8_t*)cab_list + 16);
                         condish = cabinet_sim;
                         par_num = 2;
                         ind_poin = 500;
                         vol_fl = 1;
                         inp_ind_fl = 0;
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
        case preset_write:
          if(tim5_fl == 0)DisplayTask->Prog_ind(prog1);
          else if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
          if(encoder_state1 == 1)
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
          if((k_down == 1) || (encoder_knob == 1))
            {
              DisplayTask->Clear();
              DisplayTask->StringOut(38,2,TDisplayTask::fnt12x13,0,(uint8_t*)write__);
              prog_data[147] = delay_time;
              prog_data[148] = delay_time >> 8;
              eepr_write(prog1);
              send_cab_data(0,prog1 + 1,0);
              if(cab_type == 2)send_cab_data1(0,prog1 + 1);
              prog1 = prog;
              prog_cur = edit_fl = vol_fl = write_fl = num_men = 0;
              condish = menu_main;
              enc_knob_fl = 0;
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
              edit_fl = 0;
              num_men = 0;
              condish = menu_main;
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
//-----------------------------------------------------------Amp_sim-----------------------------------------
        case power_amplif:
            if(!enc_knob_fl)DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,tim5_fl * 2,(uint8_t*)amp_para + par_num*9);
            if(encoder_state1 == 1)
              {
                if(encoder_state == 1)
                  {
                   if(enc_knob_fl)
                   {
                    switch(par_num){
                    case 0:case 2:if(prog_data[am_v + par_num] > 0)
                      {
                    	prog_data[am_v + par_num] = enc_speed_dec(prog_data[am_v + par_num],0);
                    	DisplayTask->ParamIndic(58,par_num%3,prog_data[am_v + par_num]);
                    	gui_send(11,par_num);
                      }break;
                    case 1:if(prog_data[pre_v] > 0)
                      {
                    	prog_data[pre_v] = enc_speed_dec(prog_data[pre_v],0);
                    	DisplayTask->ParamIndic(58,par_num%3,prog_data[pre_v]);
                    	gui_send(4,7);
                      }break;
                    case 3:if(prog_data[am_mode] > 0)
                    	{
                    		DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_t_list + --prog_data[am_mode]*11);
                    		gui_send(23,0);
                    	}break;
                    }
                   }
                   else {
                	   if(par_num > 0)
                	   {
                		   DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)amp_para + par_num--*9);
                		   if(par_num == 2)
                		   {
                	         DisplayTask->Clear();
                	         DisplayTask->Icon_Strel(8,2);
                	         for(uint8_t i = 0 ; i < 3 ; i++)
                	         {
                	          DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)amp_para + i*9);
                	          if(i == 1)DisplayTask->ParamIndic(58,i,prog_data[pre_v]);
                	          else DisplayTask->ParamIndic(58,i,prog_data[am_v + i]);
                	         }
                	         DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
                	         inp_ind_fl = 0;
                	         out_ind_fl = 1;
                	         vol_fl = 0;
                	         ind_poin = 500;
                		   }
                		   DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)amp_para + par_num*9);

                	   }
                   }
                   tim5_start(0);
                  }
                if(encoder_state == 2)
                  {
                    if(enc_knob_fl)
                    {
                     switch(par_num){
                     case 0:case 2:if(prog_data[am_v + par_num] < 127)
                    	 {
                    	 	 prog_data[am_v + par_num] = enc_speed_inc(prog_data[am_v + par_num],127);
                    	 	 DisplayTask->ParamIndic(58,par_num%3,prog_data[am_v + par_num]);
                    	 	 gui_send(11,par_num);
                    	 }break;
                     case 1:if(prog_data[pre_v] < 127)
                     	 {
                    	 	 prog_data[pre_v] = enc_speed_inc(prog_data[pre_v],127);
                    	 	 DisplayTask->ParamIndic(58,par_num%3,prog_data[pre_v]);
                    	 	 gui_send(4,7);
                     	 }break;
                     case 3:if(prog_data[am_mode] < 14)
                    	 {
                    	 	 DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_t_list + ++prog_data[am_mode]*11);
                    	 	 gui_send(23,0);
                    	 }break;
                     }
                    }
                    else {
                    	if(par_num < 3)
                    	{
                    		DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)amp_para + par_num++*9);
                    		if(par_num == 3)
                    		{
                    			out_ind_fl = 0;
                    			DisplayTask->Clear();
                    			DisplayTask->Icon_Strel(8,1);
                    			DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_t_list + prog_data[am_mode]*11);
                    		}
                    		DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)amp_para + par_num*9);

                    	}
                    }
                  }
                tim5_start(0);
                clean_flag();
              }
            if(encoder_knob)
              {
                    if(enc_knob_fl == 0)
                      {
                        enc_knob_fl = 1;
                        DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)amp_para + par_num*9);
                      }
                    else {
                    	DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)amp_para + par_num*9);
                        enc_knob_fl = 0;
                    }
                    tim5_start(1);
                    clean_flag();
              }
          if(k_up == 1)
            {
              condish = menu_main;
              vol_fl = 0;
              enc_knob_fl = 0;
              gui_send(15,0);
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//--------------------------------------------------------------Menu Name-------------------------------------------
        case name_edit:
          if(par_num < 14)DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,tim5_fl*2-enc_knob_fl,imya[par_num]);
          else DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,tim5_fl*2-enc_knob_fl,imya1[(par_num-14)]);
          if(enc_knob_fl) {
        	if(eq_num < 21)
        	{
        	  if(!key_shift)DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,tim5_fl,ascii_low1[eq_num]);
        	  else DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,tim5_fl,ascii_hig1[eq_num]);
        	}
        	else {
        	 if(!key_shift)DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,tim5_fl,ascii_low2[eq_num-21]);
        	 else DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,tim5_fl,ascii_hig2[eq_num-21]);
        	}
          }
          if(encoder_state1 == 1)
            {
             if(encoder_state == 1)
              {
               if(enc_knob_fl == 0)
                {
                 if(par_num > 0)
                  {
                     if(par_num < 14)
                       {
                         DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,0,imya[par_num--]);
                         DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,2,imya[par_num]);
                       }
                     if(par_num == 14)
                       {
                         DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[(par_num-- -14)]);
                         DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,2,imya[par_num]);
                       }
                     if(par_num > 14)
                       {
                         DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[(par_num-- -14)]);
                         DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,2,imya1[(par_num -14)]);
                       }
                  }
                }
               else {
                   if(eq_num < 21)
                   {
                   	if(!key_shift)
                   	{
                   		if(eq_num == 0)
                   		{
                   			DisplayTask->SymbolOut((eq_num)*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num]);
                   			eq_num = 41;
                   			DisplayTask->SymbolOut((eq_num - 21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num - 21]);
                   			nam_sym_temp = ascii_low2[eq_num - 21];
                   		}
                   		else {
                   		 DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num--]);
                   		 DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num]);
                   		 nam_sym_temp = ascii_low1[eq_num];
                   		}
                   	}
                   	else {
                   		if(eq_num == 0)
                   		{
                   			DisplayTask->SymbolOut((eq_num)*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num]);
                   			eq_num = 41;
                   			DisplayTask->SymbolOut((eq_num - 21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num - 21]);
                   			nam_sym_temp = ascii_hig2[eq_num - 21];
                   		}
                   		else {
                   		 DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num--]);
                   		 DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num]);
                   		 nam_sym_temp = ascii_hig1[eq_num];
                   		}
                   	   }
                     }
                   else {
                   	if(eq_num > 20)
                   	{
                   	 if(!key_shift)
                   		 {
                   		  if(eq_num == 21)
                   			  {
                   			   DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num-- -21]);
                   			   DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,1,ascii_low1[eq_num]);
                   			   nam_sym_temp = ascii_low1[eq_num];
                   			  }
                   		  else {
                   			  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num-- -21]);
                   			  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,1,ascii_low2[eq_num-21]);
                   			  nam_sym_temp = ascii_low2[eq_num - 21];
                   		  }
                   		 }
                   	 else {
                  		  if(eq_num == 21)
                  			  {
                  			   DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num-- -21]);
                  			   DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,1,ascii_hig1[eq_num]);
                  			   nam_sym_temp = ascii_hig1[eq_num];
                  			  }
                  		  else {
                  			  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num-- -21]);
                  			  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num-21]);
                  			  nam_sym_temp = ascii_hig2[eq_num - 21];
                  		  }
                   	 }
                   	}
                   }
               }
              }
              if(encoder_state == 2)
               {
                if(enc_knob_fl == 0)
                 {
                  if(par_num < 27)
                   {
                      if(par_num > 13)
                        {
                          DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[(par_num++ -14)]);
                          DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,2,imya1[(par_num -14)]);
                        }
                      if(par_num == 13)
                        {
                          DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,0,imya[par_num++]);
                          DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,2,imya1[(par_num -14)]);
                        }
                      if(par_num < 13)
                        {
                          DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,0,imya[par_num++]);
                          DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,2,imya[par_num]);
                        }
                   }
                  }
                else {
                    if(eq_num < 20)
                    {
                    	if(!key_shift)
                    	{
                    		DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num++]);
                    		DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num]);
                    		nam_sym_temp = ascii_low1[eq_num];
                    	}
                    	else {
                   		    DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num++]);
                   		    DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num]);
                   		    nam_sym_temp = ascii_hig1[eq_num];
                    	}
                    }
                    else {
                    	if(!key_shift)
                    	{
                    	  if(eq_num == 41)
                    	  {
                    		  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num - 21]);
                    		  eq_num = 0;
                    		  DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num]);
                    		  nam_sym_temp = ascii_low1[eq_num];
                    	  }
                    	  else {
                    		  if(eq_num == 20)DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num++]);
                    		  else DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num++ -21]);
                    		  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num-21]);
                    		  nam_sym_temp = ascii_low2[eq_num - 21];
                    	  }
                    	}
                    	else {
                    		if(eq_num == 41)
                    		{
                    		  DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num - 21]);
                      		  eq_num = 0;
                      		  DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num]);
                      		  nam_sym_temp = ascii_hig1[eq_num];
                    		}
                    		else {
                    			if(eq_num == 20)DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num++]);
                    			else DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num++ -21]);
                    			DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num] -21);
                    			nam_sym_temp = ascii_hig2[eq_num - 21];
                    		}
                    	}
                    }
                }
              }
              tim5_start(0);
              clean_flag();
            }
          if(k_down == 1)
            {
        	  if(!enc_knob_fl)
        	  {
               if(par_num < 14)
                {
                  imya[par_num] = 32;
                  DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,1,imya[par_num]);
                }
               else {
                  imya1[par_num-14] = 32;
                  DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,1,imya1[par_num-14]);
               }
        	  }
        	  else {
        		  if(key_shift)
        		  {
        			key_shift = 0;
                  	DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low1);
                  	DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low2);
                  	if(eq_num < 21)nam_sym_temp = ascii_low1[eq_num];
                  	else nam_sym_temp = ascii_low2[eq_num-21];
        		  }
        		  else {
        			  key_shift = 1;
        			  DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_hig1);
        			  DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_hig2);
                      if(eq_num < 21)nam_sym_temp = ascii_hig1[eq_num];
                      else nam_sym_temp = ascii_hig2[eq_num-21];
        		  }
        	  }
        	  tim5_start(0);
              clean_flag();
            }
          if(encoder_knob == 1)
            {
              if(enc_knob_fl == 0)
            	  {
            	    enc_knob_fl = 1;
            	    if(par_num < 14)DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,1,imya[par_num]);
            	    else DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,1,imya1[par_num-14]);
            	  }
              else {
            	  enc_knob_fl = 0;
            	  if(par_num < 14)
            		  {
            		    imya[par_num] = nam_sym_temp;
            		    DisplayTask->SymbolOut(par_num*6+2,0,TDisplayTask::fntSystem,0,imya[par_num]);
            		  }
            	  else {
            		  imya1[par_num-14] = nam_sym_temp;
            		  DisplayTask->SymbolOut((par_num-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[par_num-14]);
            	  }
             	 if(!key_shift)
             		 {
             		  if(eq_num < 21)DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_low1[eq_num]);
             		  else DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[eq_num -21]);
             		 }
             	 else {
             		 if(eq_num < 21)DisplayTask->SymbolOut(eq_num*6,2,TDisplayTask::fntSystem,0,ascii_hig1[eq_num]);
             		 else DisplayTask->SymbolOut((eq_num-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[eq_num -21]);
             	 }
              }
              tim5_start(0);
              clean_flag();
            }
          if((k_att == 1) || (k_master_eq == 1) || (tuner_flag == 1) || (k_master == 1))
          {
        	  if(tuner_flag == 1)num_men = 15;
              condish = menu_main;
              enc_knob_fl = 0;
              tim5_start(0);
          	  CSTask->Give();
          	  break;
          }
          if((k_up == 1) || (k_sys == 1))
            {
        	  sub_men = 0;
        	  eq_num = prog;
              condish = menu_main;
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//----------------------------------------------------Menu controllers---------------------------------------------
        case controllers:
          if(enc_knob_fl == 0)
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
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
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
                  if(enc_knob_fl == 0)
                    {
                	 if(par_num < 6)
                	 {
                         DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num++ * 7);
                         if(par_num == 3)
                         {
                        	 DisplayTask->Clear();
                        	 DisplayTask->Icon_Strel(7,1);
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
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                    {
                      enc_knob_fl = 1;
                      if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
                      else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,2,(uint8_t*)contr_list + par_num*7);
                    }
                  else {
                	  if(par_num < 6)DisplayTask->StringOut(0,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
                	  else DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)contr_list + par_num*7);
                      enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
          if((k_att == 1) || (k_sys == 1) || (tuner_flag == 1) || (k_master == 1))
          {
        	  if(tuner_flag == 1)num_men = 14;
              condish = menu_main;
              enc_knob_fl = 0;
              tim5_start(0);
          	  CSTask->Give();
          	  break;
          }
          if((k_up == 1) || (k_master_eq == 1))
          {
        	sub_men = 0;
            condish = menu_main;
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//------------------------------------------System menu---------------------------------------------------------
        case sys_menu:
          if(!enc_knob_fl)DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,tim5_fl * 2,(uint8_t*)sys_menu_list + par_num*12);

          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num > 0)
                        {
                          DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + par_num--*12);
                          if(par_num == 3)
                          {
                        	  DisplayTask->Clear();
                        	  DisplayTask->Icon_Strel(11,2);
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
                    			  DisplayTask->Icon_Strel(11,3);
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
                  if(enc_knob_fl == 0)
                    {
                	  if(par_num < 11)
                	  {
                		  DisplayTask->StringOut(3,par_num%4,TDisplayTask::fntSystem,0,(uint8_t*)sys_menu_list + par_num++*12);
                		  if(par_num == 4)
                		  {
                			  DisplayTask->Clear();
                			  DisplayTask->Icon_Strel(11,3);
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
                				  DisplayTask->Icon_Strel(11,1);
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
        		  condish = exp_menu;
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
        	      enc_knob_fl = 0;
        	  }
        	  else {
        		  if((par_num == 8) && (sys_para[tun_ext] & 0x80))
        		  {
        			  condish = tuner_ext_menu;
        			  DisplayTask->Clear();
        			  DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)"CC#");
        			  DisplayTask->ParamIndicNum(50,0,sys_para[tun_ext] & 0x7f);
        		  }
        	  }
              clean_flag();
          }
          if(encoder_knob == 1)
            {
        	  switch(par_num){
        	  case 4:condish = fsw_typ;DisplayTask->Clear();
        	      for(uint8_t i = 0 ; i < 4 ; i++)DisplayTask->StringOut(3,i,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + i*12);
        		  eq_num = 0;encoder_knob = 0;DisplayTask->ParamIndic(58,3,sys_para[foot_sp]);break;
        	  case 6:condish = midi_pc_menu;DisplayTask->Clear();eq_num = 0;encoder_knob = 0;
        	      DisplayTask->ParamIndicNum(3,0,eq_num + 1);
        	      DisplayTask->StringOut(24,0,TDisplayTask::fntSystem,0,(uint8_t*)"->");
        	      DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);break;
        	  default:if(enc_knob_fl == 0)
                  {
                	  enc_knob_fl = 1;
                	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)sys_menu_list + par_num*12);
                  }
                  else {
                	  enc_knob_fl = 0;
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
            edit_fl = 0;
            enc_knob_fl = 0;
            eq_num = prog;
            condish = menu_main;
            DisplayTask->Main_scr();
            DisplayTask->Prog_ind(prog1);
            if((sys_para[fs1] == 1) || ((sys_para[fs11] == 1) && sys_para[fsm1]))DisplayTask->IndFoot(0,contr_kn[0]);
            if((sys_para[fs2] == 1) || ((sys_para[fs21] == 1) && sys_para[fsm2]))DisplayTask->IndFoot(1,contr_kn[1]);
            if((sys_para[fs3] == 1) || ((sys_para[fs31] == 1) && sys_para[fsm3]))DisplayTask->IndFoot(2,contr_kn[2]);
        	tim5_start(0);
            clean_flag();
          }
        if((k_att == 1) || (k_master == 1) || (tuner_flag == 1) || (k_master_eq == 1))
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
        	condish = menu_main;
        	eq_num = prog;
        	write_sys();
        	enc_knob_fl = 0;
        	edit_fl = 0;
        	DisplayTask->Clear();
        	tim5_start(0);
        	CSTask->Give();
        	break;
        }
        break;
//-----------------------------------------------------Expression-------------------------------------------
        case exp_menu:
        if(enc_knob_fl == 0)
        {
          if(tim5_fl == 1)DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);
          else DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num*10);
        }
        if(encoder_state1)
        {
        	if(encoder_state == 1)
        	{
        		if(!enc_knob_fl)
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
        		if(!enc_knob_fl)
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
        if(encoder_knob)
        {
        	if(!enc_knob_fl)
        	{
        		enc_knob_fl = 1;
        		switch(eq_num){
        		case 0:DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);break;
        		case 1:DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_min);
        		condish = adc_calibrate;
        		tim5_start(0);
        		break;
        		case 2:DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)expr_menu + eq_num*10);break;
        		}
        	}
        	else {
        		enc_knob_fl = 0;
        		DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu + eq_num*10);
        	}
            clean_flag();
        }
        if(k_up == 1)
        {
          condish = sys_menu;
          enc_knob_fl = 0;
      	  DisplayTask->Clear();
      	  DisplayTask->Icon_Strel(11,2);
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
        case adc_calibrate:
   		 if(tim5_fl == 1)
   		 {
   			 if(calib_fl)DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 2 , (uint8_t*)set_max);
   			 else DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 2 , (uint8_t*)set_min);
   		 }
   		 else {
   			 if(calib_fl)DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_max);
   			 else DisplayTask->StringOut(67,1,TDisplayTask::fntSystem , 0 , (uint8_t*)set_min);
   		 }
    	 if(encoder_knob == 1)
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
    	         condish = exp_menu;
    	         enc_knob_fl = 0;
    		 }
    		 tim5_start(0);
    	     clean_flag();
    	     adc_calib();
    	 }
      	break;
//---------------------------------------------------Attenuator--------------------------------------------------
        case att_menu:
            if(encoder_state1 == 1)
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
            if((k_master == 1) || (k_sys == 1) || (tuner_flag == 1) || (k_master_eq == 1))
            {
            	condish = menu_main;
            	write_sys();
            	enc_knob_fl = 0;
            	edit_fl = 0;
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_att == 1))
              {
            	write_sys();
            	edit_fl = 0;
                condish = menu_main;
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
        case master_v:
            if(enc_knob_fl == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                else DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num*14);
              }
            if(encoder_state1 == 1)
              {
                if(encoder_state == 1)
                {
                	if(enc_knob_fl)
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
                	if(enc_knob_fl)
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
            if(encoder_knob == 1)
              {
                if(enc_knob_fl == 0)
                {
              	  enc_knob_fl = 1;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)master_vo + par_num*14);
                }
                else {
              	  enc_knob_fl = 0;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)master_vo + par_num*14);
                }
                tim5_start(1);
                clean_flag();
              }
            if((k_att == 1) || (k_sys == 1) || (tuner_flag == 1) || (k_master_eq == 1))
            {
            	condish = menu_main;
            	write_sys();
            	enc_knob_fl = 0;
            	edit_fl = 0;
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_master == 1))
              {
            	write_sys();
            	edit_fl = 0;
            	enc_knob_fl = 0;
                condish = menu_main;
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
        case master_eq:
          if(sys_para[120])
          {
            if(enc_knob_fl == 0)
              {
                if(tim5_fl == 1)DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
                else DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,0,(uint8_t*)mas_eq_list + par_num*9);
              }
            if(encoder_state1 == 1)
            {
            	if(encoder_state == 1)
            	{
            		if(enc_knob_fl)
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
            		if(enc_knob_fl)
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
            if(encoder_knob == 1)
              {
                if(enc_knob_fl == 0)
                {
              	  enc_knob_fl = 1;
              	  DisplayTask->StringOut(3,par_num,TDisplayTask::fntSystem,2,(uint8_t*)mas_eq_list + par_num*9);
                }
                else {
              	  enc_knob_fl = 0;
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
            	condish = menu_main;
            	k_down = 0;
            	k_master_eq = 1;
            	write_sys();
            	enc_knob_fl = 0;
            	edit_fl = 0;
            	gui_send(18, 14 | (sys_para[120] << 8));
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_att == 1) || (k_sys == 1) || (tuner_flag == 1) || (k_master == 1))
            {
            	condish = menu_main;
            	write_sys();
            	enc_knob_fl = 0;
            	edit_fl = 0;
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	break;
            }
            if((k_up == 1) || (k_master_eq == 1))
              {
            	write_sys();
            	edit_fl = 0;
            	enc_knob_fl = 0;
                condish = menu_main;
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
        case tuner_menu:
        	if(encoder_state1)
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
        	if((k_up == 1) || (tuner_flag == 1))
              {
           	    tuner_use = 0;
                condish = menu_main;
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
            	condish = menu_main;
            	enc_knob_fl = 0;
            	edit_fl = 0;
            	DisplayTask->Clear();
            	tim5_start(0);
            	CSTask->Give();
            	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
            	send_codec(0xa103);
            	break;
            }
            break;
//-------------------------------------------------------Cab Name---------------------------------------------
        case cab_name:
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
            if(encoder_state1 == 1)
            {
        		condish = menu_main;
            	CSTask->Give();
            	break;
            }
        	if(k_up == 1)
        	{
        		condish = menu_main;
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
        case fsw_typ:
            if(!enc_knob_fl)
              {
            	if(tim5_fl)DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            	else DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,0,(uint8_t*)footsw_menu + eq_num*12);
              }
            if(encoder_state1)
            {
            	if(encoder_state == 1)
            	{
            	  if(!enc_knob_fl)
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
              	  if(!enc_knob_fl)
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
            if(encoder_knob == 1)
            {
              if(eq_num < 3)
              {
            	DisplayTask->Clear();
            	condish = sw_mode;
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
            	  if(!enc_knob_fl)
            	  {
            		  enc_knob_fl = 1;
            		  DisplayTask->StringOut(3,eq_num,TDisplayTask::fntSystem,2,(uint8_t*)footsw_menu + eq_num*12);
            	  }
            	  else {
            		  enc_knob_fl = 0;
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
   	            condish = sys_menu;
   	            DisplayTask->Icon_Strel(11,1);
   	            enc_knob_fl = 0;
   	            tim5_start(0);
   	            clean_flag();
        	}
        	break;
//--------------------------------------------------Foot SW mode----------------------------------------------------------------
        case sw_mode:
            if(!enc_knob_fl)
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
            if(encoder_state1)
            {
            	if(encoder_state == 1)
            	{
            		if(!enc_knob_fl)
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
            		if(!enc_knob_fl)
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
            if(encoder_knob == 1)
            {
            	if(enc_knob_fl)
            	{
            		enc_knob_fl = 0;
            		if(!par_num1)DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            		else DisplayTask->StringOut(10 + (par_num1-1)*70,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + ((par_num1-1) * 10));
            	}
            	else {
            		if(!par_num1)
            		{
            			DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,2,(uint8_t*)mode_);
                		enc_knob_fl = 1;
            		}
            		else {
                    	DisplayTask->Clear();
                    	par_num2 = 0;
                    	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)expr_menu);
                       if(par_num1 == 1)
                       {
                       	condish = sw_sel;
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
                       	condish = sw_sel1;
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
   			    condish = fsw_typ;
   	            enc_knob_fl = 0;
   	            tim5_start(0);
   	            clean_flag();
        	}
        	break;
//--------------------------------------------------Foot SW set-----------------------------------------------------------------
        case sw_sel:
            if(!enc_knob_fl)
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
            if(encoder_state1)
            {
            	if(encoder_state == 1)
            	{
            		if(!enc_knob_fl)
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
            		if(!enc_knob_fl)
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
            if(encoder_knob)
            {
            	if(enc_knob_fl)
            	{
            		enc_knob_fl = 0;
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
            		enc_knob_fl = 1;
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
            	condish = sw_mode;
            	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + sys_para[fsm1 + eq_num]*7);
            	if(sys_para[fsm1 + eq_num])
            	{
            		DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            		DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            	}
            	tim5_start(0);
                clean_flag();
                enc_knob_fl = 0;
            }
        	break;
//-----------------------------------------------------SW sel1--------------------------------------------------
        case sw_sel1:
            if(!enc_knob_fl)
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
            if(encoder_state1)
            {
            	if(encoder_state == 1)
            	{
            		if(!enc_knob_fl)
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
            		if(!enc_knob_fl)
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
            if(encoder_knob)
            {
            	if(enc_knob_fl)
            	{
            		enc_knob_fl = 0;
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
            		enc_knob_fl = 1;
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
            	condish = sw_mode;
            	DisplayTask->StringOut(3,0,TDisplayTask::fntSystem,0,(uint8_t*)mode_);
            	DisplayTask->StringOut(40,0,TDisplayTask::fntSystem,0,(uint8_t*)switch_mod + sys_para[fsm1 + eq_num]*7);
            	if(sys_para[fsm1 + eq_num])
            	{
            		DisplayTask->StringOut(10,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow);
            		DisplayTask->StringOut(80,2,TDisplayTask::fntSystem,0,(uint8_t*)switch_slow + 10);
            	}
            	tim5_start(0);
                clean_flag();
                enc_knob_fl = 0;
            }
        	break;
//-----------------------------------------------------Copy-----------------------------------------------------
        case copy_men:
          if(tim5_fl == 0)DisplayTask->Prog_ind(copy_temp);
          else if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(copy_temp > 0)
                    {
                	  copy_temp -=1;
                    }
                  else copy_temp = 98;
                }
              if(encoder_state == 2)
                {
                  if(copy_temp < 98)
                    {
                	  copy_temp +=1;
                    }
                  else copy_temp = 0;
                }
              eepr_read_imya(copy_temp);
              read_prog_temp(copy_temp);
              for(uint8_t i = 0 ; i < 15 ; i++)imya_t[i] = preset_temp[i];
              for(uint8_t i = 0 ; i < 15 ; i++)imya1_t[i] = preset_temp[15 + i];
              imya_temp = 1;
              DisplayTask->Main_scr();
              DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");
              DisplayTask->Prog_ind(copy_temp);
              TIM_SetCounter(TIM6,0x8000);
              TIM_ClearFlag(TIM6,TIM_FLAG_Update);
              TIM_Cmd(TIM6,ENABLE);
              tim5_start(0);
              clean_flag();
            }
          if((k_down == 1) || (encoder_knob == 1))
            {
              DisplayTask->Clear();
              DisplayTask->StringOut(38,2,TDisplayTask::fnt12x13,0,(uint8_t*)"Copy OK!");
              switch(num_men){
              case 0:for(uint8_t i = 0 ; i < 11 ; i++)preset_temp[30 + mog_vol + i] = prog_data[mog_vol + i];
                     preset_temp[30 + moog] = prog_data[moog];
                     preset_temp[30 + mog_gen_t] = prog_data[mog_gen_t];
                     break;
              case 1:for(uint8_t i = 0 ; i < 3 ; i++)preset_temp[30 + gate_thr + i] = prog_data[gate_thr + i];
                     preset_temp[30 + gate] = prog_data[gate];break;
              case 2:for(uint8_t i = 0 ; i < 5 ; i++)preset_temp[30 + comp_thr + i] = prog_data[comp_thr + i];
                     preset_temp[30 + compr] = prog_data[compr];break;
              case 3:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + pre_gain + i] = prog_data[pre_gain + i];
              	     preset_temp[30 + pream] = prog_data[pream];
              	  	 break;
              case 4:for(uint8_t i = 0 ; i < 3 ; i++)preset_temp[30 + am_v + i] = prog_data[am_v + i];
                     preset_temp[30 + pre_v] = prog_data[pre_v];preset_temp[30 + amp] = prog_data[amp];break;
              case 5:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + vol + i] = prog_data[vol + i];
                     for(uint8_t i = 0 ; i < 64 ; i++)preset_temp[13344 + i] = name_buf[i];
                     for(uint8_t i = 0 ; i < 64 ; i++)preset_temp[25696 + i] = name_buf1[i];
                     for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[1056 + i] = cab_data[i];
                     if(cab_type == 2)for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[13408 + i] = cab_data1[i];
                     else for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[25760 + i] = cab_data1[i];
                     for(uint16_t i = 0 ; i < 512 ; i++)preset_temp[38048 + i] = impulse_path[i];
                     send_cab_data(0,copy_temp + 1,0);
                     if(cab_type == 2)send_cab_data1(0,copy_temp + 1);
                     preset_temp[30 + cab] = prog_data[cab];
                     break;
              case 6:for(uint8_t i = 0 ; i < 8 ; i++)preset_temp[30 + eq1 + i] = prog_data[eq1 + i];
                     for(uint8_t i = 0 ; i < 10 ; i++)preset_temp[30 + f1 + i] = prog_data[f1 + i];
                     preset_temp[30 + eq] = prog_data[eq];
                     preset_temp[30 + eq_pr_po] = prog_data[eq_pr_po];
                     break;
              case 7:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + phaser_vol + i] = prog_data[phaser_vol + i];
                     preset_temp[30 + phas] = prog_data[phas];
                     preset_temp[30 + hpf_ph] = prog_data[hpf_ph];
                     preset_temp[30 + phas_pos] = prog_data[phas_pos];
                     break;
              case 8:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + fl_v + i] = prog_data[fl_v + i];
                     preset_temp[30 + fl] = prog_data[fl];
                     preset_temp[30 + hpf_fl] = prog_data[hpf_fl];
                     preset_temp[30 + flan_pos] = prog_data[flan_pos];
                     break;
              case 9:for(uint8_t i = 0 ; i < 5 ; i++)preset_temp[30 + chor_volum + i] = prog_data[chor_volum + i];
              	     preset_temp[30 + chor] = prog_data[chor];preset_temp[30 + hpf_ch] = prog_data[hpf_ch];
                     break;
              case 10:for(uint8_t i = 0 ; i < 11 ; i++)preset_temp[30 + d_vol + i] = prog_data[d_vol + i];
                     preset_temp[1054] = delay_time;preset_temp[1055] = delay_time >> 8;
                     preset_temp[30 + deley] = prog_data[deley];
                     preset_temp[30 + d_tap_t] = prog_data[d_tap_t];
                     preset_temp[30 + d_tail] = prog_data[d_tail];
                     break;
              case 11:for(uint8_t i = 0 ; i < 2 ; i++)preset_temp[30 + early_vol + i] = prog_data[early_vol + i];
              	  	 preset_temp[30 + early] = prog_data[early];
                     break;
              case 12:for(uint8_t i = 0 ; i < 7 ; i++)preset_temp[30 + r_vol + i] = prog_data[r_vol + i];
              	  	 preset_temp[30 + reve] = prog_data[reve];
              	  	 preset_temp[30 + rev_t] = prog_data[rev_t];
              	     preset_temp[30 + rev_di] = prog_data[rev_di];
              	     preset_temp[30 + r_pre] = prog_data[r_pre];
              	     preset_temp[30 + r_tail] = prog_data[r_tail];
                     break;
              case 13:for(uint8_t i = 0 ; i < 4 ; i++)preset_temp[30 + tr_vol + i] = prog_data[tr_vol + i];
              	  	 preset_temp[30 + trem] = prog_data[trem];
              	  	 preset_temp[30 + t_tap_t] = prog_data[t_tap_t];
              	  	 preset_temp[30 + tr_lfo_t] = prog_data[tr_lfo_t];
                     break;
              }
              switch(sub_men){
              case 1:preset_temp[30 + pres_lev] = prog_data[pres_lev];num_men = num_men_temp;
                      break;
              case 2:for(uint16_t i = 0 ; i < 512 ; i++)preset_temp[542 + i] = control[i];num_men = num_men_temp;
              	     break;
              case 3:for(uint8_t i = 0 ; i < 15 ; i++)preset_temp[i] = imya[i];
                      for(uint8_t i = 0 ; i < 15 ; i++)preset_temp[15 + i] = imya1[i];num_men = num_men_temp;
                      break;
              }
              imya_temp = 0;
              sub_men = 0;
              send_cab_data(1,copy_temp + 1,1);
              write_prog_temp(copy_temp);
              condish = menu_main;
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          if(k_up == 1)
            {
        	  imya_temp = 0;
        	  sub_men = 0;
              condish = menu_main;
              enc_knob_fl = 0;
              DisplayTask->Menu_init();
              tim5_start(0);
              clean_flag();
            }
          break;
//----------------------------------------------------Preamp menu----------------------------------------------
        case pre_menu:
            if(!enc_knob_fl)DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , tim5_fl*2 , (uint8_t*)pre_para + par_num*7);
            if(encoder_state1)
              {
                if(encoder_state == 1)
                {
                	if(enc_knob_fl)
                	{
                		temp = prog_data[pre_gain + par_num];
                		if(temp > 0)
                		{
                			temp = enc_speed_dec(temp,0);
                			prog_data[pre_gain + par_num] = temp;
                			DisplayTask->ParamIndic(58,par_num%3,temp);
                			gui_send(27,par_num);
                		}
                	}
                	else {
                		if(par_num > 0)
                		{
                			DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para + par_num--*7);
                 		    if(par_num == 2)
                 		    {
                 			  par_num--;
                 	          DisplayTask->Clear();
                 	          DisplayTask->Icon_Strel(12,2);
                 	          DisplayTask->StringOut(3,0,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para);
                 	          DisplayTask->ParamIndic(58,0,prog_data[pre_gain]);
                 	          DisplayTask->ParamIndic(58,1,prog_data[pre_volume]);
                 	          DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_outp + 7);
                 	          inp_ind_fl = 0;
                 	          out_ind_fl = 1;
                 	          vol_fl = 0;
                 	          ind_poin = 500;
                 		    }
                			DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 2 , (uint8_t*)pre_para + par_num*7);
                		}
                	}
                }
                if(encoder_state == 2)
                {
                	if(enc_knob_fl)
                	{
                		temp = prog_data[pre_gain + par_num];
                		if(temp < 127)
                		{
                			temp = enc_speed_inc(temp,127);
                			prog_data[pre_gain + par_num] = temp;
                			DisplayTask->ParamIndic(58,par_num%3,temp);
                			gui_send(27,par_num);
                		}
                	}
                	else {
                		if(par_num < 5)
                		{
                			DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para + par_num++*7);
                			if(par_num == 2)
                			{
                				par_num++;
                    			out_ind_fl = 0;
                    			DisplayTask->Clear();
                    			DisplayTask->Icon_Strel(12,1);
                    			for(uint8_t i = 0 ; i < 3 ; i++)
                    			{
                    				DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para + (par_num+i)*7);
                    				DisplayTask->ParamIndic(58,i,prog_data[pre_gain + par_num + i]);
                    			}
                			}
                			DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 2 , (uint8_t*)pre_para + par_num*7);
                		}
                	}
                }
                tim5_start(0);
                clean_flag();
              }
            if(encoder_knob)
            {
            	if(enc_knob_fl)
            	{
            		DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 0 , (uint8_t*)pre_para + par_num*7);
            		enc_knob_fl = 0;
            	}
            	else {
            		DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem , 2 , (uint8_t*)pre_para + par_num*7);
            		enc_knob_fl = 1;
            	}
            	tim5_start(0);
                clean_flag();
            }
            if(k_up == 1)
              {
                condish = menu_main;
                enc_knob_fl = 0;
                DisplayTask->Menu_init();
                tim5_start(0);
                clean_flag();
              }
        	break;
//----------------------------------------------------------------Default preset----------------------------------
        case erase_menu:
        	if(par_num)DisplayTask->StringOut(78,0,TDisplayTask::fntSystem , tim5_fl*2 , (uint8_t*)"Yes");
        	else DisplayTask->StringOut(30,0,TDisplayTask::fntSystem , tim5_fl*2 , (uint8_t*)"No");
        	if(encoder_state1)
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
        	if(encoder_knob)
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
                    condish = menu_main;
                    DisplayTask->Menu_init();
            	}
            	else {
                    condish = menu_main;
                    DisplayTask->Menu_init();
            	}
            	tim5_start(0);
                clean_flag();
        	}
            if(k_up == 1)
              {
                condish = menu_main;
                DisplayTask->Menu_init();
                tim5_start(0);
                clean_flag();
              }
        	break;
//---------------------------------------------------------Gate---------------------------------------------------------
        case gate_menu:
          if(enc_knob_fl == 0)
          {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)gate_list + par_num*7);
              else DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gate_list + par_num*7);
          }
          if(encoder_state1 == 1)
          {
              if(encoder_state == 1)
              {
                  if(enc_knob_fl == 0)
                  {
                      if(par_num > 0)
                      {
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gate_list + par_num-- * 7);
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)gate_list + par_num * 7);
                          tim5_start(0);
                      }
                  }
                  else {
                      if(prog_data[gate_thr + par_num] > 0)
                      {
                    	prog_data[gate_thr + par_num] = enc_speed_dec(prog_data[gate_thr + par_num],0);
                    	DisplayTask->ParamIndic(53,par_num,prog_data[gate_thr + par_num]);
                    	gui_send(29,par_num);
                      }
                  }
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num < 2)
                        {
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gate_list + par_num++ * 7);
                          DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)gate_list + par_num * 7);
                          tim5_start(0);
                        }
                    }
                  else {
                	if(prog_data[gate_thr + par_num] < 127)
                	{
                		 prog_data[gate_thr + par_num] = enc_speed_inc(prog_data[gate_thr + par_num],127);
                		 DisplayTask->ParamIndic(53,par_num,prog_data[gate_thr + par_num]);
                		 gui_send(29,par_num);
                	}
                  }
                }
              clean_flag();
            }
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                    {
                      enc_knob_fl = 1;
                      DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,2,(uint8_t*)gate_list + par_num * 7);
                    }
                  else {
                      DisplayTask->StringOut(6,par_num,TDisplayTask::fntSystem,0,(uint8_t*)gate_list + par_num * 7);
                      enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
        if(k_up == 1)
          {
            condish = menu_main;
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//-------------------------------------------------------------Compressor--------------------------------------------
        case compr_menu:
          if(enc_knob_fl == 0)
            {
              if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)comp_list + par_num*8);
              else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)comp_list + par_num*8);
            }
          if(encoder_state1 == 1)
            {
              if(encoder_state == 1)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num > 0)
                        {
                   	      DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)comp_list + par_num-- * 8);
                          if(par_num == 3)
                           {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(13,2);
                       	    for(uint8_t i = 0 ; i < 4 ; i++)
                       		{
                       		    DisplayTask->ParamIndic(53,i,prog_data[comp_thr + i]);
                       		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)comp_list + i*8);
                       		}
                           }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)comp_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                  	if(prog_data[comp_thr + par_num] > 0)
                  	{
                        prog_data[comp_thr + par_num] = enc_speed_dec(prog_data[comp_thr + par_num],0);
                      	DisplayTask->ParamIndic(53,par_num & 3,prog_data[comp_thr + par_num]);
                        gui_send(30,par_num);
                    }
                  }
                }
              if(encoder_state == 2)
                {
                  if(enc_knob_fl == 0)
                    {
                      if(par_num < 4)
                        {
                    	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)comp_list + par_num++ * 8);
                          if(par_num == 4)
                          {
                            DisplayTask->Clear();
                       	    DisplayTask->Icon_Strel(13,1);
                       	    DisplayTask->ParamIndic(53,par_num & 3,prog_data[comp_thr + par_num]);
                          }
                          DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)comp_list + par_num * 8);
                          tim5_start(0);
                        }
                    }
                  else {
                     if(prog_data[comp_thr + par_num] < 127)
                     {
                      	prog_data[comp_thr + par_num] = enc_speed_inc(prog_data[comp_thr + par_num],127);
                        DisplayTask->ParamIndic(53,par_num & 3,prog_data[comp_thr + par_num]);
                        gui_send(30,par_num);
                     }
                  }
                }
              clean_flag();
            }
          if(encoder_knob == 1)
            {
                  if(enc_knob_fl == 0)
                  {
                    enc_knob_fl = 1;
                    DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)comp_list + par_num*8);
                  }
                  else {
                  	DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)comp_list + par_num*8);
                    enc_knob_fl = 0;
                  }
                  tim5_start(1);
                  clean_flag();
            }
        if(k_up == 1)
          {
            condish = menu_main;
            gui_send(15,0);
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//------------------------------------------------------MIDI Map----------------------------------------------
        case midi_pc_menu:
            if(!enc_knob_fl)
              {
                if(tim5_fl)DisplayTask->Clear_str(3,0,TDisplayTask::fntSystem,3);
                else DisplayTask->ParamIndicNum(3,0,eq_num + 1);
              }
            else {
                if(tim5_fl)DisplayTask->Clear_str(36,0,TDisplayTask::fntSystem,3);
                else DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
              }
        if(encoder_state1 == 1)
        {
           if(encoder_state == 1)
           {
              if(enc_knob_fl == 0)
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
               if(enc_knob_fl == 0)
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
        if(encoder_knob == 1)
        {
           if(enc_knob_fl == 0)
           {
             enc_knob_fl = 1;
             DisplayTask->ParamIndicNum(3,0,eq_num + 1);
           }
           else {
        	  DisplayTask->ParamIndicNum(36,0,sys_para[eq_num + 128] + 1);
              enc_knob_fl = 0;
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
   	        condish = sys_menu;
   	        DisplayTask->Icon_Strel(11,1);
   	        enc_knob_fl = 0;
   	        tim5_start(0);
   	        clean_flag();
        }
        break;
//----------------------------------------------Moog filter----------------------------------
        case moog_menu:
        if(enc_knob_fl == 0)
          {
            if(tim5_fl == 1)DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)moog_list + par_num*8);
            else DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)moog_list + par_num*8);
          }
        if(encoder_state1 == 1)
          {
            if(encoder_state == 1)
              {
                if(enc_knob_fl == 0)
                  {
              	  if(par_num > 0)
              	  {
              	   DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)moog_list + par_num-- * 8);
                     if(par_num == 3)
                      {
                  	  DisplayTask->Clear();
                  	  DisplayTask->Icon_Strel(15,2);
                  	  for(uint8_t i = 0 ; i < 4 ; i++)
                  		  {
                  		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + i*8);
                      	    switch(i){
                      	    case 0:DisplayTask->ParamIndicMix(53,i,prog_data[mog_vol]);break;
                      	    case 1:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_typ_list + prog_data[mog_type]*4);break;
                      	    case 2:DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_mod_list + prog_data[mog_mod]*4);break;
                      	    case 3:DisplayTask->ParamIndic(53,i,prog_data[mog_rate]);break;
                      	   }
                  		  }
                      }
                     else {
                  	   if(par_num == 7)
                  	   {
                       	  DisplayTask->Clear();
                       	  DisplayTask->Icon_Strel(15,3);
                      	  for(uint8_t i = 0 ; i < 4 ; i++)
                      		{
                      	     DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + (i+4)*8);
                      	     DisplayTask->ParamIndic(53,i,prog_data[mog_vol + 4 + i]);
                      	    }
                  	   }
                     }
                     DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)moog_list + par_num * 8);
                     tim5_start(0);
              	  }
                  }
                else {
                  	  switch(par_num){
                  	  case 0:
                  		if(prog_data[mog_vol + par_num])prog_data[mog_vol] = enc_speed_dec(prog_data[mog_vol],0);
                  		DisplayTask->ParamIndicMix(53,par_num & 3,prog_data[mog_vol]);break;
                  	  case 1:
                  		if(prog_data[mog_vol + par_num])DisplayTask->StringOut(53,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_typ_list + --prog_data[mog_type]*4);
                  	    break;
                  	  case 2:
                  		if(prog_data[mog_vol + par_num])DisplayTask->StringOut(53,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_mod_list + --prog_data[mog_mod]*4);
                  	    break;
                  	  case 11:
                  		if(prog_data[mog_gen_t])DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_gen_type + --prog_data[mog_gen_t] * 7);
                  	    break;
                  	  default:
                  		if(prog_data[mog_vol + par_num])prog_data[mog_vol + par_num] = enc_speed_dec(prog_data[mog_vol + par_num],0);
                  		DisplayTask->ParamIndic(53,par_num & 3,prog_data[mog_vol + par_num]);
                  	    break;
                      }
                  	gui_send(31,par_num);
                }
              }
            if(encoder_state == 2)
              {
                if(enc_knob_fl == 0)
                  {
              	  if(par_num < 11)
              	  {
              	   DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)moog_list + par_num++ * 8);
                     if(par_num == 4)
                     {
                  	  DisplayTask->Clear();
                  	  DisplayTask->Icon_Strel(15,3);
                  	  for(uint8_t i = 0 ; i < 4 ; i++)
                  		  {
                  		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + (i+par_num)*8);
                  		    DisplayTask->ParamIndic(53,i,prog_data[mog_vol + par_num + i]);
                  		  }
                     }
                     else {
                  	   if(par_num == 8)
                  	   {
                       	  DisplayTask->Clear();
                       	  DisplayTask->Icon_Strel(15,1);
                       	  for(uint8_t i = 0 ; i < 4 ; i++)
                       		{
                    		    DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_list + (i+par_num)*8);
                    		    if(i < 3)DisplayTask->ParamIndic(53,i,prog_data[mog_vol + par_num + i]);
                    		    else DisplayTask->StringOut(53,i,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_gen_type + prog_data[mog_gen_t] * 7);
                       		}
                  	   }
                     }
                     DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)moog_list + par_num * 8);
                     tim5_start(0);
              	  }
                  }
                else {
              	  switch(par_num){
              	  case 0:if(prog_data[mog_vol] < 127)
              	  {
              		  prog_data[mog_vol] = enc_speed_inc(prog_data[mog_vol],127);
              		  DisplayTask->ParamIndicMix(53,par_num & 3,prog_data[mog_vol]);
              	  }break;
              	  case 1:
              		if(prog_data[mog_type] < 2)
              	      { DisplayTask->StringOut(53,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_typ_list + ++prog_data[mog_type]*4); }
              	    break;
              	  case 2:if(prog_data[mog_mod] < 2)
              		DisplayTask->StringOut(53,par_num,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_mod_list + ++prog_data[mog_mod]*4);
              	    break;
              	  case 11:if(prog_data[mog_gen_t] < 6)prog_data[mog_gen_t]++;
              	    DisplayTask->StringOut(53,3,TDisplayTask::fntSystem , 0 , (uint8_t*)moog_gen_type + prog_data[mog_gen_t] * 7);
              	    break;
              	  default:if(prog_data[mog_vol + par_num] < 127)
              	  {
              		prog_data[mog_vol + par_num] = enc_speed_inc(prog_data[mog_vol + par_num],127);
              		DisplayTask->ParamIndic(53,par_num & 3,prog_data[mog_vol + par_num]);
              	  }break;
              	  }
              	  gui_send(31,par_num);
              }
          }
          clean_flag();
        }
        if(encoder_knob == 1)
          {
            if(enc_knob_fl == 0)
              {
                  enc_knob_fl = 1;
             	  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,2,(uint8_t*)moog_list + par_num * 8);
              }
            else {
                  enc_knob_fl = 0;
                  DisplayTask->StringOut(6,par_num & 3,TDisplayTask::fntSystem,0,(uint8_t*)moog_list + par_num * 8);
            }
            tim5_start(1);
            clean_flag();
          }
        if(k_down == 1)
         {
        	if(prog_data[mog_gen_t])
        	{
        		moog_time = tap_global / 48.0f;
        		gui_send(31,13);
        	}
           clean_flag();
         }
        if(k_up == 1)
          {
            condish = menu_main;
            enc_knob_fl = 0;
            DisplayTask->Menu_init();
            tim5_start(0);
            clean_flag();
          }
        break;
//-----------------------------------------------------Tuner ext controller----------------------------
        case tuner_ext_menu:
        if(encoder_state1 == 1)
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
           condish = sys_menu;
           DisplayTask->Clear();
           enc_knob_fl = 0;
 		   DisplayTask->Icon_Strel(11,1);
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
//---------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler()
{
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
  if(tim5_fl)tim5_fl = 0;
  else tim5_fl = 1;
  if(!tuner_use)CSTask->Give();
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



