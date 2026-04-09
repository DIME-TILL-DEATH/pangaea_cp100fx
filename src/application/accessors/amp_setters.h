#ifndef _AMP_SETTERS_H_
#define _AMP_SETTERS_H_

#include "translator.h"
#include "module_parameter.h"

enum amp_parameters_t
{
	AMP_PRESENCE = 55,
	AMP_MASTER = 96,
	AMP_TYPE,
	AMP_LEVEL
};

enum
{
	AMP_MASTER_POS = 0, AMP_TYPE_POS = PARAM_EQUAL_POS, AMP_LEVEL_POS = 2
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t master;
    uint8_t type;
    uint8_t level;
}TPaData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor master;
	TParamDescriptor presence;
	TParamDescriptor level;
	TParamDescriptor type;
}TPaDesc;

extern TPaDesc PaDesc;

void amp_on_setter(uint32_t value);
void amp_master_setter(uint32_t value);
void amp_presence_setter(uint32_t value);
void amp_level_setter(uint32_t value);
void amp_type_setter(uint32_t value);

void set_amp_handlers(TTranslator *rl);

#endif /* _AMP_SETTERS_H_ */
