#include "gui_task.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "filt.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "gui/elements/icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"
#include "spectrum.h"
#include "vdt/vdt.h"

#include "modules.h"

#include "menus/abstractmenu.h"

const uint8_t ef_list[][8] =
		{"Cab Sim", "Volume ", "    Eq ", "Phaser ", "Chorus ", " Delay ", " Early ", "Reverb ", " Name  ", " Write ",
				"Write ?"};


const uint8_t tap_tim[][6] = {"1/1  ", "1/1.5", "1/2  ", "1/3  ", "1/4  ", "2/1  "};
const float tap_time_coefs[6] = {1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 0.5f};

const uint8_t contr_list[][7] =
{"Contr", "Source", "Destin", "MinVal", "MaxVal", "PC Out", "SET"};
const uint8_t cab_list[][8] =
{"Pan", "Browser", "Volume"};

const uint8_t cab_list_menu[][9] =
{"Cabinet1", "Cabinet2"};

const uint8_t mas_eq_list[][9] =
{"Low", "Mid", "Mid Freq", "High"};
const uint8_t switch_mod[][7] =
{"Single", "Double"};
const uint8_t switch_slow[][10] =
{"OnRelease", "OnHold"};
const uint8_t out_pc_list[][8] =
{"MIDI IN", "  MAP  ", "  SET  "};

const uint8_t pres_num_pos[4] =
{58, 46, 37, 28};

const uint8_t exit_[] = "Exit";
const uint8_t volum[] = "Preset Level";
const uint8_t eq_on[] = "Eq on";
const uint8_t eq_off[] = "Eq off";

const uint8_t edit_[] = "Edit";
const uint8_t push[] = "Push to write";
const uint8_t tap[] = "Edit -> Tap";

const uint8_t tremol[] = "Tremolo";
const uint8_t controll[] = "Controllers";
const uint8_t com[] = "Compressor";
const uint8_t cc[] = "CC#";
const uint8_t com_val[] = "Value";
const uint8_t thr_by[] = "TRUE BYPASS";
const uint8_t cab_s[] = "Cabinet simulator";
const uint8_t calib[] = "Calibration";
const uint8_t set_min[] = "Set to min";
const uint8_t set_max[] = "Set to max";
const uint8_t ok[] = "Ok";

const uint8_t dw1[][6] =
{"D<--W", "D-->W", "D<->W"};


const uint8_t mode_[] = "Mode";

uint16_t midi_clk_send;
uint8_t num_men = 0;
uint8_t num_men_temp = 0;
volatile uint8_t prog2;
uint8_t tim5_fl2;
volatile uint8_t par_num = 0;
volatile uint8_t par_num1 = 0;
volatile uint8_t par_num2 = 0;
volatile uint8_t prog_old;
volatile uint8_t calib_fl = 0;
gui_menu_type current_menu_type;
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

void str_pres_init(char *adr, uint16_t val)
{
	if(val<10)
		ksprintf(adr, "0%d", val);
	else
		ksprintf(adr, "%d", val);
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

void clean_flag(void)
{
	k_up = k_down = k_sys = k_att = k_master = k_master_eq = encoder_knob_pressed = encoder_state_updated = k_tuner = 0;
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

extern volatile uint8_t pc_mute_fl;
void prog_ch(void)
{
	gui_send(32, currentPresetNumber);
	eepr_read_prog_data(currentPresetNumber);
	eepr_read_prog(currentPresetNumber);
	pc_mute_fl = 0;

	MidiSendTask->prog_ch_midi_start();
	MidiSendTask->Give();

	for(uint8_t i = 0; i<3; i++)
	{
		contr_kn[i] = presetData[fo1+i];
		contr_kn1[i] = presetData[fo11+i];
	}
	if((sys_para[fs1]==1)||((sys_para[fs11]==1)&&sys_para[fsm1]))
		DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[fs2]==1)||((sys_para[fs21]==1)&&sys_para[fsm2]))
		DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[fs3]==1)||((sys_para[fs31]==1)&&sys_para[fsm3]))
		DisplayTask->IndFoot(2, contr_kn[2]);

	if(sys_para[sav_exp])
		adc_proc();

	pc_mute_fl = 1;
	tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
	for(uint8_t i = 0; i<32; i++)
	{
		if(presetControllers[i*4])
		{
			if(presetControllers[i*4+1]==9)
				tap_del_fl = 1;
			if(presetControllers[i*4+1]==26)
				tap_trem_fl = 1;
			if(presetControllers[i*4+1]==35)
				tap_moog_fl = 1;
		}
	}

}

uint8_t d_w_init(uint8_t val)
{
	uint8_t b;
	if(val<63)
	{
		b = 0;
		val = 63-val;
	}
	else
	{
		b = 1;
		val = val-63;
		if(!val)
			b = 2;
	}
	return b;
}



volatile uint16_t dsaf = 0x8000;
//---------------------------------------------------------GUI----------------------------------------------------
void GUI_return_to_main_menu()
{
	current_menu_type = MENU_MAIN;
	encoder_knob_selected = 0;
	DisplayTask->Menu_init();
	tim5_start(0);
	clean_flag();
}

void gui(void)
{
	currentMenu->task();

	if(encoder_knob_pressed)
		currentMenu->encoderPressed();
	if(encoder_state_updated)
	{
		if(encoder_state==ENC_COUNTERCLOCKWISE_STEP)
		{
			currentMenu->encoderCounterClockwise();
		}

		if(encoder_state==ENC_CLOCKWISE_STEP)
		{
			currentMenu->encoderClockwise();
		}
	}
	if(k_up)
		currentMenu->keyUp();
	if(k_down)
		currentMenu->keyDown();
	if(k_att)
		currentMenu->key1();
	if(k_master)
		currentMenu->key2();
	if(k_master_eq)
		currentMenu->key3();
	if(k_sys)
		currentMenu->key4();
	if(k_tuner)
		currentMenu->key5();

//		clean_flag();
	switch(current_menu_type)
	{
		//----------------------------------------------------Menu controllers---------------------------------------------
		case MENU_CONTROLLERS:

			if(encoder_state_updated==1)
			{
				if(encoder_state==1)
				{
					if(encoder_knob_selected==0)
					{
						if(par_num>0)
						{
							if(par_num<6)
								DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 0,
										(uint8_t*)contr_list+par_num--*7);
							else
								DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0,
										(uint8_t*)contr_list+par_num--*7);
							if(par_num==2)
							{
								DisplayTask->Clear();
								DisplayTask->Strel(58, 3, 0);
								DisplayTask->StringOut(0, 0, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list);
								DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+7);
								for(uint8_t i = 0; i<3; i++)
								{
									DisplayTask->StringOut(0, i, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+i*7);
									switch(i)
									{
										case 0:
											DisplayTask->ParamIndicNum(45, 0, contrs+1);
										break;
										case 1:
											if(!presetControllers[contrs*4])
												DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
														(uint8_t*)cc_of);
											else
											{
												DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
												if(presetControllers[contrs*4]>4)
												{
													DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
															(uint8_t*)cc);
													DisplayTask->ParamIndicNum(69, 1, presetControllers[contrs*4]-5);
												}
												else
													DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
															(uint8_t*)contr_ext_l+(presetControllers[contrs*4]-1)*12);
											}
										break;
										case 2:
											DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
													(uint8_t*)midi_dest_list+(presetControllers[contrs*4+1])*14);
										break;
									}
								}
							}
							DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 2,
									(uint8_t*)contr_list+par_num*7);

						}
					}
					else
					{
						switch(par_num)
						{
							case 0:
								if(contrs>0)
								{
									DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
									DisplayTask->ParamIndicNum(45, 0, --contrs+1);
									if(!presetControllers[contrs*4])
										DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc_of);
									else
									{
										if(presetControllers[contrs*4]>4)
										{
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc);
											DisplayTask->ParamIndicNum(69, 1, presetControllers[contrs*4]-5);
										}
										else
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
													(uint8_t*)contr_ext_l+(presetControllers[contrs*4]-1)*12);
									}
									control_destin = dest_tabl_start[presetControllers[contrs*4+1]];
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
											(uint8_t*)midi_dest_list+(presetControllers[contrs*4+1])*14);
								}
							break;
							case 1:
								if(presetControllers[contrs*4]>0)
								{
									DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
									presetControllers[contrs*4] = enc_speed_dec(presetControllers[contrs*4], 0);
									if(presetControllers[contrs*4]==4)
									{
										if(sys_para[fs3]!=1)
											presetControllers[contrs*4]--;
									}
									if(presetControllers[contrs*4]==3)
									{
										if(sys_para[fs2]!=1)
											presetControllers[contrs*4]--;
									}
									if(presetControllers[contrs*4]==2)
									{
										if(sys_para[fs1]!=1)
											presetControllers[contrs*4]--;
									}
									if(presetControllers[contrs*4]==1)
									{
										if(sys_para[EXPRESSION_TYPE]<3)
											presetControllers[contrs*4]--;
									}
									if(!presetControllers[contrs*4])
										DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc_of);
									else
									{
										if(presetControllers[contrs*4]>4)
										{
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc);
											DisplayTask->ParamIndicNum(69, 1, presetControllers[contrs*4]-5);
										}
										else
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
													(uint8_t*)contr_ext_l+(presetControllers[contrs*4]-1)*12);
									}
								}
							break;
							case 2:
								if(control_destin)
								{
									presetControllers[contrs*4+1] = dest_tabl[--control_destin];
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
											(uint8_t*)midi_dest_list+(presetControllers[contrs*4+1])*14);
								}
							break;
							case 3:
								if(presetControllers[2+contrs*4]>0)
								{
									presetControllers[2+contrs*4] = enc_speed_dec(presetControllers[2+contrs*4], 0);
									DisplayTask->ParamIndic(45, 0, presetControllers[2+contrs*4]);
								}
							break;
							case 4:
								if(presetControllers[3+contrs*4]>0)
								{
									presetControllers[3+contrs*4] = enc_speed_dec(presetControllers[3+contrs*4], 0);
									DisplayTask->ParamIndic(45, 1, presetControllers[3+contrs*4]);
								}
							break;
							case 5:
								if(presetControllers[510]>0)
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,
											(uint8_t*)out_pc_list+--presetControllers[510]*8);
							break;
							case 6:
								uint8_t a = 0;
								if(presetControllers[511]&0x80)
									a = presetControllers[511]&0x7f;
								else
									a = currentPresetNumber;
								if(a>0)
								{
									a = enc_speed_dec(a, 0);
									DisplayTask->ParamIndicNum(45, 3, a+1);
									presetControllers[511] = a|0x80;
								}
							break;
						}
						tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
						for(uint8_t i = 0; i<32; i++)
						{
							if(presetControllers[i*4])
							{
								if(presetControllers[i*4+1]==9)
									tap_del_fl = 1;
								if(presetControllers[i*4+1]==26)
									tap_trem_fl = 1;
								if(presetControllers[i*4+1]==35)
									tap_moog_fl = 1;
							}
						}
					}
				}
				if(encoder_state==2)
				{
					if(encoder_knob_selected==0)
					{
						if(par_num<6)
						{
//							DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+par_num++*7);
							if(par_num==3)
							{
//								DisplayTask->Clear();
//								DisplayTask->Icon_Strel(ICON_CC, STRELKA_UP);
//								DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+(par_num+1)*7);
//								DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+(par_num+2)*7);
//								DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+(par_num+3)*7);
//								DisplayTask->ParamIndic(45, 0, control[2+contrs*4]);
//								DisplayTask->ParamIndic(45, 1, control[3+contrs*4]);
//								DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0,(uint8_t*)out_pc_list+control[510]*8);
//								if(control[511]&0x80)
//									DisplayTask->ParamIndicNum(45, 3, (control[511]&0x7f)+1);
//								else
//									DisplayTask->ParamIndicNum(45, 3, prog+1);
							}
//							if(par_num<6)
//								DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 2, (uint8_t*)contr_list+par_num*7);
//							else
//								DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 2, (uint8_t*)contr_list+par_num*7);
						}
					}
					else
					{
						switch(par_num)
						{
							case 0:
								if(contrs<31)
								{
									DisplayTask->ParamIndicNum(45, 0, ++contrs+1);

									DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);

									if(!presetControllers[contrs*4])
										DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc_of);
									else
									{
										if(presetControllers[contrs*4]>4)
										{
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc);
											DisplayTask->ParamIndicNum(69, 1, presetControllers[contrs*4]-5);
										}
										else
											DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0,
													(uint8_t*)contr_ext_l+(presetControllers[contrs*4]-1)*12);
									}
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0, (uint8_t*)midi_dest_list+(presetControllers[contrs*4+1])*14);

									control_destin = dest_tabl_start[presetControllers[contrs*4+1]];
								}
							break;
							case 1:
								if(presetControllers[contrs*4]<132)
								{
									DisplayTask->Clear_str(45, 1, TDisplayTask::fntSystem, 13);
									presetControllers[contrs*4] = enc_speed_inc(presetControllers[contrs*4], 132);
									if(presetControllers[contrs*4]==1)
									{
										if(sys_para[EXPRESSION_TYPE]<3) presetControllers[contrs*4]++;

									}
									if(presetControllers[contrs*4]==2)
									{
										if(sys_para[fs1]!=1) presetControllers[contrs*4]++;

									}
									if(presetControllers[contrs*4]==3)
									{
										if(sys_para[fs2]!=1) presetControllers[contrs*4]++;

									}
									if(presetControllers[contrs*4]==4)
									{
										if(sys_para[fs3]!=1) presetControllers[contrs*4]++;

									}
									if(presetControllers[contrs*4]>4)
									{
										DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cc);
										DisplayTask->ParamIndicNum(69, 1, presetControllers[contrs*4]-5);
									}
									else
										DisplayTask->StringOut(45, 1, TDisplayTask::fntSystem, 0, (uint8_t*)contr_ext_l+(presetControllers[contrs*4]-1)*12);
								}
							break;
							case 2:
								if(control_destin<54)
								{
									presetControllers[contrs*4+1] = dest_tabl[++control_destin];
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0, (uint8_t*)midi_dest_list+(presetControllers[contrs*4+1])*14);
								}
							break;
							case 3:
								if(presetControllers[2+contrs*4]<127)
								{
									presetControllers[2+contrs*4] = enc_speed_inc(presetControllers[2+contrs*4], 127);
									DisplayTask->ParamIndic(45, 0, presetControllers[2+contrs*4]);
								}
							break;
							case 4:
								if(presetControllers[3+contrs*4]<127)
								{
									presetControllers[3+contrs*4] = enc_speed_inc(presetControllers[3+contrs*4], 127);
									DisplayTask->ParamIndic(45, 1, presetControllers[3+contrs*4]);
								}
							break;
							case 5:
								if(presetControllers[510]<2)
									DisplayTask->StringOut(45, 2, TDisplayTask::fntSystem, 0, (uint8_t*)out_pc_list+ ++presetControllers[510]*8);
							break;
							case 6:
								uint8_t a = 0;
								if(presetControllers[511]&0x80)
									a = presetControllers[511]&0x7f;
								else
									a = currentPresetNumber;
								if(a<127)
								{
									a = enc_speed_inc(a, 127);
									DisplayTask->ParamIndicNum(45, 3, a+1);
									presetControllers[511] = a|0x80;
								}
							break;
						}
						tap_del_fl = tap_trem_fl = tap_moog_fl = 0;
						for(uint8_t i = 0; i<32; i++)
						{
							if(presetControllers[i*4])
							{
								if(presetControllers[i*4+1]==9)
									tap_del_fl = 1;
								if(presetControllers[i*4+1]==26)
									tap_trem_fl = 1;
								if(presetControllers[i*4+1]==35)
									tap_moog_fl = 1;
							}
						}
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(encoder_knob_pressed==1)
			{
				if(encoder_knob_selected==0)
				{
					encoder_knob_selected = 1;
					if(par_num<6)
						DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 2,
								(uint8_t*)contr_list+par_num*7);
					else
						DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 2, (uint8_t*)contr_list+par_num*7);
				}
				else
				{
					if(par_num<6)
						DisplayTask->StringOut(0, par_num%3, TDisplayTask::fntSystem, 0,
								(uint8_t*)contr_list+par_num*7);
					else
						DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)contr_list+par_num*7);
					encoder_knob_selected = 0;
				}
				tim5_start(1);
				clean_flag();
			}
			if((k_att==1)||(k_sys==1)||(k_tuner==1)||(k_master==1))
			{
				if(k_tuner==1)
					num_men = 14;
				current_menu_type = MENU_MAIN;
				encoder_knob_selected = 0;
				tim5_start(0);
				CSTask->Give();
				break;
			}
			if((k_up==1)||(k_master_eq==1))
			{
				sub_men = 0;
				GUI_return_to_main_menu();
			}
		break;
			//------------------------------------------System menu---------------------------------------------------------
		case MENU_SYSTEM: break;

			if(k_down==1)
			{
				if((par_num==3)&&(sys_para[EXPRESSION_TYPE]&0x80))
				{
					current_menu_type = MENU_EXPRESSION;
					DisplayTask->Clear();
					for(uint8_t i = 0; i<4; i++)
					{
						DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu+i*10);
					}
					DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0,
							(uint8_t*)expr_type+(sys_para[EXPRESSION_TYPE]&0x7f)*12);
					if(!sys_para[exp_ccN])
						DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
					else
						DisplayTask->ParamIndicNum(84, 2, sys_para[exp_ccN]-1);
					if(!sys_para[sav_exp])
						DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
					else
						DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"On ");
					eq_num = 0;
					encoder_knob_selected = 0;
				}
			}
			if(encoder_knob_pressed==1)
			{
				switch(par_num)
				{
					case 4:
						current_menu_type = MENU_FSW_TYPE;
						DisplayTask->Clear();
						for(uint8_t i = 0; i<4; i++)
							DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)footsw_menu+i*12);
						eq_num = 0;
						encoder_knob_pressed = 0;
						DisplayTask->ParamIndic(58, 3, sys_para[foot_sp]);
					break;
					case 6:
						current_menu_type = MENU_MIDI_PC;
						DisplayTask->Clear();
						eq_num = 0;
						encoder_knob_pressed = 0;
						DisplayTask->ParamIndicNum(3, 0, eq_num+1);
						DisplayTask->StringOut(24, 0, TDisplayTask::fntSystem, 0, (uint8_t*)"->");
						DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
					break;
					default:
					break;
				}
				tim5_start(0);
				clean_flag();
			}

		break;
//-----------------------------------------------------Expression-------------------------------------------
		case MENU_EXPRESSION:
			if(encoder_knob_selected==0)
			{
				if(tim5_fl==1)
					DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu+eq_num*10);
				else
					DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu+eq_num*10);
			}
			if(encoder_state_updated)
			{
				if(encoder_state==1)
				{
					if(!encoder_knob_selected)
					{
						if(eq_num>0)
						{
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0,
									(uint8_t*)expr_menu+eq_num--*10);
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)expr_menu+eq_num*10);
							tim5_start(0);
						}
					}
					else
					{
						switch(eq_num)
						{
							case 0:
								if((sys_para[EXPRESSION_TYPE]&0x7f)>1)
								{
									DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0,
											(uint8_t*)expr_type+(--sys_para[EXPRESSION_TYPE]&0x7f)*12);
									adc_pin_init();
									if(sys_para[EXPRESSION_TYPE]>2)
										ext_send(127);
								}
							break;
							case 2:
								if(sys_para[exp_ccN]>0)
								{
									sys_para[exp_ccN] = enc_speed_dec(sys_para[exp_ccN], 0);
									if(!sys_para[exp_ccN])
										DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
									else
										DisplayTask->ParamIndicNum(84, 2, sys_para[exp_ccN]-1);
								}
							break;
							case 3:
								if(sys_para[sav_exp])
								{
									sys_para[sav_exp]--;
									DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								}
							break;
						}
					}
				}
				if(encoder_state==2)
				{
					if(!encoder_knob_selected)
					{
						if(eq_num<3)
						{
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0,
									(uint8_t*)expr_menu+eq_num++*10);
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)expr_menu+eq_num*10);
							tim5_start(0);
						}
					}
					else
					{
						switch(eq_num)
						{
							case 0:
								if((sys_para[EXPRESSION_TYPE]&0x7f)<4)
								{
									DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0,
											(uint8_t*)expr_type+(++sys_para[EXPRESSION_TYPE]&0x7f)*12);
									adc_pin_init();
									if(sys_para[EXPRESSION_TYPE]>2)
										ext_send(127);
								}
							break;
							case 2:
								if(sys_para[exp_ccN]<128)
								{
									sys_para[exp_ccN] = enc_speed_inc(sys_para[exp_ccN], 100);
									if(!sys_para[exp_ccN])
										DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
									else
										DisplayTask->ParamIndicNum(84, 2, sys_para[exp_ccN]-1);
								}
							break;
							case 3:
								if(!sys_para[sav_exp])
								{
									sys_para[sav_exp]++;
									DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"On ");
								}
							break;
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
					switch(eq_num)
					{
						case 0:
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)expr_menu+eq_num*10);
						break;
						case 1:
							DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 0, (uint8_t*)set_min);
							current_menu_type = MENU_ADC_CALIBRATE;
							tim5_start(0);
						break;
						case 2:
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)expr_menu+eq_num*10);
						break;
					}
				}
				else
				{
					encoder_knob_selected = 0;
					DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu+eq_num*10);
				}
				clean_flag();
			}
			if(k_up==1)
			{
//				current_menu = MENU_SYSTEM;
//				encoder_knob_selected = 0;
//				DisplayTask->Clear();
//				DisplayTask->Icon_Strel(ICON_SY, STRELKA_DOWN);
//				for(uint8_t i = 0; i<4; i++)
//				{
//					DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)sys_menu_list+i*12);
//					switch(i)
//					{
//						case 0:
////							DisplayTask->StringOut(36, i, TDisplayTask::fntSystem, 0,
////									(uint8_t*)mode_list+sys_para[i]*12);
////							gui_send(14, 0);
//						break;
//						case 1:
//							DisplayTask->ParamIndicNum(60, i, sys_para[i]+1);
//						break;
//						case 2:
////							DisplayTask->StringOut(60, i, TDisplayTask::fntSystem, 0,
////									(uint8_t*)cab_out_list+sys_para[i]*6);
//						break;
//						case 3:
////							if(!sys_para[EXPRESSION_TYPE])
////								DisplayTask->StringOut(78, i, TDisplayTask::fntSystem, 0, (uint8_t*)expr_on_off+5);
////							else
////								DisplayTask->StringOut(78, i, TDisplayTask::fntSystem, 0, (uint8_t*)expr_on_off);
//						break;
//					}
//				}
				clean_flag();
			}
		break;
//-----------------------------------------------------Calibrate--------------------------------------------
		case MENU_ADC_CALIBRATE:
			if(tim5_fl==1)
			{
				if(calib_fl)
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 2, (uint8_t*)set_max);
				else
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 2, (uint8_t*)set_min);
			}
			else
			{
				if(calib_fl)
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 0, (uint8_t*)set_max);
				else
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 0, (uint8_t*)set_min);
			}
			if(encoder_knob_pressed==1)
			{
				if(calib_fl==0)
				{
					sys_para[7] = adc_bu;
					sys_para[8] = adc_bu>>8;
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 3, (uint8_t*)set_min);
					DisplayTask->StringOut(92, 1, TDisplayTask::fntSystem, 0, (uint8_t*)ok);
					dela(0x7fffff);
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 2, (uint8_t*)set_max);
					calib_fl = 1;
					tim5_start(0);
				}
				else
				{
					sys_para[9] = adc_bu;
					sys_para[10] = adc_bu>>8;
					DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 3, (uint8_t*)set_max);
					DisplayTask->StringOut(92, 1, TDisplayTask::fntSystem, 0, (uint8_t*)ok);
					dela(0x7fffff);
					calib_fl = 0;
					current_menu_type = MENU_EXPRESSION;
					encoder_knob_selected = 0;
				}
				tim5_start(0);
				clean_flag();
				adc_calib();
			}
		break;

//----------------------------------------------------------Foot Switch type---------------------------------
		case MENU_FSW_TYPE:
			if(!encoder_knob_selected)
			{
				if(tim5_fl)
					DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2, (uint8_t*)footsw_menu+eq_num*12);
				else
					DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0, (uint8_t*)footsw_menu+eq_num*12);
			}
			if(encoder_state_updated)
			{
				if(encoder_state==1)
				{
					if(!encoder_knob_selected)
					{
						if(eq_num>0)
						{
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0,
									(uint8_t*)footsw_menu+eq_num--*12);
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)footsw_menu+eq_num*12);
							tim5_start(0);
						}
					}
					else
					{
						if(sys_para[foot_sp])
						{
							sys_para[foot_sp] = enc_speed_dec(sys_para[foot_sp], 0);
							DisplayTask->ParamIndic(58, 3, sys_para[foot_sp]);
						}
					}
				}
				if(encoder_state==2)
				{
					if(!encoder_knob_selected)
					{
						if(eq_num<3)
						{
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0,
									(uint8_t*)footsw_menu+eq_num++*12);
							DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2,
									(uint8_t*)footsw_menu+eq_num*12);
							tim5_start(0);
						}
					}
					else
					{
						if(sys_para[foot_sp]<127)
						{
							sys_para[foot_sp] = enc_speed_inc(sys_para[foot_sp], 127);
							DisplayTask->ParamIndic(58, 3, sys_para[foot_sp]);
						}
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(encoder_knob_pressed==1)
			{
				if(eq_num<3)
				{
					DisplayTask->Clear();
					current_menu_type = MENU_SW_MODE;
					DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)mode_);
					DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
							(uint8_t*)switch_mod+sys_para[fsm1+eq_num]*7);
					if(sys_para[fsm1+eq_num])
					{
						DisplayTask->StringOut(10, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow);
						DisplayTask->StringOut(80, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow+10);
					}
					par_num1 = 0;
				}
				else
				{
					if(!encoder_knob_selected)
					{
						encoder_knob_selected = 1;
						DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 2, (uint8_t*)footsw_menu+eq_num*12);
					}
					else
					{
						encoder_knob_selected = 0;
						DisplayTask->StringOut(3, eq_num, TDisplayTask::fntSystem, 0, (uint8_t*)footsw_menu+eq_num*12);
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(k_up==1)
			{
				DisplayTask->Clear();
				for(uint8_t i = 0; i<4; i++)
				{
					DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)sys_menu_list+48+i*12);
					if(i==1)
//						DisplayTask->StringOut(44, i, TDisplayTask::fntSystem, 0,
//								(uint8_t*)spdif_type+sys_para[SPDIF_OT_TYPE]*12);
					if(i==3)
						DisplayTask->StringOut(44, i, TDisplayTask::fntSystem, 0,
								(uint8_t*)tempo_type+sys_para[TAP_TYPE]*7);
				}
				current_menu_type = MENU_SYSTEM;
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
					if(!par_num1)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)mode_);
					else
					{
						if(sys_para[fsm1+eq_num])
							DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 2,
									(uint8_t*)switch_slow+((par_num1-1)*10));
						else
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 2, (uint8_t*)switch_mod);
					}
				}
				else
				{
					if(!par_num1)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)mode_);
					else
					{
						if(sys_para[fsm1+eq_num])
							DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)switch_slow+((par_num1-1)*10));
						else
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0, (uint8_t*)switch_mod);
					}
				}
			}
			if(encoder_state_updated)
			{
				if(encoder_state==1)
				{
					if(!encoder_knob_selected)
					{
						if(sys_para[fsm1+eq_num])
						{
							if(par_num1)
							{
								DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)switch_slow+((par_num1---1)*10));
								if(par_num1)
									DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 2,
											(uint8_t*)switch_slow+(par_num1-1)*10);
								else
									DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)mode_);
								tim5_start(0);
							}
						}
						else
						{
							if(par_num1)
							{
								DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
										(uint8_t*)switch_mod+--par_num1);
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)mode_);
								tim5_start(0);
							}
						}
					}
					else
					{
						if(sys_para[fsm1+eq_num])
						{
							DisplayTask->Clear_str(10, 2, TDisplayTask::fntSystem, 35);
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)switch_mod+--sys_para[fsm1+eq_num]);
						}
					}
				}
				if(encoder_state==2)
				{
					if(!encoder_knob_selected)
					{
						if(sys_para[fsm1+eq_num])
						{
							if(par_num1<2)
							{
								if(!par_num1)
								{
									DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0,
											(uint8_t*)mode_+par_num1++);
								}
								else
								{
									DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 0,
											(uint8_t*)switch_slow+((par_num1++-1)*10));
								}
								DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 2,
										(uint8_t*)switch_slow+(par_num1-1)*10);
								tim5_start(0);
							}
						}
						else
						{
							if(!par_num1)
							{
								if(!par_num1)
									DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0,
											(uint8_t*)mode_+par_num1++);
								else
									DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 2, (uint8_t*)switch_mod);
								tim5_start(0);
							}
						}
					}
					else
					{
						if(!sys_para[fsm1+eq_num])
						{
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)switch_mod+ ++sys_para[fsm1+eq_num]*7);
							DisplayTask->StringOut(10, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow);
							DisplayTask->StringOut(80, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow+10);
						}
					}
				}
				clean_flag();
			}
			if(encoder_knob_pressed==1)
			{
				if(encoder_knob_selected)
				{
					encoder_knob_selected = 0;
					if(!par_num1)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)mode_);
					else
						DisplayTask->StringOut(10+(par_num1-1)*70, 2, TDisplayTask::fntSystem, 0,
								(uint8_t*)switch_slow+((par_num1-1)*10));
				}
				else
				{
					if(!par_num1)
					{
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)mode_);
						encoder_knob_selected = 1;
					}
					else
					{
						DisplayTask->Clear();
						par_num2 = 0;
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
						if(par_num1==1)
						{
							current_menu_type = MENU_SW_SELECT0;
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs1+eq_num]*12);
							if(sys_para[fs1+eq_num]>2)
							{
								uint8_t a = sys_para[fs1+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr11+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr11+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr11+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr11+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							else
							{
								if(sys_para[fs1+eq_num]==1)
								{
									DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
									if(!sys_para[k1_cc+eq_num])
										DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
									else
										DisplayTask->ParamIndicNum(76, 2, sys_para[k1_cc+eq_num]-1);
								}
							}
						}
						else
						{
							current_menu_type = MENU_SW_SELECT1;
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs11+eq_num]*12);
							if(sys_para[fs11+eq_num]>2)
							{
								uint8_t a = sys_para[fs11+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr111+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr111+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr111+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr111+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							else
							{
								if(sys_para[fs11+eq_num]==1)
								{
									DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
									if(!sys_para[k11_cc+eq_num])
										DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
									else
										DisplayTask->ParamIndicNum(76, 2, sys_para[k11_cc+eq_num]-1);
								}
							}
						}
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(k_up==1)
			{
				DisplayTask->Clear();
				for(uint8_t i = 0; i<4; i++)
					DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)footsw_menu+i*12);
				DisplayTask->ParamIndic(58, 3, sys_para[foot_sp]);
				current_menu_type = MENU_FSW_TYPE;
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
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
						if(sys_para[fs1+eq_num]>2)
						{
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				else
				{
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
						if(sys_para[fs1+eq_num]>2)
						{
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2-1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
			}
			if(encoder_state_updated)
			{
				if(encoder_state==1)
				{
					if(!encoder_knob_selected)
					{
						if(par_num2>0)
						{
							if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2---1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
							if(!par_num2)
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
							tim5_start(0);
						}
					}
					else
					{
						if(!par_num2)
						{
							if(sys_para[fs1+eq_num]>0)
								sys_para[fs1+eq_num]--;
							if(sys_para[fs1+eq_num]==1)
							{
								//if(sys_para[fs11 + eq_num] == 1)sys_para[fs1 + eq_num]--;
							}
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs1+eq_num]*12);
							DisplayTask->Clear_str(0, 2, TDisplayTask::fntSystem, 21);
							kgp_sdk_libc::memset(str_temp, 0, 22);
							if(sys_para[fs1+eq_num]>2)
							{
								uint8_t a = sys_para[fs1+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr11+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr11+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr11+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr11+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							if(sys_para[fs1+eq_num]==1)
							{
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
								if(!sys_para[k1_cc+eq_num])
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, sys_para[k1_cc+eq_num]-1);
							}
						}
						else
						{
							if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							{
								uint8_t a = sys_para[k1_cc+eq_num];
								if(a>0)
									a = enc_speed_dec(a, 0);
								sys_para[k1_cc+eq_num] = a;
								if(!a)
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, a-1);
							}
							if(sys_para[fs1+eq_num]>2)
							{
								int8_t a = sys_para[pr11+eq_num*4+par_num2-1];
								a = enc_speed_dec(a, -1);
								if(a<0)
									a = 98;
								sys_para[pr11+eq_num*4+par_num2-1] = a++;
								str_temp[(par_num2-1)*4] = ('0'+a/10)|0x80;
								str_temp[(par_num2-1)*4+1] = ('0'+a%10)|0x80;
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1+eq_num]-3]+(par_num2-1)*24, 2,
										TDisplayTask::fntSystem, 2, '0'+a/10);
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1+eq_num]-3]+(par_num2-1)*24+6, 2,
										TDisplayTask::fntSystem, 2, '0'+a%10);
							}
						}
					}
				}
				if(encoder_state==2)
				{
					if(!encoder_knob_selected)
					{
						if(sys_para[fs1+eq_num]==1)
						{
							if(!par_num2)
							{
								par_num2++;
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
								tim5_start(0);
							}
						}
						if(sys_para[fs1+eq_num]>2)
						{
							if(!par_num2)
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
							if(par_num2<(sys_para[fs1+eq_num]-2))
							{
								str_temp[(par_num2-1)*4] &= ~0x80;
								str_temp[(par_num2++-1)*4+1] &= ~0x80;
								DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem,
										0, (uint8_t*)str_temp);
								str_temp[(par_num2-1)*4] |= 0x80;
								str_temp[(par_num2-1)*4+1] |= 0x80;
								DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem,
										0, (uint8_t*)str_temp);
							}
							tim5_start(0);
						}
					}
					else
					{
						if(!par_num2)
						{
							if(sys_para[fs1+eq_num]<6)
								sys_para[fs1+eq_num]++;
							if(sys_para[fs1+eq_num]==1)
							{
								//if(sys_para[fs11 + eq_num] == 1)sys_para[fs1 + eq_num]++;
							}
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs1+eq_num]*12);
							DisplayTask->Clear_str(0, 2, TDisplayTask::fntSystem, 21);
							kgp_sdk_libc::memset(str_temp, 0, 22);
							if(sys_para[fs1+eq_num]>2)
							{
								uint8_t a = sys_para[fs1+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr11+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr11+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr11+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr11+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							if(sys_para[fs1+eq_num]==1)
							{
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
								if(!sys_para[k1_cc+eq_num])
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, sys_para[k1_cc+eq_num]-1);
							}
						}
						else
						{
							if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							{
								uint8_t a = sys_para[k1_cc+eq_num];
								if(a<128)
									a = enc_speed_inc(a, 128);
								sys_para[k1_cc+eq_num] = a;
								if(!a)
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, a-1);
							}
							if(sys_para[fs1+eq_num]>2)
							{
								uint8_t a = sys_para[pr11+eq_num*4+par_num2-1];
								a = enc_speed_inc(a, 98);
								if(a>98)
									a = 0;
								sys_para[pr11+eq_num*4+par_num2-1] = a++;
								str_temp[(par_num2-1)*4] = ('0'+a/10)|0x80;
								str_temp[(par_num2-1)*4+1] = ('0'+a%10)|0x80;
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1+eq_num]-3]+(par_num2-1)*24, 2,
										TDisplayTask::fntSystem, 2, '0'+a/10);
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs1+eq_num]-3]+(par_num2-1)*24+6, 2,
										TDisplayTask::fntSystem, 2, '0'+a%10);
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
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
						else
						{
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2-1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				else
				{
					encoder_knob_selected = 1;
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs1+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
						else
						{
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs1+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(k_up==1)
			{
				DisplayTask->Clear();
				current_menu_type = MENU_SW_MODE;
				DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)mode_);
				DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0, (uint8_t*)switch_mod+sys_para[fsm1+eq_num]*7);
				if(sys_para[fsm1+eq_num])
				{
					DisplayTask->StringOut(10, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow);
					DisplayTask->StringOut(80, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow+10);
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
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
						if(sys_para[fs11+eq_num]>2)
						{
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				else
				{
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
						if(sys_para[fs11+eq_num]>2)
						{
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2-1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
			}
			if(encoder_state_updated)
			{
				if(encoder_state==1)
				{
					if(!encoder_knob_selected)
					{
						if(par_num2>0)
						{
							if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2---1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
							if(!par_num2)
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
							tim5_start(0);
						}
					}
					else
					{
						if(!par_num2)
						{
							if(sys_para[fs11+eq_num]>0)
								sys_para[fs11+eq_num]--;
							if(sys_para[fs11+eq_num]==1)
							{
								//if(sys_para[fs1 + eq_num] == 1)sys_para[fs11 + eq_num]--;
							}
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs11+eq_num]*12);
							DisplayTask->Clear_str(0, 2, TDisplayTask::fntSystem, 21);
							kgp_sdk_libc::memset(str_temp, 0, 22);
							if(sys_para[fs11+eq_num]>2)
							{
								uint8_t a = sys_para[fs11+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr111+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr111+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr111+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr111+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							if(sys_para[fs11+eq_num]==1)
							{
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
								if(!sys_para[k11_cc+eq_num])
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, sys_para[k11_cc+eq_num]-1);
							}
						}
						else
						{
							if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							{
								uint8_t a = sys_para[k11_cc+eq_num];
								if(a>0)
									a = enc_speed_dec(a, 0);
								sys_para[k11_cc+eq_num] = a;
								if(!a)
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, a-1);
							}
							if(sys_para[fs11+eq_num]>2)
							{
								int8_t a = sys_para[pr111+eq_num*4+par_num2-1];
								a = enc_speed_dec(a, -1);
								if(a<0)
									a = 98;
								sys_para[pr111+eq_num*4+par_num2-1] = a++;
								str_temp[(par_num2-1)*4] = ('0'+a/10)|0x80;
								str_temp[(par_num2-1)*4+1] = ('0'+a%10)|0x80;
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11+eq_num]-3]+(par_num2-1)*24, 2,
										TDisplayTask::fntSystem, 2, '0'+a/10);
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11+eq_num]-3]+(par_num2-1)*24+6, 2,
										TDisplayTask::fntSystem, 2, '0'+a%10);
							}
						}
					}
				}
				if(encoder_state==2)
				{
					if(!encoder_knob_selected)
					{
						if(sys_para[fs11+eq_num]==1)
						{
							if(!par_num2)
							{
								par_num2++;
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
								tim5_start(0);
							}
						}
						if(sys_para[fs11+eq_num]>2)
						{
							if(!par_num2)
								DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
							if(par_num2<(sys_para[fs11+eq_num]-2))
							{
								str_temp[(par_num2-1)*4] &= ~0x80;
								str_temp[(par_num2++-1)*4+1] &= ~0x80;
								DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2,
										TDisplayTask::fntSystem, 0, (uint8_t*)str_temp);
								str_temp[(par_num2-1)*4] |= 0x80;
								str_temp[(par_num2-1)*4+1] |= 0x80;
								DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2,
										TDisplayTask::fntSystem, 0, (uint8_t*)str_temp);
							}
							tim5_start(0);
						}
					}
					else
					{
						if(!par_num2)
						{
							if(sys_para[fs11+eq_num]<6)
								sys_para[fs11+eq_num]++;
							if(sys_para[fs11+eq_num]==1)
							{
								//if(sys_para[fs1 + eq_num] == 1)sys_para[fs11 + eq_num]++;
							}
							DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0,
									(uint8_t*)fsw_t+sys_para[fs11+eq_num]*12);
							DisplayTask->Clear_str(0, 2, TDisplayTask::fntSystem, 21);
							kgp_sdk_libc::memset(str_temp, 0, 22);
							if(sys_para[fs11+eq_num]>2)
							{
								uint8_t a = sys_para[fs11+eq_num]-3;
								str_pres_init(str_temp, sys_para[pr111+eq_num*4]+1);
								if(a)
									kgp_sdk_libc::memcpy(str_temp+2, (uint8_t*)"->", 2);
								if(a)
									str_pres_init(str_temp+4, sys_para[pr111+eq_num*4+1]+1);
								if(a>1)
									kgp_sdk_libc::memcpy(str_temp+6, (uint8_t*)"->", 2);
								if(a>1)
									str_pres_init(str_temp+8, sys_para[pr111+eq_num*4+2]+1);
								if(a>2)
									kgp_sdk_libc::memcpy(str_temp+10, (uint8_t*)"->", 2);
								if(a>2)
									str_pres_init(str_temp+12, sys_para[pr111+eq_num*4+3]+1);
								DisplayTask->StringOut(pres_num_pos[a], 2, TDisplayTask::fntSystem, 0,
										(uint8_t*)str_temp);
							}
							if(sys_para[fs11+eq_num]==1)
							{
								DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
								if(!sys_para[k11_cc+eq_num])
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, sys_para[k11_cc+eq_num]-1);
							}
						}
						else
						{
							if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							{
								uint8_t a = sys_para[k11_cc+eq_num];
								if(a<128)
									a = enc_speed_inc(a, 128);
								sys_para[k11_cc+eq_num] = a;
								if(!a)
									DisplayTask->StringOut(76, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
								else
									DisplayTask->ParamIndicNum(76, 2, a-1);
							}
							if(sys_para[fs11+eq_num]>2)
							{
								uint8_t a = sys_para[pr111+eq_num*4+par_num2-1];
								a = enc_speed_inc(a, 98);
								if(a>98)
									a = 0;
								sys_para[pr111+eq_num*4+par_num2-1] = a++;
								str_temp[(par_num2-1)*4] = ('0'+a/10)|0x80;
								str_temp[(par_num2-1)*4+1] = ('0'+a%10)|0x80;
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11+eq_num]-3]+(par_num2-1)*24, 2,
										TDisplayTask::fntSystem, 2, '0'+a/10);
								DisplayTask->SymbolOut(pres_num_pos[sys_para[fs11+eq_num]-3]+(par_num2-1)*24+6, 2,
										TDisplayTask::fntSystem, 2, '0'+a%10);
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
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"CC#");
						else
						{
							str_temp[(par_num2-1)*4] &= ~0x80;
							str_temp[(par_num2-1)*4+1] &= ~0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				else
				{
					encoder_knob_selected = 1;
					if(!par_num2)
						DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 2, (uint8_t*)expr_menu);
					else
					{
						if((sys_para[fs11+eq_num]==1)&&(par_num2==1))
							DisplayTask->StringOut(52, 2, TDisplayTask::fntSystem, 2, (uint8_t*)"CC#");
						else
						{
							str_temp[(par_num2-1)*4] |= 0x80;
							str_temp[(par_num2-1)*4+1] |= 0x80;
							DisplayTask->StringOut(pres_num_pos[sys_para[fs11+eq_num]-3], 2, TDisplayTask::fntSystem, 0,
									(uint8_t*)str_temp);
						}
					}
				}
				tim5_start(0);
				clean_flag();
			}
			if(k_up==1)
			{
				DisplayTask->Clear();
				current_menu_type = MENU_SW_MODE;
				DisplayTask->StringOut(3, 0, TDisplayTask::fntSystem, 0, (uint8_t*)mode_);
				DisplayTask->StringOut(40, 0, TDisplayTask::fntSystem, 0, (uint8_t*)switch_mod+sys_para[fsm1+eq_num]*7);
				if(sys_para[fsm1+eq_num])
				{
					DisplayTask->StringOut(10, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow);
					DisplayTask->StringOut(80, 2, TDisplayTask::fntSystem, 0, (uint8_t*)switch_slow+10);
				}
				tim5_start(0);
				clean_flag();
				encoder_knob_selected = 0;
			}
		break;

//------------------------------------------------------MIDI Map----------------------------------------------
		case MENU_MIDI_PC:
			if(!encoder_knob_selected)
			{
				if(tim5_fl)
					DisplayTask->Clear_str(3, 0, TDisplayTask::fntSystem, 3);
				else
					DisplayTask->ParamIndicNum(3, 0, eq_num+1);
			}
			else
			{
				if(tim5_fl)
					DisplayTask->Clear_str(36, 0, TDisplayTask::fntSystem, 3);
				else
					DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
			}
			if(encoder_state_updated==1)
			{
				if(encoder_state==1)
				{
					if(encoder_knob_selected==0)
					{
						if(eq_num>0)
						{
							eq_num = enc_speed_dec(eq_num, 0);
							DisplayTask->ParamIndicNum(3, 0, eq_num+1);
							DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
						}
					}
					else
					{
						if(sys_para[eq_num+128]>0)
						{
							sys_para[eq_num+128] = enc_speed_dec(sys_para[eq_num+128], 0);
							DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
						}
					}
				}
				if(encoder_state==2)
				{
					if(encoder_knob_selected==0)
					{
						if(eq_num<127)
						{
							eq_num = enc_speed_inc(eq_num, 127);
							DisplayTask->ParamIndicNum(3, 0, eq_num+1);
							DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
						}
					}
					else
					{
						if(sys_para[eq_num+128]<98)
						{
							sys_para[eq_num+128] = enc_speed_inc(sys_para[eq_num+128], 98);
							DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
						}
					}
				}
				tim5_start(1);
				clean_flag();
			}
			if(encoder_knob_pressed==1)
			{
				if(encoder_knob_selected==0)
				{
					encoder_knob_selected = 1;
					DisplayTask->ParamIndicNum(3, 0, eq_num+1);
				}
				else
				{
					DisplayTask->ParamIndicNum(36, 0, sys_para[eq_num+128]+1);
					encoder_knob_selected = 0;
				}
				tim5_start(0);
				clean_flag();
			}
			if(k_up==1)
			{
				DisplayTask->Clear();
				for(uint8_t i = 0; i<4; i++)
				{
					DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)sys_menu_list+48+i*12);
					if(i==1)
//						DisplayTask->StringOut(44, i, TDisplayTask::fntSystem, 0,
//								(uint8_t*)spdif_type+sys_para[SPDIF_OT_TYPE]*12);
					if(i==3)
						DisplayTask->StringOut(44, i, TDisplayTask::fntSystem, 0,
								(uint8_t*)tempo_type+sys_para[TAP_TYPE]*7);
				}
				current_menu_type = MENU_SYSTEM;
				DisplayTask->Icon_Strel(ICON_SY, STRELKA_UP);
				encoder_knob_selected = 0;
				tim5_start(0);
				clean_flag();
			}
		break;
	}
	if(sys_para[TAP_TYPE]==2)
	{
		extern volatile uint16_t midi_clk_buf[];
		uint16_t mediann_tap(uint16_t *array, int length);
		uint16_t c = mediann_tap((uint16_t*)midi_clk_buf, 64);
		c >>= 4;
		if((c<(midi_clk_send-7))||(c>(midi_clk_send+7)))
		{
			midi_clk_send = c;
			void send_midi_temp(uint16_t *c);
			send_midi_temp(&c);
		}
	}
}

uint8_t tap_temp_global(void)
{
	uint8_t a = 0;
	if(tap_temp<=131070)
	{
		tap_global = tap_temp>>4;
		if(sys_para[TAP_TYPE]) // global temp On
		{
			sys_para[TAP_HIGH] = (tap_global/3)>>8;
			gui_send(33, tap_global/3);
			moog_time = tap_global/3.0f;
			gui_send(31, 13);
			delay_time = tap_global/3.0f/tap_time_coefs[presetData[d_tap_t]];
			if(delay_time<2731)
				gui_send(3, 1);
			trem_time = tap_global/3.0f/tap_time_coefs[presetData[t_tap_t]];
			if(trem_time<2731)
				gui_send(10, 5);
		}
		a = 1;
	}
	tap_temp = 0;
	return a;
}
//---------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler()
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	if(tim5_fl)
		tim5_fl = 0;
	else
		tim5_fl = 1;
	if(!tuner_use)
		CSTask->Give();
}
