#include "tremolo_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void tremolo_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[ENABLE_TREMOLO], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_TREMOLO, presetData[ENABLE_TREMOLO]);
}

static void tremolo_intensity_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_INTENSITY], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS, presetData[TREMOLO_INTENSITY]);
}

static void tremolo_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_RATE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS, presetData[TREMOLO_RATE]);
}

static void tremolo_from_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_LFO_TYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_TYPE_POS, presetData[TREMOLO_LFO_TYPE]);
}

static void tremolo_lfomod_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_LFO_MOD], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_LFO_MOD_POS, presetData[TREMOLO_LFO_MOD]);
}

static void tremolo_ms_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_MS], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_MS_POS, presetData[TREMOLO_MS]);
}

static void tremolo_tap_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[TREMOLO_TAP], rl, args, count);
}

void set_tremolo_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("tr_on",  tremolo_on_command_handler);
	rl->AddCommandHandler("tr_dp",  tremolo_intensity_command_handler);
	rl->AddCommandHandler("tr_rt",  tremolo_rate_command_handler);
	rl->AddCommandHandler("tr_fm",  tremolo_from_command_handler);
	rl->AddCommandHandler("tr_lm",  tremolo_lfomod_command_handler);
	rl->AddCommandHandler("tr_ms",  tremolo_ms_command_handler);
	rl->AddCommandHandler("tr_tp",  tremolo_tap_command_handler);
}
