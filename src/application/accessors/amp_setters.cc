#include "amp_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"

#include "sharc_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.amp,
	.handlerStr = "pa_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_AMP,
	.name = "On",
	.setterHandler = amp_on_setter
};

static TParamDescriptor masterParamDesc = {
	.ptr = &currentPreset.paramData.pa.master,
	.handlerStr = "pa_ms",
	.dspAddress = DSP_ADDRESS_AMP,
	.dspPosition = AMP_MASTER_POS,
	.name = "Master",
	.setterHandler = amp_master_setter
};

static TParamDescriptor presenceParamDesc = {
	.ptr = &currentPreset.paramData.presence,
	.handlerStr = "pa_ps",
	.dspAddress = DSP_ADDRESS_EQ,
	.dspPosition = EQ_PRESENCE_POS,
	.name = "Presence",
	.setterHandler = amp_presence_setter
};

static TParamDescriptor levelParamDesc = {
	.ptr = &currentPreset.paramData.pa.level,
	.handlerStr = "pa_lv",
	.dspAddress = DSP_ADDRESS_AMP,
	.dspPosition = AMP_LEVEL_POS,
	.name = "Level",
	.setterHandler = amp_level_setter
};

static TParamDescriptor typeParamDesc = {
	.ptr = &currentPreset.paramData.pa.type,
	.handlerStr = "pa_tp",
	.dspAddress = DSP_ADDRESS_AMP_TYPE,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Type",
	.setterHandler = amp_type_setter
};

TPaDesc PaDesc = {
	.on = onParamDesc,
	.master = masterParamDesc,
	.presence = presenceParamDesc,
	.level = levelParamDesc,
	.type = typeParamDesc
};

void amp_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void amp_master_setter(uint32_t value)
{
	default_param_setter(masterParamDesc, value);}

void amp_presence_setter(uint32_t value)
{
	default_param_setter(presenceParamDesc, value);
}

void amp_level_setter(uint32_t value)
{
	default_param_setter(levelParamDesc, value);
}

void amp_type_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)typeParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", typeParamDesc.handlerStr, *param_ptr);
	SharcTask->setParameter(typeParamDesc.dspAddress, *param_ptr, 0);
}

void set_amp_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  amp_on_setter);
	rl->AddAccessorHandler(masterParamDesc.handlerStr,  amp_master_setter);	//master
	rl->AddAccessorHandler(presenceParamDesc.handlerStr,  amp_presence_setter);
	rl->AddAccessorHandler(levelParamDesc.handlerStr,  amp_level_setter);	//level
	rl->AddAccessorHandler(typeParamDesc.handlerStr,  amp_type_setter);
}
