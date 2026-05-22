#ifndef _CHORUS_SETTERS_H_
#define _CHORUS_SETTERS_H_

#include "translator.h"

enum chorus_parameters_t
{
	CHORUS_MIX = 62,
	CHORUS_RATE,
	CHORUS_WIDTH,
	CHORUS_DELAY,
	CHORUS_TYPE,
	CHORUS_HPF = 127
};

enum
{
	CHORUS_MIX_POS = 0, CHORUS_RATE_POS, CHORUS_WIDTH_POS, CHORUS_DELAY_POS, CHORUS_TYPE_POS, CHORUS_HPF_POS
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;    // volume
    uint8_t rate;
    uint8_t width;
    uint8_t delay;
    uint8_t type;
}TChorusData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor rate;
	TParamDescriptor width;
	TParamDescriptor delay;
	TParamDescriptor type;
	TParamDescriptor hpf;
}TChorusDesc;

extern TChorusDesc ChorusDesc;

void chorus_on_setter(uint32_t value);
void chorus_mix_setter(uint32_t value);
void chorus_rate_setter(uint32_t value);
void chorus_width_setter(uint32_t value);
void chorus_delay_setter(uint32_t value);
void chorus_type_setter(uint32_t value);
void chorus_hpf_setter(uint32_t value);

void set_chorus_handlers(TTranslator *rl);

#endif /* _CHORUS_SETTERS_H_ */
