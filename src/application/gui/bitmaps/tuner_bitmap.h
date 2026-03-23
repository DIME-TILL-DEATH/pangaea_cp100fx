#ifndef TUN_BIT_H_
#define TUN_BIT_H_

#include "appdefs.h"

extern volatile uint8_t t_no;

void tuner_init(void);
void tuner_arrow(uint8_t arrow_pos);
void tuner_note(void);
void tuner_ref_freq(uint16_t refFreq);
#endif
