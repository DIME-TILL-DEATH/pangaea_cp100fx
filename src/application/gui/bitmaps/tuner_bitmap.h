#ifndef TUN_BIT_H_
#define TUN_BIT_H_

#include "appdefs.h"

extern volatile uint8_t t_no;
extern uint8_t t_po;
extern uint8_t t_po1;
extern volatile uint8_t notee;
extern float tun_base_old;

void tuner_init(void);
void tuner_scale(void);
void tuner_arrow(void);
void tuner_note(void);

#endif
