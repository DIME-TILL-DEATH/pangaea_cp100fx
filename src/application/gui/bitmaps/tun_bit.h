#ifndef TUN_BIT_H_
#define TUN_BIT_H_

#include "appdefs.h"

extern volatile uint8_t t_no;
extern uint8_t t_po;
extern uint8_t t_po1;
extern volatile uint8_t notee;
extern float tun_base_old;

void tun_ini(void);
void scal_tun(void);
void strel_tun(void);
void note_tun(void);

#endif
