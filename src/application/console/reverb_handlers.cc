#include "reverb_handlers.h"

#include "console_helpers.h"

static void reverb_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[reve], rl, args, count);
}

static void reverb_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_vol], rl, args, count);
}

static void reverb_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[rev_t], rl, args, count);
}

static void reverb_time_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_time], rl, args, count);
}

static void reverb_size_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_size], rl, args, count);
}

static void reverb_damping_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_dump], rl, args, count);
}

static void reverb_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_hp], rl, args, count);
}

static void reverb_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_lp], rl, args, count);
}

static void reverb_detune_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_det], rl, args, count);
}

static void reverb_diffusion_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[rev_di], rl, args, count);
}

static void reverb_predelay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_pre], rl, args, count);
}

static void reverb_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[r_tail], rl, args, count);
}

void set_reverb_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("rv_on",  reverb_on_command_handler);
	rl->AddCommandHandler("rv_mx",  reverb_mix_command_handler);
	rl->AddCommandHandler("rv_tp",  reverb_type_command_handler);
	rl->AddCommandHandler("rv_tm",  reverb_time_command_handler);
	rl->AddCommandHandler("rv_sz",  reverb_size_command_handler);
	rl->AddCommandHandler("rv_dp",  reverb_damping_command_handler);
	rl->AddCommandHandler("rv_hp",  reverb_hpf_command_handler);
	rl->AddCommandHandler("rv_lp",  reverb_lpf_command_handler);
	rl->AddCommandHandler("rv_dt",  reverb_detune_command_handler);
	rl->AddCommandHandler("rv_df",  reverb_diffusion_command_handler);
	rl->AddCommandHandler("rv_pd",  reverb_predelay_command_handler);
	rl->AddCommandHandler("rv_tl",  reverb_tail_command_handler);
}
