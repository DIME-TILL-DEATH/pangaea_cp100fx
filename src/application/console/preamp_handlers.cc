#include "preamp_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void preamp_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[ENABLE_PREAMP], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PREAMP, prog_data[ENABLE_PREAMP]);
}

static void preamp_gain_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PREAMP_GAIN], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS, prog_data[PREAMP_GAIN]);
}

static void preamp_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PREAMP_VOLUME], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS, prog_data[PREAMP_VOLUME]);
}

static void preamp_low_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_low], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS, prog_data[PREAMP_LOW]);
}

static void preamp_mid_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_mid], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_MID_POS, prog_data[PREAMP_MID]);
}

static void preamp_high_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_high], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS, prog_data[PREAMP_HIGH]);
}


void set_preamp_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("pr_on",  preamp_on_command_handler);
	rl->AddCommandHandler("pr_gn",  preamp_gain_command_handler);
	rl->AddCommandHandler("pr_vl",  preamp_volume_command_handler);
	rl->AddCommandHandler("pr_lo",  preamp_low_command_handler);
	rl->AddCommandHandler("pr_mi",  preamp_mid_command_handler);
	rl->AddCommandHandler("pr_hi",  preamp_high_command_handler);
}
