#ifndef _COMPRESSOR_SETTERS_H_
#define _COMPRESSOR_SETTERS_H_

#include "translator.h"
#include "module_parameter.h"

enum compressor_parameters_t
{
	COMPRESSOR_THRESHOLD = 120,
	COMPRESSOR_RATIO,
	COMPRESSOR_VOLUME,
	COMPRESSOR_ATTACK,
	COMPRESSOR_KNEE
};

enum
{
	COMPRESSOR_THRESHOLD_POS, COMPRESSOR_RATIO_POS, COMPRESSOR_VOLUME_POS, COMPRESSOR_ATTACK_POS, COMPRESSOR_KNEE_POS
};


#pragma pack(push, 1)
typedef struct
{
    uint8_t threshold;
    uint8_t ratio;
    uint8_t volume;
    uint8_t attack;
    uint8_t knee;
}TCompressorData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor threshold;
	TParamDescriptor ratio;
	TParamDescriptor volume;
	TParamDescriptor attack;
	TParamDescriptor knee;
}TCompressorDesc;

extern TCompressorDesc CompressorDesc;

void compressor_on_setter(uint32_t value);
void compressor_threshold_setter(uint32_t value);
void compressor_ratio_setter(uint32_t value);
void compressor_volume_setter(uint32_t value);
void compressor_attack_setter(uint32_t value);
void compressor_knee_setter(uint32_t value);

void set_compressor_handlers(TTranslator *rl);

#endif /* _COMPRESSOR_SETTERS_H_ */
