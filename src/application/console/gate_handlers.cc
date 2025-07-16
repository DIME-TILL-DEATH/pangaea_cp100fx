#include "gate_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void gate_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[ENABLE_GATE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_GATE, currentPreset.modules.rawData[ENABLE_GATE]);
}

static void gate_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[GATE_THRESHOLD], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_GATE, GATE_THRESHOLD_POS, currentPreset.modules.rawData[GATE_THRESHOLD]);
}

static void gate_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[GATE_ATTACK], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_GATE, GATE_ATTACK_POS, currentPreset.modules.rawData[GATE_ATTACK]);
}

static void gate_decay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[GATE_DECAY], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_GATE, GATE_DECAY_POS, currentPreset.modules.rawData[GATE_DECAY]);
}

void set_gate_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ng_on",  gate_on_command_handler);
	rl->AddCommandHandler("ng_th",  gate_threshold_command_handler);
	rl->AddCommandHandler("ng_at",  gate_attack_command_handler);
	rl->AddCommandHandler("ng_dc",  gate_decay_command_handler);
}
