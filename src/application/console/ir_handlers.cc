#include "ir_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void ir_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[cab], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CAB, currentPreset.modules.rawData[ENABLE_CAB]);
}

static void ir_volume1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_VOLUME1], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]);
}

static void ir_pan1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_PAN1], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_PAN1_POS, currentPreset.modules.rawData[IR_PAN1]);
}

static void ir_delay1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_DELAY1], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_DELAY1_POS, currentPreset.modules.rawData[IR_DELAY1]);
}

static void ir_volume2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_VOLUME2], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]);
}

static void ir_pan2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_PAN2], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_PAN2_POS, currentPreset.modules.rawData[IR_PAN2]);
}

static void ir_delay2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[IR_DELAY2], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_DELAY2_POS, currentPreset.modules.rawData[IR_DELAY2]);
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
