#include "reverb_setters.h"

#include "console_helpers.h"
#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.reverb,
	.handlerStr = "rv_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_REVERB,
	.name = "On",
	.setterHandler = reverb_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.reverb.mix,
	.handlerStr = "rv_mx",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_MIX_POS,
	.name = "Mix",
	.setterHandler = reverb_mix_setter
};

static TParamDescriptor typeParamDesc = {
	.ptr = &currentPreset.paramData.reverb_type,
	.handlerStr = "rv_tp",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_TYPE_POS,
	.name = "Type",
	.setterHandler = reverb_type_setter
};

static TParamDescriptor timeParamDesc = {
	.ptr = &currentPreset.paramData.reverb.time,
	.handlerStr = "rv_tm",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_TIME_POS,
	.name = "Time",
	.setterHandler = reverb_time_setter
};

static TParamDescriptor sizeParamDesc = {
	.ptr = &currentPreset.paramData.reverb.size,
	.handlerStr = "rv_sz",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_SIZE_POS,
	.name = "Size",
	.setterHandler = reverb_size_setter
};

static TParamDescriptor dampingParamDesc = {
	.ptr = &currentPreset.paramData.reverb.damping,
	.handlerStr = "rv_dp",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_DAMPING_POS,
	.name = "Damp",
	.setterHandler = reverb_damping_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.reverb.hpf,
	.handlerStr = "rv_hp",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_HPF_POS,
	.name = "HPF",
	.setterHandler = reverb_hpf_setter
};

static TParamDescriptor lpfParamDesc = {
	.ptr = &currentPreset.paramData.reverb.lpf,
	.handlerStr = "rv_lp",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_LPF_POS,
	.name = "LPF",
	.setterHandler = reverb_lpf_setter
};

static TParamDescriptor detuneParamDesc = {
	.ptr = &currentPreset.paramData.reverb.detune,
	.handlerStr = "rv_dt",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_DETUNE_POS,
	.name = "Detune",
	.setterHandler = reverb_detune_setter
};

static TParamDescriptor diffusionParamDesc = {
	.ptr = &currentPreset.paramData.reverb_diffusion,
	.handlerStr = "rv_df",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_DIFFUSION_POS,
	.name = "Diffus",
	.setterHandler = reverb_diffusion_setter
};

static TParamDescriptor predelayParamDesc = {
	.ptr = &currentPreset.paramData.reverb_predelay,
	.handlerStr = "rv_pd",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_PREDELAY_POS,
	.name = "PreDel",
	.setterHandler = reverb_predelay_setter
};

static TParamDescriptor tailParamDesc = {
	.ptr = &currentPreset.paramData.reverb_tail,
	.handlerStr = "rv_tl",
	.dspAddress = DSP_ADDRESS_REVERB,
	.dspPosition = REVERB_TAIL_POS,
	.name = "Tail",
	.setterHandler = reverb_tail_setter
};

TReverbDesc ReverbDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.type = typeParamDesc,
	.time = timeParamDesc,
	.size = sizeParamDesc,
	.damping = dampingParamDesc,
	.hpf = hpfParamDesc,
	.lpf = lpfParamDesc,
	.detune = detuneParamDesc,
	.diffusion = diffusionParamDesc,
	.predelay = predelayParamDesc,
	.tail = tailParamDesc
};

void reverb_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void reverb_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void reverb_type_setter(uint32_t value)
{
	default_param_setter(typeParamDesc, value);
}

void reverb_time_setter(uint32_t value)
{
	default_param_setter(timeParamDesc, value);
}

void reverb_size_setter(uint32_t value)
{
	default_param_setter(sizeParamDesc, value);
}

void reverb_damping_setter(uint32_t value)
{
	default_param_setter(dampingParamDesc, value);
}

void reverb_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void reverb_lpf_setter(uint32_t value)
{
	default_param_setter(lpfParamDesc, value);
}

void reverb_detune_setter(uint32_t value)
{
	default_param_setter(detuneParamDesc, value);
}

void reverb_diffusion_setter(uint32_t value)
{
	default_param_setter(diffusionParamDesc, value);
}

void reverb_predelay_setter(uint32_t value)
{
	default_param_setter(predelayParamDesc, value);
}

void reverb_tail_setter(uint32_t value)
{
	default_param_setter(tailParamDesc, value);
}

void set_reverb_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  reverb_on_setter);
	rl->AddAccessorHandler(mixParamDesc.handlerStr,  reverb_mix_setter);
	rl->AddAccessorHandler(typeParamDesc.handlerStr,  reverb_type_setter);
	rl->AddAccessorHandler(timeParamDesc.handlerStr,  reverb_time_setter);
	rl->AddAccessorHandler(sizeParamDesc.handlerStr,  reverb_size_setter);
	rl->AddAccessorHandler(dampingParamDesc.handlerStr,  reverb_damping_setter);
	rl->AddAccessorHandler(hpfParamDesc.handlerStr,  reverb_hpf_setter);
	rl->AddAccessorHandler(lpfParamDesc.handlerStr,  reverb_lpf_setter);
	rl->AddAccessorHandler(detuneParamDesc.handlerStr,  reverb_detune_setter);
	rl->AddAccessorHandler(diffusionParamDesc.handlerStr,  reverb_diffusion_setter);
	rl->AddAccessorHandler(predelayParamDesc.handlerStr,  reverb_predelay_setter);
	rl->AddAccessorHandler(tailParamDesc.handlerStr,  reverb_tail_setter);
}
