#include "console_helpers.h"

#include "modules.h"
#include "phaser_setters.h"

#include "sharc_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.phaser,
	.handlerStr = "ph_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_PHASER,
	.name = "On",
	.setterHandler = phaser_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.phaser.mix,
	.handlerStr = "ph_mx",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_MIX_POS,
	.name = "Mix",
	.setterHandler = phaser_mix_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.phaser.rate,
	.handlerStr = "ph_rt",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_RATE_POS,
	.name = "Rate",
	.setterHandler = phaser_rate_setter
};

static TParamDescriptor widthParamDesc = {
	.ptr = &currentPreset.paramData.phaser.width,
	.handlerStr = "ph_wd",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_WIDTH_POS,
	.name = "Width",
	.setterHandler = phaser_width_setter
};

static TParamDescriptor centerParamDesc = {
	.ptr = &currentPreset.paramData.phaser.center,
	.handlerStr = "ph_cn",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_CENTER_POS,
	.name = "Center",
	.setterHandler = phaser_center_setter
};

static TParamDescriptor feedbackParamDesc = {
	.ptr = &currentPreset.paramData.phaser.feedback,
	.handlerStr = "ph_fb",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_FEEDBACK_POS,
	.name = "F_Back",
	.setterHandler = phaser_feedback_setter
};

static TParamDescriptor stagesParamDesc = {
	.ptr = &currentPreset.paramData.phaser.type,
	.handlerStr = "ph_st",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_TYPE_POS,
	.name = "Stages",
	.setterHandler = phaser_stages_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.hpf_phaser,
	.handlerStr = "ph_hp",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_HPF_POS,
	.name = "HPF",
	.setterHandler = phaser_hpf_setter
};


static TParamDescriptor positionParamDesc = {
	.ptr = &currentPreset.paramData.phaser_pre_post,
	.handlerStr = "ph_pp",
	.dspAddress = DSP_ADDRESS_PHASER,
	.dspPosition = PHASER_PREPOST_POS,
	.name = "Pos.",
	.setterHandler = phaser_position_setter
};

TPhaserDesc PhaserDesc = {
	.on = onParamDesc,
	.mix = mixParamDesc,
	.rate = rateParamDesc,
	.width = widthParamDesc,
	.center = centerParamDesc,
	.feedback = feedbackParamDesc,
	.stages = stagesParamDesc,
	.hpf = hpfParamDesc,
	.position = positionParamDesc
};

void phaser_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void phaser_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void phaser_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void phaser_width_setter(uint32_t value)
{
	default_param_setter(widthParamDesc, value);
}

void phaser_center_setter(uint32_t value)
{
	default_param_setter(centerParamDesc, value);
}

void phaser_feedback_setter(uint32_t value)
{
	default_param_setter(feedbackParamDesc, value);
}

void phaser_stages_setter(uint32_t value)
{
	default_param_setter(stagesParamDesc, value);
}

void phaser_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void phaser_position_setter(uint32_t value)
{
	default_param_setter(positionParamDesc, value);
}

void set_phaser_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr,  phaser_on_setter);
	rl->AddAccessorHandler(mixParamDesc.handlerStr,  phaser_mix_setter);
	rl->AddAccessorHandler(rateParamDesc.handlerStr,  phaser_rate_setter);
	rl->AddAccessorHandler(centerParamDesc.handlerStr,  phaser_center_setter);
	rl->AddAccessorHandler(widthParamDesc.handlerStr,  phaser_width_setter);
	rl->AddAccessorHandler(feedbackParamDesc.handlerStr,  phaser_feedback_setter);
	rl->AddAccessorHandler(stagesParamDesc.handlerStr,  phaser_stages_setter);
	rl->AddAccessorHandler(hpfParamDesc.handlerStr,  phaser_hpf_setter);
	rl->AddAccessorHandler(positionParamDesc.handlerStr,  phaser_position_setter);
}
