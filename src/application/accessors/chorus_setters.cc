#include "chorus_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.chorus,
	.handlerStr = "ch_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_CHORUS,
	.name = "On",
	.setterHandler = chorus_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.chorus.mix,
	.handlerStr = "ch_mx",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_MIX_POS,
	.name = "Mix",
	.setterHandler = chorus_mix_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.chorus.rate,
	.handlerStr = "ch_rt",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_RATE_POS,
	.name = "Rate",
	.setterHandler = chorus_rate_setter
};

static TParamDescriptor widthParamDesc = {
	.ptr = &currentPreset.paramData.chorus.width,
	.handlerStr = "ch_wd",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_WIDTH_POS,
	.name = "Width",
	.setterHandler = chorus_width_setter
};

static TParamDescriptor delayParamDesc = {
	.ptr = &currentPreset.paramData.chorus.delay,
	.handlerStr = "ch_dl",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_DELAY_POS,
	.name = "Delay",
	.setterHandler = chorus_delay_setter
};

static TParamDescriptor typeParamDesc = {
	.ptr = &currentPreset.paramData.chorus.type,
	.handlerStr = "ch_tp",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_TYPE_POS,
	.name = "Type",
	.setterHandler = chorus_type_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.hpf_chorus,
	.handlerStr = "ch_hp",
	.dspAddress = DSP_ADDRESS_CHORUS,
	.dspPosition = CHORUS_HPF_POS,
	.name = "HPF",
	.setterHandler = chorus_hpf_setter
};

TChorusDesc ChorusDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.rate = rateParamDesc,
	.width = widthParamDesc,
	.delay = delayParamDesc,
	.type = typeParamDesc,
	.hpf = hpfParamDesc
};

void chorus_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void chorus_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void chorus_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void chorus_width_setter(uint32_t value)
{
	default_param_setter(widthParamDesc, value);
}

void chorus_delay_setter(uint32_t value)
{
	default_param_setter(delayParamDesc, value);
}

void chorus_type_setter(uint32_t value)
{
	default_param_setter(typeParamDesc, value);
}

void chorus_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void set_chorus_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  chorus_on_setter);
	rl->AddAccessorHandler(mixParamDesc.handlerStr,  chorus_mix_setter);
	rl->AddAccessorHandler(rateParamDesc.handlerStr,  chorus_rate_setter);
	rl->AddAccessorHandler(widthParamDesc.handlerStr,  chorus_width_setter);
	rl->AddAccessorHandler(delayParamDesc.handlerStr,  chorus_delay_setter);
	rl->AddAccessorHandler(typeParamDesc.handlerStr,  chorus_type_setter);
	rl->AddAccessorHandler(hpfParamDesc.handlerStr,  chorus_hpf_setter);
}
