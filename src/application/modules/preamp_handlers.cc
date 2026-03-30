#include "preamp_handlers.h"
#include "console_helpers.h"

#include "modules.h"


TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.preamp,
	.handlerStr = "pr_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_PREAMP,
	.name = "",
	.setterHandler = preamp_on_command_handler
};

TParamDescriptor gainParamDesc = {
	.ptr = &currentPreset.paramData.preamp.gain,
	.handlerStr = "pr_gn",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_GAIN_POS,
	.name = "Gain",
	.setterHandler = preamp_gain_command_handler
};

TParamDescriptor volumeParamDesc = {
	.ptr = &currentPreset.paramData.preamp.volume,
	.handlerStr = "pr_vl",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_VOLUME_POS,
	.name = "Volume",
	.setterHandler = preamp_volume_command_handler
};

TParamDescriptor lowParamDesc = {
	.ptr = &currentPreset.paramData.preamp.low,
	.handlerStr = "pr_lo",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_LOW_POS,
	.name = "Low",
	.setterHandler = preamp_low_command_handler
};

TParamDescriptor midParamDesc = {
	.ptr = &currentPreset.paramData.preamp.mid,
	.handlerStr = "pr_mi",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_MID_POS,
	.name = "Mid",
	.setterHandler = preamp_mid_command_handler
};

TParamDescriptor highParamDesc = {
	.ptr = &currentPreset.paramData.preamp.high,
	.handlerStr = "pr_hi",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_HIGH_POS,
	.name = "High",
	.setterHandler = preamp_high_command_handler
};

TPreampDesc PreampDesc = {
	.on = onParamDesc,
	.gain = gainParamDesc,
	.volume = volumeParamDesc,
	.low = lowParamDesc,
	.mid = midParamDesc,
	.high = highParamDesc
};

void preamp_on_command_handler(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void preamp_gain_command_handler(uint32_t value)
{
	default_param_setter(gainParamDesc, value);
}

void preamp_volume_command_handler(uint32_t value)
{
	default_param_setter(volumeParamDesc, value);
}

void preamp_low_command_handler(uint32_t value)
{
	default_param_setter(lowParamDesc, value);
}

void preamp_mid_command_handler(uint32_t value)
{
	default_param_setter(midParamDesc, value);
}

void preamp_high_command_handler(uint32_t value)
{
	default_param_setter(highParamDesc, value);
}

void set_preamp_handlers(TTranslator *rl)
{
	rl->AddSetterHandler(onParamDesc.handlerStr,  preamp_on_command_handler);
	rl->AddSetterHandler(gainParamDesc.handlerStr,  preamp_gain_command_handler);
	rl->AddSetterHandler(volumeParamDesc.handlerStr,  preamp_volume_command_handler);
	rl->AddSetterHandler(lowParamDesc.handlerStr,  preamp_low_command_handler);
	rl->AddSetterHandler(midParamDesc.handlerStr,  preamp_mid_command_handler);
	rl->AddSetterHandler(highParamDesc.handlerStr,  preamp_high_command_handler);
}
