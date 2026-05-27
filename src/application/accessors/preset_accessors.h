#ifndef _PRESET_SETTERS_H_
#define _PRESET_SETTERS_H_

#include "translator.h"

void psave_handler(uint32_t value = 0);
void preset_change_handler(uint32_t value);
void preset_volume_setter(uint32_t value);
void preset_volume_control_setter(uint32_t value);

#ifdef __STEREO_MOD__
enum{
	IN_LEFT_EN_POS,
	IN_RIGHT_EN_POS,
	IN_LEFT_PAN_POS,
	IN_RIGHT_PAN_POS
};

void preset_inl_on_control_setter(uint32_t value);
void preset_inr_on_control_setter(uint32_t value);
void preset_inl_pan_control_setter(uint32_t value);
void preset_inr_pan_control_setter(uint32_t value);
#endif

typedef struct
{
	TParamDescriptor level;
	TParamDescriptor control;

#ifdef __STEREO_MOD__
	TParamDescriptor inlOn;
	TParamDescriptor inrOn;
	TParamDescriptor inlPan;
	TParamDescriptor inrPan;
#endif
}TPresetDesc;

extern TPresetDesc PresetDesc;

void set_preset_handlers(TTranslator *translator);

#endif /* _PRESET_SETTERS_H_ */
