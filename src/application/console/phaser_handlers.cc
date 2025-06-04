#include "phaser_handlers.h"

#include "console_helpers.h"

static void phaser_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phas], rl, args, count);
}

static void phaser_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_vol], rl, args, count);
}

static void phaser_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_rate], rl, args, count);
}

static void phaser_width_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_wdt], rl, args, count);
}

static void phaser_center_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_cnt], rl, args, count);
}

static void phaser_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_feedback], rl, args, count);
}

static void phaser_stages_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phaser_type], rl, args, count);
}

static void phaser_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[hpf_ph], rl, args, count);
}

static void phaser_position_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phas_pos], rl, args, count);
}

void set_phaser_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ph_on",  phaser_on_command_handler);
	rl->AddCommandHandler("ph_mx",  phaser_mix_command_handler);
	rl->AddCommandHandler("ph_rt",  phaser_rate_command_handler);
	rl->AddCommandHandler("ph_cn",  phaser_center_command_handler);
	rl->AddCommandHandler("ph_wd",  phaser_width_command_handler);
	rl->AddCommandHandler("ph_fb",  phaser_feedback_command_handler);
	rl->AddCommandHandler("ph_st",  phaser_stages_command_handler);
	rl->AddCommandHandler("ph_hp",  phaser_hpf_command_handler);
	rl->AddCommandHandler("ph_pp",  phaser_position_command_handler);
}
