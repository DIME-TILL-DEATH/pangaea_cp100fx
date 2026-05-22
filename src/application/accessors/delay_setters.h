#ifndef _DELAY_SETTERS_H_
#define _DELAY_SETTERS_H_

#include "translator.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t volumeFirst;
    uint8_t feedback;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t panFirst;
    uint8_t volumeSecond;
    uint8_t panSecond;
    uint8_t offset; // d - offset?
    uint8_t modulation;
    uint8_t rate;
    uint8_t direction;
}TDelayData;
#pragma pack(pop)

enum delay_parameters_t
{
	DELAY_MIX = 67,
	DELAY_FEEDBACK,
	DELAY_LPF,
	DELAY_HPF,
	DELAY_PAN1,
	DELAY_VOLUME2,
	DELAY_PAN2,
	DELAY_OFFSET,
	DELAY_MODULATION,
	DELAY_MODULATION_RATE,
	DELAY_DIRECTION,
	DELAY_TAP = 128,
	DELAY_TIME_LO = 147,
	DELAY_TIME_HI,
	DELAY_TAIL = 159
};

enum delay_parameters_pos_t
{
	DELAY_MIX_POS = 0, DELAY_FEEDBACK_POS, DELAY_LPF_POS, DELAY_HPF_POS, DELAY_PAN1_POS, DELAY_VOLUME2_POS, DELAY_PAN2_POS,
	DELAY_OFFSET_POS, DELAY_MODULATION_POS, DELAY_MODULATION_RATE_POS, DELAY_DIRECTION_POS, DELAY_TIME_LO_POS, DELAY_TIME_HI_POS,
	DELAY_TAIL_POS,
};

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor volume2;
	TParamDescriptor pan1;
	TParamDescriptor pan2;
	TParamDescriptor time;
	TParamDescriptor feedback;
	TParamDescriptor lpf;
	TParamDescriptor hpf;
	TParamDescriptor offset;
	TParamDescriptor modulation;
	TParamDescriptor rate;
	TParamDescriptor direction;
	TParamDescriptor tap;
	TParamDescriptor tail;
}TDelayDesc;

extern TDelayDesc DelayDesc;

void delay_on_setter(uint32_t value);
void delay_mix_setter(uint32_t value);
void delay_volume2_setter(uint32_t value);
void delay_pan1_setter(uint32_t value);
void delay_pan2_setter(uint32_t value);
void delay_time_setter(uint32_t value);
void delay_feedback_setter(uint32_t value);
void delay_lpf_setter(uint32_t value);
void delay_hpf_setter(uint32_t value);
void delay_offset_setter(uint32_t value);
void delay_modulation_setter(uint32_t value);
void delay_rate_setter(uint32_t value);
void delay_direction_setter(uint32_t value);
void delay_tap_setter(uint32_t value);
void delay_tail_setter(uint32_t value);

void set_delay_handlers(TTranslator *rl);

#endif /* _DELAY_SETTERS_H_ */
