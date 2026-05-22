#ifndef _EQ_SETTERS_H_
#define _EQ_SETTERS_H_

#include "translator.h"

enum eq_parameters_t
{
	EQ_G0 = 48, EQ_G1, EQ_G2, EQ_G3, EQ_G4,
	EQ_HPF,
	EQ_LPF,
	EQ_PRESENCE,
	EQ_F0 = 101, EQ_F1, EQ_F2, EQ_F3, EQ_F4,
	EQ_Q0, EQ_Q1, EQ_Q2, EQ_Q3, EQ_Q4,
	EQ_PREPOST = 146
};

enum
{
	EQ_G0_POS = 0, EQ_G1_POS, EQ_G2_POS, EQ_G3_POS, EQ_G4_POS,
	EQ_HPF_POS, EQ_LPF_POS, EQ_PRESENCE_POS,
	EQ_MASTER_LOW_GAIN_POS, EQ_MASTER_MID_GAIN_POS, EQ_MASTER_MID_FREQ_POS, EQ_MASTER_HIGH_GAIN_POS, EQ_PREPOST_POS
};

enum
{
	EQ_F0_POS = 0, EQ_F1_POS, EQ_F2_POS, EQ_F3_POS, EQ_F4_POS,
	EQ_Q0_POS, EQ_Q1_POS, EQ_Q2_POS, EQ_Q3_POS, EQ_Q4_POS,
};

typedef struct
{
	TParamDescriptor on;
}TEqDesc;

extern TEqDesc EqDesc;

void eq_on_setter(uint32_t value);

void eq_gain_setter(uint32_t value);
void eq_freq_setter(uint32_t value);
void eq_q_setter(uint32_t value);

void eq_position_setter(uint32_t value);
void lpf_freq_setter(uint32_t value);
void hpf_freq_setter(uint32_t value);

void set_eq_handlers(TTranslator *rl);


#endif /* _EQ_SETTERS_H_ */
