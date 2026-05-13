#ifndef _IR_SETTERS_H_
#define _IR_SETTERS_H_

#include "translator.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t volume;
    uint8_t pan;
    uint8_t delay;
}TCabData;
#pragma pack(pop)


enum ir_parameters_t
{
	IR_VOLUME1 = 42,
	IR_PAN1,
	IR_DELAY1,
	IR_VOLUME2,
	IR_PAN2,
	IR_DELAY2
};

enum
{
	IR_VOLUME1_POS, IR_PAN1_POS, IR_DELAY1_POS, IR_VOLUME2_POS, IR_PAN2_POS, IR_DELAY2_POS
};

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor vol1;
	TParamDescriptor pan1;
	TParamDescriptor vol2;
	TParamDescriptor pan2;
	TParamDescriptor dummy;
}TIrDesc;

extern TIrDesc IrDesc;

void ir_on_setter(uint32_t value);
void ir_volume1_setter(uint32_t value);
void ir_pan1_setter(uint32_t value);
void ir_volume2_setter(uint32_t value);
void ir_pan2_setter(uint32_t value);

void ir_cab_setter(uint32_t value);
void ir_cab_restore(uint32_t value);

void set_ir_handlers(TTranslator *translator);

#endif /* _IR_SETTERS_H_ */
