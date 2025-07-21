#ifndef GU_H_
#define GU_H_

#include "appdefs.h"
#include "modules.h"
#include "abstractmenu.h"

#include "baseparam.h"

#define IN_OUT_STR_SIZE 7
const uint8_t in_out_strings[][IN_OUT_STR_SIZE] = {"Input ", "Output"};

typedef union {
	float s;
	uint32_t ss;
}tr_t;

void tim5_start(uint8_t val);

extern const uint8_t expr_menu [][10];

extern const float tap_time_coefs[];
extern const float tap_moog[];
extern bool cab_data_ready;
extern uint8_t tun_del_val;
extern float tun_base_old;
extern uint32_t tap_val;

void gui(void);
void clean_flag (void);
void adc_proc(void);
void comp_para(uint32_t val);
void gate_par(uint32_t val);

void prog_ch(void);

#endif /* GU_H_ */
