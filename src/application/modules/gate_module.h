#ifndef _GATE_MODULE_H_
#define _GATE_MODULE_H_

#include "translator.h"
#include "module_parameter.h"

enum gate_parameters_t
{
	GATE_THRESHOLD = 117,
	GATE_ATTACK,
	GATE_DECAY
};

enum
{
	GATE_THRESHOLD_POS, GATE_ATTACK_POS, GATE_DECAY_POS
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t threshold;
    uint8_t attack;
    uint8_t decay;
}TGateData;
#pragma pack(pop)

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor threshold;
	TParamDescriptor attack;
	TParamDescriptor decay;
}TGateDesc;

extern TGateDesc GateDesc;

void gate_on_handler(uint32_t value);
void gate_threshold_handler(uint32_t value);
void gate_attack_handler(uint32_t value);
void gate_decay_handler(uint32_t value);


void set_gate_handlers(TTranslator *rl);

#endif /* _GATE_MODULE_H_ */
