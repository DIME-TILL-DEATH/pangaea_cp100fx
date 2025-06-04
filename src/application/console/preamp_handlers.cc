#include "preamp_handlers.h"

#include "console_helpers.h"

static void preamp_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pream], rl, args, count);
}

static void preamp_gain_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_gain], rl, args, count);
}

static void preamp_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_volume], rl, args, count);
}

static void preamp_low_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_low], rl, args, count);
}

static void preamp_mid_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_mid], rl, args, count);
}

static void preamp_high_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_high], rl, args, count);
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
