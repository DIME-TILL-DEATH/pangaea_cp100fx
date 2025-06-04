#include "resonance_filter_handlers.h"

#include "console_helpers.h"

static void rf_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[moog], rl, args, count);
}

static void rf_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_mix], rl, args, count);
}

static void rf_filter_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_ftype], rl, args, count);
}

static void rf_fmod_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_fmod], rl, args, count);
}

static void rf_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_rate], rl, args, count);
}

static void rf_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_lo], rl, args, count);
}

static void rf_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_hi], rl, args, count);
}

static void rf_resonance_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_res], rl, args, count);
}

static void rf_dyn_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_th], rl, args, count);
}

static void rf_dyn_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_at], rl, args, count);
}

static void rf_dyn_release_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_re], rl, args, count);
}

static void rf_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_vo], rl, args, count);
}

static void rf_lfo_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[mog_gen_t], rl, args, count);
}

void set_resonance_filter_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("rf_on",  rf_on_command_handler);
	rl->AddCommandHandler("rf_mx",  rf_mix_command_handler);
	rl->AddCommandHandler("rf_ft",  rf_filter_type_command_handler);
	rl->AddCommandHandler("rf_fm",  rf_fmod_command_handler);
	rl->AddCommandHandler("rf_rt",  rf_rate_command_handler);
	rl->AddCommandHandler("rf_lp",  rf_lpf_command_handler);
	rl->AddCommandHandler("rf_hp",  rf_hpf_command_handler);
	rl->AddCommandHandler("rf_rs",  rf_resonance_command_handler);
	rl->AddCommandHandler("rf_dt",  rf_dyn_threshold_command_handler);
	rl->AddCommandHandler("rf_da",  rf_dyn_attack_command_handler);
	rl->AddCommandHandler("rf_dr",  rf_dyn_release_command_handler);
	rl->AddCommandHandler("rf_vl",  rf_volume_command_handler);
	rl->AddCommandHandler("rf_lt",  rf_lfo_type_command_handler);
}
