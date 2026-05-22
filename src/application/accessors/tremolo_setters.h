#ifndef _TREMOLO_SETTERS_H_
#define _TREMOLO_SETTERS_H_

#include "translator.h"
#include "module_parameter.h"

enum tremolo_parameters_t
{
	TREMOLO_INTENSITY = 91,
	TREMOLO_RATE,
	TREMOLO_LFO_MOD,
	TREMOLO_MS,
	TREMOLO_TAP = 129,
	TREMOLO_LFO_TYPE = 158
};

// Порядок отсылки не как в меню
enum
{
	TREMOLO_INTENSITY_POS, TREMOLO_RATE_POS, TREMOLO_LFO_MOD_POS,
	TREMOLO_MS_POS,
	TREMOLO_TIME_LO_POS,
	TREMOLO_TIME_HI_POS,
	TREMOLO_LFO_TYPE_POS = 6
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t depth;  // volume
    uint8_t rate;
    uint8_t lfo_mod;
    uint8_t ms;
}TTremoloData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor depth;
	TParamDescriptor rate;
	TParamDescriptor lfo_type;
	TParamDescriptor lfo_mod;
	TParamDescriptor ms;
	TParamDescriptor tap;
}TTremoloDesc;

extern TTremoloDesc TremoloDesc;

void tremolo_on_setter(uint32_t value);
void tremolo_intensity_setter(uint32_t value);
void tremolo_rate_setter(uint32_t value);
void tremolo_lfotype_setter(uint32_t value);
void tremolo_lfomod_setter(uint32_t value);
void tremolo_ms_setter(uint32_t value);
void tremolo_tap_setter(uint32_t value);

void set_tremolo_handlers(TTranslator *rl);

#endif /* _TREMOLO_SETTERS_H_ */
