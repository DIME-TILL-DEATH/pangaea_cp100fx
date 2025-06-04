#include "chorus_handlers.h"

#include "console_helpers.h"

static void chorus_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor], rl, args, count);
}

static void chorus_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor_volum], rl, args, count);
}

static void chorus_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor_rat], rl, args, count);
}

static void chorus_width_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor_width], rl, args, count);
}

static void chorus_delay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor_del], rl, args, count);
}

static void chorus_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor_typ], rl, args, count);
}

static void chorus_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[hpf_ch], rl, args, count);
}


void set_chorus_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ch_on",  chorus_on_command_handler);
	rl->AddCommandHandler("ch_mx",  chorus_mix_command_handler);
	rl->AddCommandHandler("ch_rt",  chorus_rate_command_handler);
	rl->AddCommandHandler("ch_wd",  chorus_width_command_handler);
	rl->AddCommandHandler("ch_dl",  chorus_delay_command_handler);
	rl->AddCommandHandler("ch_tp",  chorus_type_command_handler);
	rl->AddCommandHandler("ch_hp",  chorus_hpf_command_handler);
}



