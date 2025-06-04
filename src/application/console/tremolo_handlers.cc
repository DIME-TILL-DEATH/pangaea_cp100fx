#include "tremolo_handlers.h"

#include "console_helpers.h"

static void tremolo_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[trem], rl, args, count);
}

static void tremolo_intensity_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[tr_vol], rl, args, count);
}

static void tremolo_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[tr_rate], rl, args, count);
}

static void tremolo_from_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[tr_lfo_t], rl, args, count);
}

static void tremolo_lfomod_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[tr_lfo], rl, args, count);
}

static void tremolo_ms_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[tr_m_s], rl, args, count);
}

static void tremolo_tap_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[t_tap_t], rl, args, count);
}

void set_tremolo_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("tr_on",  tremolo_on_command_handler);
	rl->AddCommandHandler("tr_dp",  tremolo_intensity_command_handler);
	rl->AddCommandHandler("tr_rt",  tremolo_rate_command_handler);
	rl->AddCommandHandler("tr_fm",  tremolo_from_command_handler);
	rl->AddCommandHandler("tr_lm",  tremolo_lfomod_command_handler);
	rl->AddCommandHandler("tr_ms",  tremolo_ms_command_handler);
	rl->AddCommandHandler("tr_tp",  tremolo_tap_command_handler);
}
