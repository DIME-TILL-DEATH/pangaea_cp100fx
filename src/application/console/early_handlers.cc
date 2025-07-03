#include "early_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void early_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EARLY_REFLECTIONS, currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS]);
}

static void early_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[EARLY_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_EARLY_REFLECTIONS, EARLY_MIX_POS, currentPreset.modules.rawData[EARLY_MIX]);
}

static void early_size_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[EARLY_SIZE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_EARLY_REFLECTIONS, EARLY_SIZE_POS, currentPreset.modules.rawData[EARLY_SIZE]);
}

void set_early_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("er_on",  early_on_command_handler);
	rl->AddCommandHandler("er_mx",  early_mix_command_handler);
	rl->AddCommandHandler("er_sz",  early_size_command_handler);
}
