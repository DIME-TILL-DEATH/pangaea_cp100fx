#include "flanger_handlers.h"

#include "console_helpers.h"

static void flanger_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl], rl, args, count);
}

static void flanger_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_v], rl, args, count);
}

static void flanger_gen_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_lfo], rl, args, count);
}

static void flanger_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_r], rl, args, count);
}

static void flanger_width_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_w], rl, args, count);
}

static void flanger_delay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_d], rl, args, count);
}

static void flanger_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[fl_f], rl, args, count);
}

static void flanger_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[hpf_fl], rl, args, count);
}

static void flanger_position_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[flan_pos], rl, args, count);
}

void set_flanger_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("fl_on",  flanger_on_command_handler);
	rl->AddCommandHandler("fl_mx",  flanger_mix_command_handler);
	rl->AddCommandHandler("fl_lf",  flanger_gen_type_command_handler);
	rl->AddCommandHandler("fl_rt",  flanger_rate_command_handler);
	rl->AddCommandHandler("fl_wd",  flanger_width_command_handler);
	rl->AddCommandHandler("fl_dl",  flanger_delay_command_handler);
	rl->AddCommandHandler("fl_fb",  flanger_feedback_command_handler);
	rl->AddCommandHandler("fl_hp",  flanger_hpf_command_handler);
	rl->AddCommandHandler("fl_pp",  flanger_position_command_handler);
}
