#include "early_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"

#include "sharc_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.early_reflections,
	.handlerStr = "er_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_EARLY_REFLECTIONS,
	.name = "On",
	.setterHandler = early_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.early_reflections.mix,
	.handlerStr = "er_mx",
	.dspAddress = DSP_ADDRESS_EARLY_REFLECTIONS,
	.dspPosition = EARLY_MIX_POS,
	.name = "Mix",
	.setterHandler = early_mix_setter
};

static TParamDescriptor sizeParamDesc = {
	.ptr = &currentPreset.paramData.early_reflections.size,
	.handlerStr = "er_sz",
	.dspAddress = DSP_ADDRESS_EARLY_REFLECTIONS,
	.dspPosition = EARLY_SIZE_POS,
	.name = "Size",
	.setterHandler = early_size_setter
};


TEarlyDesc EarlyDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.size = sizeParamDesc
};

void early_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);}

void early_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void early_size_setter(uint32_t value)
{
	default_param_setter(sizeParamDesc, value);
}

void set_early_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(onParamDesc.handlerStr,  early_on_setter);
	translator->AddAccessorHandler(mixParamDesc.handlerStr,  early_mix_setter);
	translator->AddAccessorHandler(sizeParamDesc.handlerStr,  early_size_setter);
}
