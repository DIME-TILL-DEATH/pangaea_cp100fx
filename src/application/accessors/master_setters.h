#ifndef _MASTER_SETTERS_H_
#define _MASTER_SETTERS_H_

#include "translator.h"

typedef struct
{
	TParamDescriptor volume;
	TParamDescriptor source;

}TAttenuatorDesc;

typedef struct
{
	TParamDescriptor master;
	TParamDescriptor phones;
}TMasterVolDesc;

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor low;
	TParamDescriptor mid;
	TParamDescriptor midFreq;
	TParamDescriptor high;
}TMasterEqDesc;

extern TAttenuatorDesc AttenuatorDesc;
extern TMasterVolDesc MasterVolDesc;
extern TMasterEqDesc MasterEqDesc;

void attenuator_vol_setter(uint32_t value);
void attenuator_source_setter(uint32_t value);
void master_volume_setter(uint32_t value);
void phones_volume_setter(uint32_t value);
void meq_on_setter(uint32_t value);
void meq_low_setter(uint32_t value);
void meq_mid_setter(uint32_t value);
void meq_high_setter(uint32_t value);
void meq_mid_freq_setter(uint32_t value);

void set_master_handlers(TTranslator *translator);

#endif /* _MASTER_SETTERS_H_ */
