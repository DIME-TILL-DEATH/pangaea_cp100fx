#ifndef _PHASER_SETTERS_H_
#define _PHASER_SETTERS_H_


#include "translator.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t mix;
    uint8_t rate;
    uint8_t width;
    uint8_t center;
    uint8_t feedback;
    uint8_t type;
}TPhaserData;
#pragma pack(pop)

enum phaser_parameters_t
{
	PHASER_MIX = 56,
	PHASER_RATE,
	PHASER_WIDTH,
	PHASER_CENTER,
	PHASER_FEEDBACK,
	PHASER_TYPE,
	PHASER_HPF = 125,
	PHASER_PREPOST = 149
};

enum
{
	PHASER_MIX_POS = 0, PHASER_RATE_POS, PHASER_CENTER_POS, PHASER_WIDTH_POS, PHASER_FEEDBACK_POS, PHASER_TYPE_POS,
	PHASER_HPF_POS, PHASER_PREPOST_POS
};

typedef struct
{
	TParamDescriptor on;
	TParamDescriptor mix;
	TParamDescriptor rate;
	TParamDescriptor width;
	TParamDescriptor center;
	TParamDescriptor feedback;
	TParamDescriptor stages;
	TParamDescriptor hpf;
	TParamDescriptor position;
}TPhaserDesc;

extern TPhaserDesc PhaserDesc;

void phaser_on_setter(uint32_t value);
void phaser_mix_setter(uint32_t value);
void phaser_rate_setter(uint32_t value);
void phaser_width_setter(uint32_t value);
void phaser_center_setter(uint32_t value);
void phaser_feedback_setter(uint32_t value);
void phaser_stages_setter(uint32_t value);
void phaser_hpf_setter(uint32_t value);
void phaser_position_setter(uint32_t value);

void set_phaser_handlers(TTranslator *rl);

#endif /* _PHASER_SETTERS_H_ */
