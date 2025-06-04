#include "amp_handlers.h"

#include "console_helpers.h"

static void amp_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[amp], rl, args, count);
}

static void amp_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[am_v], rl, args, count);
}

static void amp_presence_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[pre_v], rl, args, count);
}

static void amp_slave_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[am_out], rl, args, count);
}

static void amp_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[am_mode], rl, args, count);
}


void set_amp_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("pa_on",  amp_on_command_handler);
	rl->AddCommandHandler("pa_ms",  amp_volume_command_handler);	//master
	rl->AddCommandHandler("pa_ps",  amp_presence_command_handler);
	rl->AddCommandHandler("pa_lv",  amp_slave_command_handler);	//level
	rl->AddCommandHandler("pa_tp",  amp_type_command_handler);
}
