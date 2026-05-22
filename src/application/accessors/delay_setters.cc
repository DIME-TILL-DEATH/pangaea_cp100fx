#include "delay_setters.h"

#include "console_helpers.h"

#include "modules.h"
#include "preset.h"
#include "system.h"

#include "sharc_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.delay,
	.handlerStr = "dl_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_DELAY,
	.name = "On",
	.setterHandler = delay_on_setter
};

static TParamDescriptor mixParamDesc = {
	.ptr = &currentPreset.paramData.delay.volumeFirst,
	.handlerStr = "dl_m1",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_MIX_POS,
	.name = "Mix",
	.setterHandler = delay_mix_setter
};

static TParamDescriptor vol2ParamDesc = {
	.ptr = &currentPreset.paramData.delay.volumeSecond,
	.handlerStr = "dl_m2",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_VOLUME2_POS,
	.name = "D2 Vol",
	.setterHandler = delay_volume2_setter
};

static TParamDescriptor pan1ParamDesc = {
	.ptr = &currentPreset.paramData.delay.panFirst,
	.handlerStr = "dl_p1",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_PAN1_POS,
	.name = "D1 Pan",
	.setterHandler = delay_pan1_setter
};

static TParamDescriptor pan2ParamDesc = {
	.ptr = &currentPreset.paramData.delay.panSecond,
	.handlerStr = "dl_p2",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_PAN2_POS,
	.name = "D2 Pan",
	.setterHandler = delay_pan2_setter
};

static TParamDescriptor timeParamDesc = {
	.ptr = (uint8_t*)&currentPreset.delayTime,
	.handlerStr = "dl_tm",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = NOT_SEND_POS,
	.name = "Time",
	.setterHandler = delay_time_setter
};

static TParamDescriptor feedbackParamDesc = {
	.ptr = &currentPreset.paramData.delay.feedback,
	.handlerStr = "dl_fb",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_FEEDBACK_POS,
	.name = "F_Back",
	.setterHandler = delay_feedback_setter
};

static TParamDescriptor lpfParamDesc = {
	.ptr = &currentPreset.paramData.delay.lpf,
	.handlerStr = "dl_lp",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_LPF_POS,
	.name = "LPF",
	.setterHandler = delay_lpf_setter
};

static TParamDescriptor hpfParamDesc = {
	.ptr = &currentPreset.paramData.delay.hpf,
	.handlerStr = "dl_hp",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_HPF_POS,
	.name = "HPF",
	.setterHandler = delay_hpf_setter
};

static TParamDescriptor offsetParamDesc = {
	.ptr = &currentPreset.paramData.delay.offset,
	.handlerStr = "dl_os",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_OFFSET_POS,
	.name = "D->D2",
	.setterHandler = delay_offset_setter
};

static TParamDescriptor modulationParamDesc = {
	.ptr = &currentPreset.paramData.delay.modulation,
	.handlerStr = "dl_md",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_MODULATION_POS,
	.name = "D_Mod",
	.setterHandler = delay_modulation_setter
};

static TParamDescriptor directionParamDesc = {
	.ptr = &currentPreset.paramData.delay.direction,
	.handlerStr = "dl_dr",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_DIRECTION_POS,
	.name = "Direct",
	.setterHandler = delay_direction_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.delay.rate,
	.handlerStr = "dl_rt",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = DELAY_MODULATION_RATE_POS,
	.name = "M_Rate",
	.setterHandler = delay_rate_setter
};

static TParamDescriptor tapParamDesc = {
	.ptr = &currentPreset.paramData.delay_tap,
	.handlerStr = "dl_tp",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = NOT_SEND_POS,
	.name = "TAP",
	.setterHandler = delay_tap_setter
};

static TParamDescriptor tailParamDesc = {
	.ptr = &currentPreset.paramData.delay_tail,
	.handlerStr = "dl_tl",
	.dspAddress = DSP_ADDRESS_DELAY,
	.dspPosition = NOT_SEND_POS,
	.name = "Tail",
	.setterHandler = delay_tail_setter
};

TDelayDesc DelayDesc ={
	.on = onParamDesc,
	.mix = mixParamDesc,
	.volume2 = vol2ParamDesc,
	.pan1 = pan1ParamDesc,
	.pan2 = pan2ParamDesc,
	.time = timeParamDesc,
	.feedback = feedbackParamDesc,
	.lpf = lpfParamDesc,
	.hpf = hpfParamDesc,
	.offset = offsetParamDesc,
	.modulation = modulationParamDesc,
	.rate = rateParamDesc,
	.direction = directionParamDesc,
	.tap = tapParamDesc,
	.tail = tailParamDesc
};

void delay_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void delay_mix_setter(uint32_t value)
{
	default_param_setter(mixParamDesc, value);
}

void delay_volume2_setter(uint32_t value)
{
	default_param_setter(vol2ParamDesc, value);
}

void delay_pan1_setter(uint32_t value)
{
	default_param_setter(pan1ParamDesc, value);
}

void delay_pan2_setter(uint32_t value)
{
	default_param_setter(pan2ParamDesc, value);
}

void delay_time_setter(uint32_t value)
{
	currentPreset.delayTime = value;

	currentPreset.paramData.delay_time_lo = currentPreset.delayTime >> 8;
	currentPreset.paramData.delay_time_hi = currentPreset.delayTime & 0xFF;

	console_printf("%s\r%04x\n", timeParamDesc.handlerStr, currentPreset.delayTime);

	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, currentPreset.paramData.delay_time_lo);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, currentPreset.paramData.delay_time_hi);
}

void delay_feedback_setter(uint32_t value)
{
	default_param_setter(feedbackParamDesc, value);
}

void delay_lpf_setter(uint32_t value)
{
	default_param_setter(lpfParamDesc, value);
}

void delay_hpf_setter(uint32_t value)
{
	default_param_setter(hpfParamDesc, value);
}

void delay_offset_setter(uint32_t value)
{
	default_param_setter(offsetParamDesc, value);
}

void delay_modulation_setter(uint32_t value)
{
	default_param_setter(modulationParamDesc, value);
}

void delay_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void delay_direction_setter(uint32_t value)
{
	default_param_setter(directionParamDesc, value);
}

void delay_tap_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)tapParamDesc.ptr;

	*param_ptr = value;
	console_printf("%s\r%02x\n", tapParamDesc.handlerStr, *param_ptr);
}

void delay_tail_setter(uint32_t value)
{
	default_param_setter(tailParamDesc, value);
}

void set_delay_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(onParamDesc.handlerStr,  delay_on_setter);
	translator->AddAccessorHandler(mixParamDesc.handlerStr,  delay_mix_setter);
	translator->AddAccessorHandler(vol2ParamDesc.handlerStr,  delay_volume2_setter);
	translator->AddAccessorHandler(pan1ParamDesc.handlerStr,  delay_pan1_setter);
	translator->AddAccessorHandler(pan2ParamDesc.handlerStr,  delay_pan2_setter);
	translator->AddAccessorHandler(timeParamDesc.handlerStr,  delay_time_setter);
	translator->AddAccessorHandler(feedbackParamDesc.handlerStr,  delay_feedback_setter);
	translator->AddAccessorHandler(hpfParamDesc.handlerStr,  delay_hpf_setter);
	translator->AddAccessorHandler(lpfParamDesc.handlerStr,  delay_lpf_setter);
	translator->AddAccessorHandler(offsetParamDesc.handlerStr,  delay_offset_setter);
	translator->AddAccessorHandler(modulationParamDesc.handlerStr,  delay_modulation_setter);
	translator->AddAccessorHandler(rateParamDesc.handlerStr,  delay_rate_setter);
	translator->AddAccessorHandler(directionParamDesc.handlerStr,  delay_direction_setter);
	translator->AddAccessorHandler(tapParamDesc.handlerStr,  delay_tap_setter);
	translator->AddAccessorHandler(tailParamDesc.handlerStr,  delay_tail_setter);

}
