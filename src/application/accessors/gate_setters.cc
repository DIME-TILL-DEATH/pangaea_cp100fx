#include "gate_setters.h"

#include "console_helpers.h"

#include "preset.h"
#include "modules.h"
#include "module_parameter.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.gate,
	.handlerStr = "ng_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_GATE,
	.name = "On",
	.setterHandler = gate_on_setter
};

static TParamDescriptor thresholdParamDesc = {
	.ptr = &currentPreset.paramData.gate.threshold,
	.handlerStr = "ng_th",
	.dspAddress = DSP_ADDRESS_GATE,
	.dspPosition = GATE_THRESHOLD_POS,
	.name = "Thresh",
	.setterHandler = gate_threshold_setter
};

static TParamDescriptor attackParamDesc = {
	.ptr = &currentPreset.paramData.gate.attack,
	.handlerStr = "ng_at",
	.dspAddress = DSP_ADDRESS_GATE,
	.dspPosition = GATE_ATTACK_POS,
	.name = "Attack",
	.setterHandler = gate_attack_setter
};

static TParamDescriptor decayParamDesc = {
	.ptr = &currentPreset.paramData.gate.decay,
	.handlerStr = "ng_dc",
	.dspAddress = DSP_ADDRESS_GATE,
	.dspPosition = GATE_DECAY_POS,
	.name = "Decay",
	.setterHandler = gate_decay_setter
};

TGateDesc GateDesc = {
	.on = onParamDesc,
	.threshold = thresholdParamDesc,
	.attack = attackParamDesc,
	.decay = decayParamDesc
};

void gate_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void gate_threshold_setter(uint32_t value)
{
	default_param_setter(thresholdParamDesc, value);
}

void gate_attack_setter(uint32_t value)
{
	default_param_setter(attackParamDesc, value);
}

void gate_decay_setter(uint32_t value)
{
	default_param_setter(decayParamDesc, value);
}

void set_gate_handlers(TTranslator *translate)
{
	translate->AddAccessorHandler(onParamDesc.handlerStr,  gate_on_setter);
	translate->AddAccessorHandler(thresholdParamDesc.handlerStr,  gate_threshold_setter);
	translate->AddAccessorHandler(attackParamDesc.handlerStr,  gate_attack_setter);
	translate->AddAccessorHandler(decayParamDesc.handlerStr,  gate_decay_setter);
}
