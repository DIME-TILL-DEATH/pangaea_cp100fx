#include "early_handlers.h"

#include "console_helpers.h"

static void early_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[early], rl, args, count);
}

static void early_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[early_vol], rl, args, count);
}

static void early_size_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[early_type], rl, args, count);
}

void set_early_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("er_on",  early_on_command_handler);
	rl->AddCommandHandler("er_mx",  early_mix_command_handler);
	rl->AddCommandHandler("er_sz",  early_size_command_handler);
}
