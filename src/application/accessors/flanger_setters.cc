#include "flanger_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.flanger,
	.handlerStr = "fl_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_FLANGER,
	.name = "On",
	.setterHandler = flanger_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.flanger.mix,
	.handlerStr = "fl_mx",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_MIX_POS,
	.name = "Mix",
	.setterHandler = flanger_mix_setter
};

static TParamDescriptor genTypeParamDesc = {
	.ptr = &currentPreset.paramData.flanger.lfo,
	.handlerStr = "fl_lf",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_LFO_POS,
	.name = "LFO",
	.setterHandler = flanger_gen_type_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.flanger.rate,
	.handlerStr = "fl_rt",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_RATE_POS,
	.name = "Rate",
	.setterHandler = flanger_rate_setter
};

static TParamDescriptor widthParamDesc = {
	.ptr = &currentPreset.paramData.flanger.width,
	.handlerStr = "fl_wd",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_WIDTH_POS,
	.name = "Width",
	.setterHandler = flanger_width_setter
};

static TParamDescriptor delayParamDesc = {
	.ptr = &currentPreset.paramData.flanger.delay,
	.handlerStr = "fl_dl",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_DELAY_POS,
	.name = "Delay",
	.setterHandler = flanger_delay_setter
};

static TParamDescriptor feedbackParamDesc = {
	.ptr = &currentPreset.paramData.flanger.feedback,
	.handlerStr = "fl_fb",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_FEEDBACK_POS,
	.name = "F_Back",
	.setterHandler = flanger_feedback_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.hpf_flanger,
	.handlerStr = "fl_hp",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_HPF_POS,
	.name = "HPF",
	.setterHandler = flanger_hpf_setter
};


static TParamDescriptor positionParamDesc = {
	.ptr = &currentPreset.paramData.flanger_pre_post,
	.handlerStr = "fl_pp",
	.dspAddress = DSP_ADDRESS_FLANGER,
	.dspPosition = FLANGER_PREPOST_POS,
	.name = "Pos.",
	.setterHandler = flanger_position_setter
};

TFlangerDesc FlangerDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.genType = genTypeParamDesc,
	.rate = rateParamDesc,
	.width = widthParamDesc,
	.delay = delayParamDesc,
	.feedback = feedbackParamDesc,
	.hpf = hpfParamDesc,
	.position = positionParamDesc
};

void flanger_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void flanger_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void flanger_gen_type_setter(uint32_t value)
{
	default_param_setter(genTypeParamDesc, value);
}

void flanger_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void flanger_width_setter(uint32_t value)
{
	default_param_setter(widthParamDesc, value);
}

void flanger_delay_setter(uint32_t value)
{
	default_param_setter(delayParamDesc, value);
}

void flanger_feedback_setter(uint32_t value)
{
	default_param_setter(feedbackParamDesc, value);
}

void flanger_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void flanger_position_setter(uint32_t value)
{
	default_param_setter(positionParamDesc, value);
}

void set_flanger_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  flanger_on_setter);
	rl->AddAccessorHandler(mixParamDesc.handlerStr,  flanger_mix_setter);
	rl->AddAccessorHandler(genTypeParamDesc.handlerStr,  flanger_gen_type_setter);
	rl->AddAccessorHandler(rateParamDesc.handlerStr,  flanger_rate_setter);
	rl->AddAccessorHandler(widthParamDesc.handlerStr,  flanger_width_setter);
	rl->AddAccessorHandler(delayParamDesc.handlerStr,  flanger_delay_setter);
	rl->AddAccessorHandler(feedbackParamDesc.handlerStr,  flanger_feedback_setter);
	rl->AddAccessorHandler(hpfParamDesc.handlerStr,  flanger_hpf_setter);
	rl->AddAccessorHandler(positionParamDesc.handlerStr,  flanger_position_setter);
}
