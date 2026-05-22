#include "eq_setters.h"

#include "console_helpers.h"
#include "modules.h"

#include "sharc_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.eq,
	.handlerStr = "eq_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_EQ,
	.name = "On",
	.setterHandler = eq_on_setter
};

TEqDesc EqDesc = {
	.on = onParamDesc
};

void eq_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void eq_gain_setter(uint32_t value)
{
	uint8_t bandNum = (value >> 16) & 0xFF;
	uint8_t* param_ptr = &currentPreset.paramData.eq_gain[bandNum];
	*param_ptr = value & 0xFF;

	SharcTask->setParameter(DSP_ADDRESS_EQ, bandNum, *param_ptr);
	console_printf("eq_g %d\r%02x\n", bandNum, *param_ptr);
}

void eq_freq_setter(uint32_t value)
{
	uint8_t bandNum = (value >> 16) & 0xFF;
	uint8_t* param_ptr = &currentPreset.paramData.eq_freq[bandNum];
	*param_ptr = value & 0xFF;

	SharcTask->setParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + bandNum, *param_ptr);
	console_printf("eq_f %d\r%02x\n", bandNum, *param_ptr);
}

void eq_q_setter(uint32_t value)
{
	uint8_t bandNum = (value >> 16) & 0xFF;
	uint8_t* param_ptr = &currentPreset.paramData.eq_q[bandNum];
	*param_ptr = value & 0xFF;

	SharcTask->setParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + bandNum, *param_ptr);
	console_printf("eq_q %d\r%02x\n", bandNum, *param_ptr);
}

void eq_position_setter(uint32_t value)
{
	currentPreset.paramData.eq_pre_post = value;
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_PREPOST_POS, currentPreset.paramData.eq_pre_post);
	console_printf("eq_pp\r%02x\n", currentPreset.paramData.eq_pre_post);
}

void lpf_freq_setter(uint32_t value)
{
	currentPreset.paramData.lpf = value;
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_LPF_POS, currentPreset.paramData.lpf);
	console_printf("lp_f\r%02x\n", currentPreset.paramData.lpf);
}

void hpf_freq_setter(uint32_t value)
{
	currentPreset.paramData.hpf = value;
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_HPF_POS, currentPreset.paramData.hpf);
	console_printf("hp_f\r%02x\n", currentPreset.paramData.hpf);
}

void set_eq_handlers(TTranslator *rl)
{
	rl->AddAccessorHandler(onParamDesc.handlerStr, eq_on_setter);

	rl->AddAccessorHandler("eq_g", eq_gain_setter);
	rl->AddAccessorHandler("eq_f", eq_freq_setter);
	rl->AddAccessorHandler("eq_q", eq_q_setter);

	rl->AddAccessorHandler("eq_pp", eq_position_setter);
	rl->AddAccessorHandler("lp_f", lpf_freq_setter);
	rl->AddAccessorHandler("hp_f", hpf_freq_setter);
}
