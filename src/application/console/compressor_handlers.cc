#include "preamp_handlers.h"

#include "console_helpers.h"

static void compressor_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[compr], rl, args, count);
}

static void compressor_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[comp_thr], rl, args, count);
}

static void compressor_ratio_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[comp_ra], rl, args, count);
}

static void compressor_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[comp_v], rl, args, count);
}

static void compressor_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[comp_at], rl, args, count);
}

static void compressor_knee_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[comp_kn], rl, args, count);
}


void set_compressor_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("cm_on",  compressor_on_command_handler);
	rl->AddCommandHandler("cm_th",  compressor_threshold_command_handler);
	rl->AddCommandHandler("cm_rt",  compressor_ratio_command_handler);
	rl->AddCommandHandler("cm_vl",  compressor_volume_command_handler);
	rl->AddCommandHandler("cm_at",  compressor_attack_command_handler);
	rl->AddCommandHandler("cm_kn",  compressor_knee_command_handler);
}


