#include "tremolo_module.h"

#include "console_helpers.h"
#include "modules.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.tremolo,
	.handlerStr = "tr_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_TREMOLO,
	.name = "On",
	.setterHandler = tremolo_on_handler
};

static TParamDescriptor depthParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.depth,
	.handlerStr = "tr_dp",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_INTENSITY_POS,
	.name = "Intens",
	.setterHandler = tremolo_intensity_handler
};

static TParamDescriptor rateParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.rate,
	.handlerStr = "tr_rt",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_RATE_POS,
	.name = "Rate",
	.setterHandler = tremolo_rate_handler
};

static TParamDescriptor lfotypeParamDesc = {
	.ptr = &currentPreset.paramData.tremolo_lfo_type,
	.handlerStr = "tr_fm",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_LFO_TYPE_POS,
	.name = "LFOtype",
	.setterHandler = tremolo_lfotype_handler
};

static TParamDescriptor lfomodParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.lfo_mod,
	.handlerStr = "tf_lm",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_LFO_MOD_POS,
	.name = "LFO mod",
	.setterHandler = tremolo_lfomod_handler
};

static TParamDescriptor msParamDesc = {
	.ptr = &currentPreset.paramData.tremolo.ms,
	.handlerStr = "tf_ms",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = TREMOLO_MS_POS,
	.name = "Type",
	.setterHandler = tremolo_ms_handler
};

static TParamDescriptor tapParamDesc = {
	.ptr = &currentPreset.paramData.tremolo_tap,
	.handlerStr = "tr_tp",
	.dspAddress = DSP_ADDRESS_TREMOLO,
	.dspPosition = NOT_SEND_POS,
	.name = "TAP",
	.setterHandler = tremolo_tap_handler
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

void tremolo_on_handler(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void tremolo_intensity_handler(uint32_t value)
{
	default_param_setter(depthParamDesc, value);}

void tremolo_rate_handler(uint32_t value)
{
	default_param_setter(rateParamDesc, value);
}

void tremolo_lfotype_handler(uint32_t value)
{
	default_param_setter(lfotypeParamDesc, value);
}

void tremolo_lfomod_handler(uint32_t value)
{
	default_param_setter(lfomodParamDesc, value);
}

void tremolo_ms_handler(uint32_t value)
{
	default_param_setter(msParamDesc, value);
}

void tremolo_tap_handler(uint32_t value)
{
	char hex[3] = {0, 0, 0};
	*tapParamDesc.ptr = value;

	i2hex(*tapParamDesc.ptr, hex);
	console_printf("%s\r%s\n", tapParamDesc.handlerStr, hex);
}

void set_tremolo_handlers(TTranslator *translator)
{
	translator->AddSetterHandler(onParamDesc.handlerStr,  tremolo_on_handler);
	translator->AddSetterHandler(depthParamDesc.handlerStr,  tremolo_intensity_handler);
	translator->AddSetterHandler(rateParamDesc.handlerStr,  tremolo_rate_handler);
	translator->AddSetterHandler(lfotypeParamDesc.handlerStr,  tremolo_lfotype_handler);
	translator->AddSetterHandler(lfomodParamDesc.handlerStr,  tremolo_lfomod_handler);
	translator->AddSetterHandler(msParamDesc.handlerStr,  tremolo_ms_handler);
	translator->AddSetterHandler(tapParamDesc.handlerStr,  tremolo_tap_handler);
}
