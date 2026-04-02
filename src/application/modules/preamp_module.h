#ifndef _PREAMP_MODULE_H_
#define _PREAMP_MODULE_H_

#include "translator.h"
#include "module_parameter.h"

enum preamp_parameters_t
{
	PREAMP_GAIN = 111,
	PREAMP_VOLUME,
	PREAMP_0,
	PREAMP_LOW,
	PREAMP_MID,
	PREAMP_HIGH
};

enum
{
	PREAMP_GAIN_POS = 0, PREAMP_VOLUME_POS, PREAMP_0_POS, PREAMP_LOW_POS, PREAMP_MID_POS, PREAMP_HIGH_POS
};


#pragma pack(push, 1)
typedef struct
{
    uint8_t gain;
    uint8_t volume;
    uint8_t pre_0;
    uint8_t low;
    uint8_t mid;
    uint8_t high;
}TPreampData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor gain;
	TParamDescriptor volume;
	TParamDescriptor low;
	TParamDescriptor mid;
	TParamDescriptor high;
}TPreampDesc;

extern TPreampDesc PreampDesc;

void set_preamp_handlers(TTranslator *rl);

void preamp_on_handler(uint32_t value);
void preamp_gain_handler(uint32_t value);
void preamp_volume_handler(uint32_t value);
void preamp_low_handler(uint32_t value);
void preamp_mid_handler(uint32_t value);
void preamp_high_handler(uint32_t value);


#endif /* _CONSOLE_PREAMP_HANDLERS_H_ */
