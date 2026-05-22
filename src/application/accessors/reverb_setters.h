#ifndef _REVERB_HANDLERS_H_
#define _REVERB_HANDLERS_H_

#include "translator.h"

enum reverb_parameters_t
{
	REVERB_MIX = 84,
	REVERB_TIME,
	REVERB_SIZE,
	REVERB_DAMPING,
	REVERB_LPF,
	REVERB_HPF,
	REVERB_DETUNE,
	REVERB_TYPE = 142,
	REVERB_DIFFUSION,
	REVERB_PREDELAY = 151,
	REVERB_TAIL = 160
};

enum
{
	REVERB_MIX_POS = 0, REVERB_TYPE_POS, REVERB_TIME_POS, REVERB_SIZE_POS, REVERB_DAMPING_POS,
	REVERB_LPF_POS, REVERB_HPF_POS, REVERB_DETUNE_POS, REVERB_DIFFUSION_POS, REVERB_PREDELAY_POS, REVERB_TAIL_POS
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;
    uint8_t time;
    uint8_t size;
    uint8_t damping;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t detune;
}TReverbData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor type;
	TParamDescriptor time;
	TParamDescriptor size;
	TParamDescriptor damping;
	TParamDescriptor hpf;
	TParamDescriptor lpf;
	TParamDescriptor detune;
	TParamDescriptor diffusion;
	TParamDescriptor predelay;
	TParamDescriptor tail;
}TReverbDesc;

extern TReverbDesc ReverbDesc;

void reverb_on_setter(uint32_t value);
void reverb_mix_setter(uint32_t value);
void reverb_type_setter(uint32_t value);
void reverb_time_setter(uint32_t value);
void reverb_size_setter(uint32_t value);
void reverb_damping_setter(uint32_t value);
void reverb_hpf_setter(uint32_t value);
void reverb_lpf_setter(uint32_t value);
void reverb_detune_setter(uint32_t value);
void reverb_diffusion_setter(uint32_t value);
void reverb_predelay_setter(uint32_t value);
void reverb_tail_setter(uint32_t value);

void set_reverb_handlers(TTranslator *rl);

#endif /* _REVERB_HANDLERS_H_ */
