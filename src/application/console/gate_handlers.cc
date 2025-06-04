#include "gate_handlers.h"

#include "console_helpers.h"

static void gate_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[gate], rl, args, count);
}

static void gate_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[gate_thr], rl, args, count);
}

static void gate_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[gate_att], rl, args, count);
}

static void gate_decay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[gate_dec], rl, args, count);
}

void set_gate_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ng_on",  gate_on_command_handler);
	rl->AddCommandHandler("ng_th",  gate_threshold_command_handler);
	rl->AddCommandHandler("ng_at",  gate_attack_command_handler);
	rl->AddCommandHandler("ng_dc",  gate_decay_command_handler);
}
