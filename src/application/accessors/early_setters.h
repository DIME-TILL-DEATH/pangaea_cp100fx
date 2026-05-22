#ifndef _EARLY_SETTERS_H_
#define _EARLY_SETTERS_H_

#include "translator.h"
#include "module_parameter.h"

enum early_reflections_parameters_t
{
	EARLY_MIX = 99,
	EARLY_SIZE
};

enum
{
	EARLY_MIX_POS = 0, EARLY_SIZE_POS
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;
    uint8_t size;
}TErData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor size;
}TEarlyDesc;

extern TEarlyDesc EarlyDesc;

void early_on_setter(uint32_t value);
void early_mix_setter(uint32_t value);
void early_size_setter(uint32_t value);

void set_early_handlers(TTranslator *rl);


#endif /* _EARLY_SETTERS_H_ */
