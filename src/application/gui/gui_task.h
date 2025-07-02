#ifndef GU_H_
#define GU_H_

#include "appdefs.h"
#include "../modules.h"
#include "menus/abstractmenu.h"

#include "menus/paramlistmenu/baseparam.h"

#define IN_OUT_STR_SIZE 7
const uint8_t in_out_strings[][IN_OUT_STR_SIZE] = {"Input ", "Output"};

typedef union {
	float s;
	uint32_t ss;
}tr_t;

void tim5_start(uint8_t val);
//void clean_flag (void);


extern const uint8_t expr_menu [][10];

extern uint16_t trem_time;
extern uint16_t moog_time;

extern uint32_t tap_temp;
extern uint32_t tap_global;

extern gui_menu_type current_menu_type;

extern uint8_t k_up;
extern uint8_t k_down;
extern uint8_t k_sys;
extern uint8_t k_master;

extern uint8_t tuner_use;
extern const float tap_time_coefs[];
extern uint8_t tempo_fl;
extern const float tap_moog[];
extern uint8_t impulse_path[];
extern bool cab_data_ready;
extern uint8_t tun_del_val;
extern float tun_base_old;
extern uint32_t tap_val;

uint8_t tap_temp_global(void);
void gui(void);
void clean_flag (void);
void adc_proc(void);
void comp_para(uint32_t val);
void gate_par(uint32_t val);

void prog_ch(void);

#endif /* GU_H_ */
