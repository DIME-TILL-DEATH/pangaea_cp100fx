#ifndef _PRESET_SETTERS_H_
#define _PRESET_SETTERS_H_

#include "translator.h"

void psave_handler(uint32_t value = 0);
void preset_change_handler(uint32_t value);
void preset_volume_setter(uint32_t value);
void preset_volume_control_setter(uint32_t value);

typedef struct
{
	TParamDescriptor level;
	TParamDescriptor control;
}TPresetDesc;

extern TPresetDesc PresetDesc;

void set_preset_handlers(TTranslator *translator);

#endif /* _PRESET_SETTERS_H_ */
