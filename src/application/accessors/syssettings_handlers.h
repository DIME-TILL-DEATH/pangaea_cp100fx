#ifndef _SYSSETTINGS_HANDLERS_H_
#define _SYSSETTINGS_HANDLERS_H_

#include "translator.h"



typedef struct
{
	TParamDescriptor cabMode;
	TParamDescriptor cabNum;
	TParamDescriptor midiChannel;
	TParamDescriptor exprOn;
	TParamDescriptor exprType;
	TParamDescriptor exprCc;
	TParamDescriptor exprLev;
	TParamDescriptor exprCalibrate;
	TParamDescriptor spdif;
	TParamDescriptor tempo;
	TParamDescriptor timeFormat;
	TParamDescriptor swapConfig;
	TParamDescriptor tunerSpeed;
	TParamDescriptor tunerCtrl;
	TParamDescriptor tunerCc;
	TParamDescriptor fswSpeed;
}TSysSettingsDesc;

extern TSysSettingsDesc SysSettingsDesc;

void cab_mode_setter(uint32_t value);
void cab_num_setter(uint32_t value);
void midi_ch_setter(uint32_t value);
void expr_on_setter(uint32_t value);
void expr_type_setter(uint32_t value);
void expr_cc_setter(uint32_t value);
void expr_slev_setter(uint32_t value);
void spdif_setter(uint32_t value);
void tempo_setter(uint32_t value);
void time_format_setter(uint32_t value);
void swap_conf_setter(uint32_t value);
void tuner_speed_setter(uint32_t value);
void tuner_ctrl_setter(uint32_t value);
void tuner_cc_setter(uint32_t value);
void fsw_speed_setter(uint32_t value);

void midi_map_handler(uint32_t mapPos, uint32_t mapVal);

void set_syssettings_handlers(TTranslator *translator);

#endif /* _SYSSETTINGS_HANDLERS_H_ */
