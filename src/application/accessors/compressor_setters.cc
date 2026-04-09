#include "compressor_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.compressor,
	.handlerStr = "cm_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_COMPRESSOR,
	.name = "On",
	.setterHandler = compressor_on_setter
};

static TParamDescriptor thresholdParamDesc = {
	.ptr = &currentPreset.paramData.compressor.threshold,
	.handlerStr = "cm_th",
	.dspAddress = DSP_ADDRESS_COMPRESSOR,
	.dspPosition = COMPRESSOR_THRESHOLD_POS,
	.name = "Thresh",
	.setterHandler = compressor_threshold_setter
};

static TParamDescriptor ratioParamDesc = {
	.ptr = &currentPreset.paramData.compressor.ratio,
	.handlerStr = "cm_rt",
	.dspAddress = DSP_ADDRESS_COMPRESSOR,
	.dspPosition = COMPRESSOR_RATIO_POS,
	.name = "Ratio",
	.setterHandler = compressor_ratio_setter
};

static TParamDescriptor volumeParamDesc = {
	.ptr = &currentPreset.paramData.compressor.volume,
	.handlerStr = "cm_vl",
	.dspAddress = DSP_ADDRESS_COMPRESSOR,
	.dspPosition = COMPRESSOR_VOLUME_POS,
	.name = "Volume",
	.setterHandler = compressor_volume_setter
};

static TParamDescriptor attackParamDesc = {
	.ptr = &currentPreset.paramData.compressor.attack,
	.handlerStr = "cm_at",
	.dspAddress = DSP_ADDRESS_COMPRESSOR,
	.dspPosition = COMPRESSOR_ATTACK_POS,
	.name = "Attack",
	.setterHandler = compressor_attack_setter
};

static TParamDescriptor kneeParamDesc = {
	.ptr = &currentPreset.paramData.compressor.knee,
	.handlerStr = "cm_rl", // was Release
	.dspAddress = DSP_ADDRESS_COMPRESSOR,
	.dspPosition = COMPRESSOR_KNEE_POS,
	.name = "Knee",
	.setterHandler = compressor_knee_setter
};

TCompressorDesc CompressorDesc = {
	.on = onParamDesc,
	.threshold = thresholdParamDesc,
	.ratio = ratioParamDesc,
	.volume = volumeParamDesc,
	.attack = attackParamDesc,
	.knee = kneeParamDesc
};

void compressor_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void compressor_threshold_setter(uint32_t value)
{
	default_param_setter(thresholdParamDesc, value);
}

void compressor_ratio_setter(uint32_t value)
{
	default_param_setter(ratioParamDesc, value);
}

void compressor_volume_setter(uint32_t value)
{
	default_param_setter(volumeParamDesc, value);
}

void compressor_attack_setter(uint32_t value)
{
	default_param_setter(attackParamDesc, value);
}

void compressor_knee_setter(uint32_t value)
{
	default_param_setter(kneeParamDesc, value);
}

void set_compressor_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  compressor_on_setter);
	rl->AddAccessorHandler(thresholdParamDesc.handlerStr,  compressor_threshold_setter);
	rl->AddAccessorHandler(ratioParamDesc.handlerStr,  compressor_ratio_setter);
	rl->AddAccessorHandler(volumeParamDesc.handlerStr,  compressor_volume_setter);
	rl->AddAccessorHandler(attackParamDesc.handlerStr,  compressor_attack_setter);
	rl->AddAccessorHandler(kneeParamDesc.handlerStr,  compressor_knee_setter);
}
