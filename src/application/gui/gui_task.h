#ifndef GU_H_
#define GU_H_

#include "appdefs.h"
#include "../modules.h"
#include "abstractmenu.h"

#include "baseparam.h"


#define IN_OUT_STR_SIZE 7
const uint8_t in_out_strings[][IN_OUT_STR_SIZE] = {"Input ", "Output"};












enum {/*+++switch*/amp = 0,cab,eq,delay,phas,fl,chor,early,reve,trem,pream,gate,compr,moog,
	 /*+++cab_mono*/cab_m = 41,
	 /*+++cab*/vol,cab1_pan,cab1_del,cab2_vol,cab2_pan,cab2_del,
	 /*+++eq*/eq1,eq2,eq3,eq4,eq5,
	 /*+++lpf_hpf_pres*/hpf_v,lpf_v,pre_v,
	 /*+++phas*/phaser_vol,phaser_rate,phaser_wdt,phaser_cnt,phaser_feedback,phaser_type,
	 /*+++chor*/chor_volum,chor_rat,chor_width,chor_del,chor_typ,
   /*+++delay*/d_vol,d_fed,d_lp,d_hp,d_pan,dp_vol,dp_pan,dp_d,d_mod,d_ra,d_dir,
   /*+++flanger*/fl_v,fl_lfo,fl_r,fl_w,fl_d,fl_f,
   /*+++reverb*/r_vol,r_time,r_size,r_dump,r_lp,r_hp,r_det,
   /*+++tremolo*/tr_vol,tr_rate,tr_lfo,tr_m_s,
   /*+++pres_vol*/pres_lev,
   /*+++amp_sim*/am_v,am_mode,am_out,
   /*+++early refl*/early_vol,early_type,
   /*+++eq_b_f*/f1,f2,f3,f4,f5,
   /*+++eq_b_q*/q1,q2,q3,q4,q5,
   /*+++preamp*/pre_gain,pre_volume,pre_0,pre_low,pre_mid,pre_high,
   /*+++gate*/gate_thr,gate_att,gate_dec,
   /*+++compressor*/comp_thr,comp_ra,comp_v,comp_at,comp_kn,
   /*+++HPF_mod*/hpf_ph,hpf_fl,hpf_ch,
   /*+++del_tap*/d_tap_t,
   /*+++tremolo_tap*/t_tap_t,
   /*+++moog*/mog_mix,mog_ftype,mog_fmod,mog_rate,mog_lo,mog_hi,mog_res,mog_th,mog_at,mog_re,mog_vo,mog_ext,
   /*+++rever type*/rev_t, rev_di,
   /*+++moog gen type*/mog_gen_t,
   /*+++volum control*/vol_contr,
   /*+++eq pre post*/eq_pr_po,
   /*+++delay time*/delay_tim_lo,delay_tim_hi,
   /*+++phas pre post*/phas_pos,
   /*+++flan pre post*/flan_pos,
   /*+++rever predelay*/r_pre,
   /*+++foot ind*/fo1,fo2,fo3,fo11,fo12,fo13,
   /*+++trem lfo type*/tr_lfo_t,
   /*+++delay tail*/d_tail,
   /*+++rever tail*/r_tail,
   /*+++bpm del*/bpm_del = 511
   };

//enum {menu_main,power_amplif,cabinet_sim,cabinet_type,cabinet_browser,equaliser,diley,phazer,flanger,chorus,rever,
//	   tremolo,volume,controllers,name_edit,save_pres,preset_write,tuner_menu,sys_menu,early_menu,att_menu,
//	   master_v,adc_calibrate,eq_band,master_eq,cab_name,fsw_menu,fsw_typ,exp_menu,sw_sel,copy_men,pre_menu,erase_menu,
//	   gate_menu,compr_menu,midi_pc_menu,tap_del_menu,sw_mode,sw_sel1,moog_menu,tuner_ext_menu};



typedef union {
	float s;
	uint32_t ss;
}tr_t;

void tim5_start(uint8_t val);
void t_restart(void);
void clean_flag (void);

void GUI_param_select_task(uint8_t** param_strings);
void GUI_enc_param_decrase(uint8_t** param_pts, dsp_module_address_t module_address);
//void GUI_enc_param_decrase(gui_param_t* menu_params);
void GUI_enc_param_incrase(uint8_t** param_pts, dsp_module_address_t module_address);
void GUI_return_to_main_menu();

extern volatile uint8_t encoder_knob_selected;

extern volatile uint8_t write_fl;
extern volatile uint16_t count_enc;
extern uint8_t tim4_fl2;
extern uint16_t trem_time;
extern uint16_t moog_time;
extern uint32_t del_ind;
extern uint32_t del_ind1;
extern uint32_t del_ind2;
extern uint32_t tap_temp;
extern uint32_t tap_temp1;
extern uint32_t tap_temp2;
extern uint32_t tap_global;
extern uint32_t ind_in_p[];
extern uint16_t ind_poin;
extern volatile uint8_t ind_fl;
extern uint32_t ind_out_l[];
extern uint32_t ind_out_r[];
extern volatile uint8_t sram_fl;
extern volatile uint8_t sram_fl1;
extern gui_menu_type current_menu ;
extern float del_vol_ind;
extern float phas_vol_ind;
extern float chor_vol_ind;
extern uint8_t k_up;
extern uint8_t k_down;
extern uint8_t k_sys;
extern uint8_t k_master;
extern volatile uint8_t par_num;
extern float del_fed;
extern float del_vol;
extern float del_vol1;
extern float del_pan;
extern float del_pan1;
extern float del_pan2;
extern float del_pan3;
extern float ear_vol;
extern uint32_t cR;
//extern uint8_t edit_modules_fl;
extern uint8_t tuner_use;
extern const float tap_tim_v[];
extern volatile uint8_t midi_f1;
extern uint8_t tempo_fl;
extern const float tap_moog[];
extern uint8_t impulse_path[];
extern bool cab_data_ready ;
extern volatile uint16_t adc_bu1[];
extern uint8_t tun_del_val;
extern float tun_base_old;
extern uint32_t tap_val;
extern volatile uint16_t mas_eq_fr;

uint8_t tap_temp_global(void);
void gui(void);
void clean_flag (void);
void adc_proc(void);
void comp_para(uint32_t val);
void gate_par(uint32_t val);

void prog_ch(void);

inline int16_t enc_speed_inc(int16_t data,int16_t max)
{
	TIM_Cmd(TIM6,DISABLE);
	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update))data = data + 1;
	else {
		if(TIM_GetCounter(TIM6) > 0x3fff)data += 1;
		else {
			if(TIM_GetCounter(TIM6) > 0x1fff)
			{
				if(data < (max - 1))data += 2;
				else data += 1;
			}
			else {
				if(TIM_GetCounter(TIM6) > 0xfff)
				{
					if(data < (max - 3))data += 4;
					else data += 1;
				}
				else {
					if(TIM_GetCounter(TIM6) > 0x7ff)
					{
						if(data < (max - 7))data += 8;
						else data += 1;
					}
					else {
						if(data < (max - 49))data += 50;
						else data += 1;
					}
				}
			}
		}
	}
	TIM_SetCounter(TIM6,0);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);
	return data;
}

inline uint16_t enc_speed_dec(int16_t data,int16_t min)
{
	TIM_Cmd(TIM6,DISABLE);
	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update))data -= 1;
	else {
		if(TIM_GetCounter(TIM6) > 0x3fff)data -= 1;
		else {
			if(TIM_GetCounter(TIM6) > 0x1fff)
			{
				if(data > (min+1))data -= 2;
				else data -= 1;
			}
			else {
				if(TIM_GetCounter(TIM6) > 0xfff)
				{
					if(data > (min+3))data -= 4;
					else data -= 1;
				}
				else {
					if(TIM_GetCounter(TIM6) > 0x7ff)
					{
						if(data > (min+7))data -= 8;
						else data -= 1;
					}
					else {
						if(data > (min+49))data -= 50;
						else data -= 1;
					}
				}
			}
		}
	}
	TIM_SetCounter(TIM6,0);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);
	return data;
}
#endif /* GU_H_ */
