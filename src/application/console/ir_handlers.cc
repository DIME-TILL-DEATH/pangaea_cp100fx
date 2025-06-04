#include "ir_handlers.h"

#include "console_helpers.h"

static void ir_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab], rl, args, count);
}

static void ir_volume1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[vol], rl, args, count);
}

static void ir_pan1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab1_pan], rl, args, count);
}

static void ir_delay1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab1_del], rl, args, count);
}

static void ir_volume2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab2_vol], rl, args, count);
}

static void ir_pan2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab2_pan], rl, args, count);
}

static void ir_delay2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[cab2_del], rl, args, count);
}
void set_ir_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ir_on",  ir_on_command_handler);
	rl->AddCommandHandler("ir_v1",  ir_volume1_command_handler);
	rl->AddCommandHandler("ir_p1",  ir_pan1_command_handler);
	rl->AddCommandHandler("ir_d1",  ir_delay1_command_handler);
	rl->AddCommandHandler("ir_v2",  ir_volume2_command_handler);
	rl->AddCommandHandler("ir_p2",  ir_pan2_command_handler);
	rl->AddCommandHandler("ir_d2",  ir_delay2_command_handler);
}
