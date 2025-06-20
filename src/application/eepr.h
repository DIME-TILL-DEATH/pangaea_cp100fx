#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

enum
{
	CAB_SIM_DISABLED,
	MIDI_CHANNEL,
	CAB_SIM_CONFIG,
	EXPRESSION_TYPE, // exp_typ
	/*type foot sw*/
	fs1, fs2, fs3,
	/*calibration*/
	calib1, calib2, calib3, calib4,
	/*preset map to foot sw*/
	pr11, pr12, pr13, pr14,
	pr21, pr22, pr23, pr24,
	pr31, pr32, pr33, pr34,
	SPDIF_OUT_TYPE,
	/*num CC to express and foot*/
	exp_ccN, k1_cc, k2_cc, k3_cc,
	/*service*/midi_pc_ind,
	/*31 - last num preset*/
	/*mode foot*/
	fsm1 = 32, fsm2, fsm3,
	/*type foot sw1*/
	fs11,
	fs21,
	fs31,
	/*preset map to foot sw1*/
	pr111, pr121, pr131, pr141,
	pr211, pr221, pr231, pr241,
	pr311, pr321, pr331, pr341,
	k11_cc, k21_cc, k31_cc,
	/*tap type*/TAP_TYPE,
	TUNER_EXTERNAL,/*tuner ext controller*/
	/*speed foot*/foot_sp,
	TIME_FORMAT,
	/*global temp hi*/TAP_HIGH,
	/*save expression level*/sav_exp,
	/*swap up confirm*/SWAP_SWITCH,
	/*speed tuner*/TUNER_SPEED,
};

extern uint16_t delay_time;
extern volatile uint8_t prog;
extern volatile uint8_t prog1;
extern volatile uint8_t prog_old;
extern volatile uint32_t flash_adr;
extern uint8_t presetData[];
extern const uint8_t prog_data_init[];
extern uint8_t preset_temp[];
extern uint8_t presetName[];
extern uint8_t presetComment[];
extern const uint8_t imya_init[];
extern uint8_t imya_t[];
extern uint8_t imya1_t[];
extern uint8_t presetControllers[];
extern uint8_t sys_para[];
extern uint8_t imp_buf_uint[];
extern char name_buf[];
extern char name_buf1[];
extern char name_buf_temp[];
extern const uint8_t no_loaded[];
extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;
extern uint8_t cab_num;
extern uint8_t __CCM_BSS__ preset_temp[];

void eepr_write(uint8_t nu);
void eepr_read_prog(uint8_t nu);
void eepr_read_imya(uint8_t nu);
void write_prog_temp(uint8_t nu);
void read_prog_temp(uint8_t nu);
void preset_erase(uint8_t nu);
void eepr_read_prog_data(uint8_t nu);

#endif
