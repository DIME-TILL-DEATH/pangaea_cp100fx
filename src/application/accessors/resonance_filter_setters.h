#ifndef _RESONANCE_FILTER_SETTERS_H_
#define _RESONANCE_FILTER_SETTERS_H_

#include "translator.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;
    uint8_t f_type;
    uint8_t f_mod;
    uint8_t lfo_rate;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t resonance;
    uint8_t dyn_threshold;
    uint8_t dyn_attack;
    uint8_t dyn_release;
    uint8_t volume;
    uint8_t ext;
}TRfData;
#pragma pack(pop)

enum resonance_filter_parameters_t
{
	RFILTER_MIX = 130,
	RFILTER_FTYPE,
	RFILTER_FMOD,
	RFILTER_RATE,
	RFILTER_LPF,
	RFILTER_HPF,
	RFILTER_RESONANCE,
	RFILTER_DYN_THRESHOLD,
	RFILTER_DYN_ATTACK,
	RFLITER_DYN_RELEASE,
	RFILTER_VOLUME,
	RFILTER_EXT,
	RFILTER_LFO_TYPE = 144
};

enum
{
	RFILTER_MIX_POS = 0, RFILTER_FTYPE_POS, RFILTER_FMOD_POS, RFILTER_RATE_POS,	RFILTER_LPF_POS, RFILTER_HPF_POS,
	RFILTER_RESONANCE_POS, RFILTER_DYN_THRESHOLD_POS, RFILTER_DYN_ATTACK_POS, RFLITER_DYN_RELEASE_POS,
	RFILTER_VOLUME_POS, RFILTER_EXT_POS, RFILTER_LFO_TYPE_POS,
	RFILTER_TIME_LO_POS, RFILTER_TIME_HI_POS
};

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor filtType;
	TParamDescriptor fmod;
	TParamDescriptor rate;
	TParamDescriptor lpf;
	TParamDescriptor hpf;
	TParamDescriptor resonance;
	TParamDescriptor dynThreshold;
	TParamDescriptor dynAttack;
	TParamDescriptor dynRelease;
	TParamDescriptor volume;
	TParamDescriptor lfo;
}TRfDesc;

extern TRfDesc RfDesc;


void rf_on_setter(uint32_t value);
void rf_mix_setter(uint32_t value);
void rf_filter_type_setter(uint32_t value);
void rf_fmod_setter(uint32_t value);
void rf_rate_setter(uint32_t value);
void rf_lpf_setter(uint32_t value);
void rf_hpf_setter(uint32_t value);
void rf_resonance_setter(uint32_t value);
void rf_dyn_threshold_setter(uint32_t value);
void rf_dyn_attack_setter(uint32_t value);
void rf_dyn_release_setter(uint32_t value);
void rf_volume_setter(uint32_t value);
void rf_lfo_type_setter(uint32_t value);

void set_resonance_filter_handlers(TTranslator *rl);

#endif /* _RESONANCE_FILTER_SETTERS_H_ */
