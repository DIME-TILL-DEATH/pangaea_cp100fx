#include "amp_handlers.h"
#include "console_helpers.h"
#include "modules.h"
#include "BF706_send.h"

static void amp_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[amp], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_AMP, currentPreset.modulesBuf[ENABLE_AMP]);
}

static void amp_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[AMP_MASTER], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_AMP, AMP_MASTER_POS, currentPreset.modulesBuf[AMP_MASTER]);
}

static void amp_presence_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[EQ_PRESENCE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_PRESENCE_POS, currentPreset.modulesBuf[EQ_PRESENCE]);
}

static void amp_slave_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[AMP_LEVEL], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_AMP, AMP_LEVEL_POS, currentPreset.modulesBuf[AMP_LEVEL]);
}

static void amp_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[AMP_TYPE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_AMP_TYPE, currentPreset.modulesBuf[AMP_TYPE], 0);
}

void set_amp_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("pa_on",  amp_on_command_handler);
	rl->AddCommandHandler("pa_ms",  amp_volume_command_handler);	//master
	rl->AddCommandHandler("pa_ps",  amp_presence_command_handler);
	rl->AddCommandHandler("pa_lv",  amp_slave_command_handler);	//level
	rl->AddCommandHandler("pa_tp",  amp_type_command_handler);
}
