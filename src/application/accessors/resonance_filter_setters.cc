#include "resonance_filter_setters.h"
#include "console_helpers.h"

#include "modules.h"

#include "sharc_task.h"



static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.resonance_filter,
	.handlerStr = "rf_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_RESONANCE_FILTER,
	.name = "On",
	.setterHandler = rf_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.mix,
	.handlerStr = "rf_mx",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_MIX_POS,
	.name = "Mix",
	.setterHandler = rf_mix_setter
};

static TParamDescriptor filtTypeParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.f_type,
	.handlerStr = "rf_ft",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_FTYPE_POS,
	.name = "F Type",
	.setterHandler = rf_filter_type_setter
};

static TParamDescriptor lfoParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter_gen_type,
	.handlerStr = "rf_lt",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_LFO_TYPE_POS,
	.name = "F mod",
	.setterHandler = rf_lfo_type_setter
};

static TParamDescriptor fmodParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.f_mod,
	.handlerStr = "rf_fm",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_FMOD_POS,
	.name = "F Mod",
	.setterHandler = rf_fmod_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.lfo_rate,
	.handlerStr = "rf_lr",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_RATE_POS,
	.name = "LFO r",
	.setterHandler = rf_rate_setter
};

static TParamDescriptor lpfParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.lpf,
	.handlerStr = "rf_lp",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_LPF_POS,
	.name = "Lo freq",
	.setterHandler = rf_lpf_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.hpf,
	.handlerStr = "rf_hp",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_HPF_POS,
	.name = "Hi freq",
	.setterHandler = rf_hpf_setter
};

static TParamDescriptor resonanceParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.resonance,
	.handlerStr = "rf_rs",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_RESONANCE_POS,
	.name = "Res",
	.setterHandler = rf_resonance_setter
};

static TParamDescriptor dynThresholdDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.dyn_threshold,
	.handlerStr = "rf_dt",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_DYN_THRESHOLD_POS,
	.name = "Dyn th",
	.setterHandler = rf_dyn_threshold_setter
};

static TParamDescriptor dynAttackParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.dyn_attack,
	.handlerStr = "rf_da",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_DYN_ATTACK_POS,
	.name = "Dyn att",
	.setterHandler = rf_dyn_attack_setter
};

static TParamDescriptor dynReleaseParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.dyn_release,
	.handlerStr = "rf_dr",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFLITER_DYN_RELEASE_POS,
	.name = "Dyn rel",
	.setterHandler = rf_dyn_release_setter
};

static TParamDescriptor volumeParamDesc = {
	.ptr = &currentPreset.paramData.resonance_filter.volume,
	.handlerStr = "rf_vl",
	.dspAddress = DSP_ADDRESS_RESONANCE_FILTER,
	.dspPosition = RFILTER_VOLUME_POS,
	.name = "Volume",
	.setterHandler = rf_volume_setter
};

TRfDesc RfDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.filtType = filtTypeParamDesc,
	.fmod = fmodParamDesc,
	.rate = rateParamDesc,
	.lpf = lpfParamDesc,
	.hpf = hpfParamDesc,
	.resonance = resonanceParamDesc,
	.dynThreshold = dynThresholdDesc,
	.dynAttack = dynAttackParamDesc,
	.dynRelease = dynReleaseParamDesc,
	.volume = volumeParamDesc,
	.lfo = lfoParamDesc
};

void rf_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void rf_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void rf_filter_type_setter(uint32_t value)
{
	default_param_setter(filtTypeParamDesc, value);
}

void rf_fmod_setter(uint32_t value)
{
	default_param_setter(fmodParamDesc, value);
}

void rf_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void rf_lpf_setter(uint32_t value)
{
	default_param_setter(lpfParamDesc, value);
}

void rf_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void rf_resonance_setter(uint32_t value)
{
	default_param_setter(resonanceParamDesc, value);
}

void rf_dyn_threshold_setter(uint32_t value)
{
	default_param_setter(dynThresholdDesc, value);
}

void rf_dyn_attack_setter(uint32_t value)
{
	default_param_setter(dynAttackParamDesc, value);
}

void rf_dyn_release_setter(uint32_t value)
{
	default_param_setter(dynReleaseParamDesc, value);
}

void rf_volume_setter(uint32_t value)
{
	default_param_setter(volumeParamDesc, value);
}

void rf_lfo_type_setter(uint32_t value)
{
	default_param_setter(lfoParamDesc, value);
}

void set_resonance_filter_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(onParamDesc.handlerStr,  rf_on_setter);
	translator->AddAccessorHandler(mixParamDesc.handlerStr,  rf_mix_setter);
	translator->AddAccessorHandler(filtTypeParamDesc.handlerStr,  rf_filter_type_setter);
	translator->AddAccessorHandler(fmodParamDesc.handlerStr,  rf_fmod_setter);
	translator->AddAccessorHandler(rateParamDesc.handlerStr,  rf_rate_setter);
	translator->AddAccessorHandler(lpfParamDesc.handlerStr,  rf_lpf_setter);
	translator->AddAccessorHandler(hpfParamDesc.handlerStr,  rf_hpf_setter);
	translator->AddAccessorHandler(resonanceParamDesc.handlerStr,  rf_resonance_setter);
	translator->AddAccessorHandler(dynThresholdDesc.handlerStr,  rf_dyn_threshold_setter);
	translator->AddAccessorHandler(dynAttackParamDesc.handlerStr,  rf_dyn_attack_setter);
	translator->AddAccessorHandler(dynReleaseParamDesc.handlerStr,  rf_dyn_release_setter);
	translator->AddAccessorHandler(volumeParamDesc.handlerStr,  rf_volume_setter);
	translator->AddAccessorHandler(lfoParamDesc.handlerStr,  rf_lfo_type_setter);
}
