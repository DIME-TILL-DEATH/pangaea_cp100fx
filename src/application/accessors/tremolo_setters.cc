#include "tremolo_setters.h"

#include "console_helpers.h"
#include "modules.h"
#include "preset.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.tremolo,
	.handlerStr = "tr_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_TREMOLO,
	.name = "On",
	.setterHandler = tremolo_on_setter
};

static TParamDescriptor depthParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.depth,
	.handlerStr = "tr_dp",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_INTENSITY_POS,
	.name = "Intens",
	.setterHandler = tremolo_intensity_setter
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.rate,
	.handlerStr = "tr_rt",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_RATE_POS,
	.name = "Rate",
	.setterHandler = tremolo_rate_setter
};

static TParamDescriptor lfotypeParamDesc = {
	.ptr = &currentPreset.paramData.tremolo_lfo_type,
	.handlerStr = "tr_fm",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_LFO_TYPE_POS,
	.name = "LFOtype",
	.setterHandler = tremolo_lfotype_setter
};

static TParamDescriptor lfomodParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.lfo_mod,
	.handlerStr = "tf_lm",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_LFO_MOD_POS,
	.name = "LFO mod",
	.setterHandler = tremolo_lfomod_setter
};

static TParamDescriptor msParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.ms,
	.handlerStr = "tr_ms",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_MS_POS,
	.name = "Type",
	.setterHandler = tremolo_ms_setter
};

static TParamDescriptor tapParamDesc = {
	.ptr = &currentPreset.paramData.tremolo_tap,
	.handlerStr = "tr_tp",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = NOT_SEND_POS,
	.name = "TAP",
	.setterHandler = tremolo_tap_setter
};

TTremoloDesc TremoloDesc = {
	.on = onParamDesc,
	.depth = depthParamDesc,
	.rate = rateParamDesc,
	.lfo_type = lfotypeParamDesc,
	.lfo_mod = lfomodParamDesc,
	.ms = msParamDesc,
	.tap = tapParamDesc
};

void tremolo_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void tremolo_intensity_setter(uint32_t value)
{
	default_param_setter(depthParamDesc, value);}

void tremolo_rate_setter(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void tremolo_lfotype_setter(uint32_t value)
{
	default_param_setter(lfotypeParamDesc, value);
}

void tremolo_lfomod_setter(uint32_t value)
{
	default_param_setter(lfomodParamDesc, value);
}

void tremolo_ms_setter(uint32_t value)
{
	default_param_setter(msParamDesc, value);
}

void tremolo_tap_setter(uint32_t value)
{
	uint8_t* param_ptr = (uint8_t*)tapParamDesc.ptr;
	*param_ptr = value;
	console_printf("%s\r%02x\n", tapParamDesc.handlerStr, *param_ptr);
}

void set_tremolo_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler(onParamDesc.handlerStr,  tremolo_on_setter);
	translator->AddAccessorHandler(depthParamDesc.handlerStr,  tremolo_intensity_setter);
	translator->AddAccessorHandler(rateParamDesc.handlerStr,  tremolo_rate_setter);
	translator->AddAccessorHandler(lfotypeParamDesc.handlerStr,  tremolo_lfotype_setter);
	translator->AddAccessorHandler(lfomodParamDesc.handlerStr,  tremolo_lfomod_setter);
	translator->AddAccessorHandler(msParamDesc.handlerStr,  tremolo_ms_setter);
	translator->AddAccessorHandler(tapParamDesc.handlerStr,  tremolo_tap_setter);
}
