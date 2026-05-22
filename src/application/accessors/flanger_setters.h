#ifndef _FLANGER_SETTERS_H_
#define _FLANGER_SETTERS_H_

#include "translator.h"

enum flanger_parameters_t
{
	FLANGER_MIX = 78,
	FLANGER_LFO,
	FLANGER_RATE,
	FLANGER_WIDTH,
	FLANGER_DELAY,
	FLANGER_FEEDBACK,
	FLANGER_HPF = 126,
	FLANGER_PREPOST = 150
};

enum
{
	FLANGER_MIX_POS, FLANGER_LFO_POS, FLANGER_RATE_POS, FLANGER_WIDTH_POS, FLANGER_DELAY_POS,
	FLANGER_FEEDBACK_POS, FLANGER_HPF_POS, FLANGER_PREPOST_POS
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;
    uint8_t lfo;
    uint8_t rate;
    uint8_t width;
    uint8_t delay;
    uint8_t feedback;
}TFlangerData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor genType;
	TParamDescriptor rate;
	TParamDescriptor width;
	TParamDescriptor delay;
	TParamDescriptor feedback;
	TParamDescriptor hpf;
	TParamDescriptor position;
}TFlangerDesc;

extern TFlangerDesc FlangerDesc;

void flanger_on_setter(uint32_t value);
void flanger_mix_setter(uint32_t value);
void flanger_gen_type_setter(uint32_t value);
void flanger_rate_setter(uint32_t value);
void flanger_width_setter(uint32_t value);
void flanger_delay_setter(uint32_t value);
void flanger_feedback_setter(uint32_t value);
void flanger_hpf_setter(uint32_t value);
void flanger_position_setter(uint32_t value);

void set_flanger_handlers(TTranslator *rl);


#endif /* _FLANGER_SETTERS_H_ */
