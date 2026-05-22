#include "preamp_setters.h"

#include "console_helpers.h"
#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.preamp,
	.handlerStr = "pr_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_PREAMP,
	.name = "On",
	.setterHandler = preamp_on_setter
};

static TParamDescriptor gainParamDesc = {
	.ptr = &currentPreset.paramData.preamp.gain,
	.handlerStr = "pr_gn",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_GAIN_POS,
	.name = "Gain",
	.setterHandler = preamp_gain_setter
};

static TParamDescriptor volumeParamDesc = {
	.ptr = &currentPreset.paramData.preamp.volume,
	.handlerStr = "pr_vl",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_VOLUME_POS,
	.name = "Volume",
	.setterHandler = preamp_volume_setter
};

static TParamDescriptor lowParamDesc = {
	.ptr = &currentPreset.paramData.preamp.low,
	.handlerStr = "pr_lo",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_LOW_POS,
	.name = "Low",
	.setterHandler = preamp_low_setter
};

static TParamDescriptor midParamDesc = {
	.ptr = &currentPreset.paramData.preamp.mid,
	.handlerStr = "pr_mi",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_MID_POS,
	.name = "Mid",
	.setterHandler = preamp_mid_setter
};

static TParamDescriptor highParamDesc = {
	.ptr = &currentPreset.paramData.preamp.high,
	.handlerStr = "pr_hi",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = PREAMP_HIGH_POS,
	.name = "High",
	.setterHandler = preamp_high_setter
};

static TParamDescriptor dummyDesc = {
	.ptr = &currentPreset.modulesBuf[PREAMP_0],
	.handlerStr = "",
	.dspAddress = DSP_ADDRESS_PREAMP,
	.dspPosition = NOT_SEND_POS,
	.name = "",
	.setterHandler = nullptr
};

TPreampDesc PreampDesc = {
	.on = onParamDesc,
	.gain = gainParamDesc,
	.volume = volumeParamDesc,
	.low = lowParamDesc,
	.mid = midParamDesc,
	.high = highParamDesc,
	.dummy = dummyDesc
};

void preamp_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void preamp_gain_setter(uint32_t value)
{
	default_param_setter(gainParamDesc, value);
}

void preamp_volume_setter(uint32_t value)
{
	default_param_setter(volumeParamDesc, value);
}

void preamp_low_setter(uint32_t value)
{
	default_param_setter(lowParamDesc, value);
}

void preamp_mid_setter(uint32_t value)
{
	default_param_setter(midParamDesc, value);
}

void preamp_high_setter(uint32_t value)
{
	default_param_setter(highParamDesc, value);
}

void set_preamp_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(onParamDesc.handlerStr,  preamp_on_setter);
	translator->AddAccessorHandler(gainParamDesc.handlerStr,  preamp_gain_setter);
	translator->AddAccessorHandler(volumeParamDesc.handlerStr,  preamp_volume_setter);
	translator->AddAccessorHandler(lowParamDesc.handlerStr,  preamp_low_setter);
	translator->AddAccessorHandler(midParamDesc.handlerStr,  preamp_mid_setter);
	translator->AddAccessorHandler(highParamDesc.handlerStr,  preamp_high_setter);
}
