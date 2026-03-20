#include "compressor_handlers.h"
#include "console_helpers.h"
#include "modules.h"
#include "sharc.h"

static void compressor_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[ENABLE_COMPRESSOR], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_COMPRESSOR, currentPreset.modulesBuf[ENABLE_COMPRESSOR]);
}

static void compressor_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[COMPRESSOR_THRESHOLD], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_THRESHOLD_POS, currentPreset.modulesBuf[COMPRESSOR_THRESHOLD]);
}

static void compressor_ratio_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[COMPRESSOR_RATIO], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_RATIO_POS, currentPreset.modulesBuf[COMPRESSOR_RATIO]);
}

static void compressor_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[COMPRESSOR_VOLUME], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_VOLUME_POS, currentPreset.modulesBuf[COMPRESSOR_VOLUME]);
}

static void compressor_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[COMPRESSOR_ATTACK], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_ATTACK_POS, currentPreset.modulesBuf[COMPRESSOR_ATTACK]);
}

static void compressor_release_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[COMPRESSOR_RELEASE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_RELEASE_POS, currentPreset.modulesBuf[COMPRESSOR_RELEASE]);
}


void set_compressor_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("cm_on",  compressor_on_command_handler);
	rl->AddCommandHandler("cm_th",  compressor_threshold_command_handler);
	rl->AddCommandHandler("cm_rt",  compressor_ratio_command_handler);
	rl->AddCommandHandler("cm_vl",  compressor_volume_command_handler);
	rl->AddCommandHandler("cm_at",  compressor_attack_command_handler);
	rl->AddCommandHandler("cm_rl",  compressor_release_command_handler);
}


