#include "phaser_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void phaser_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[phas], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PHASER, prog_data[ENABLE_PHASER]);
}

static void phaser_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_MIX_POS, prog_data[PHASER_MIX]);
}

static void phaser_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_RATE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_RATE_POS, prog_data[PHASER_RATE]);
}

static void phaser_width_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_WIDTH], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_WIDTH_POS, prog_data[PHASER_WIDTH]);
}

static void phaser_center_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_CENTER], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_CENTER_POS, prog_data[PHASER_CENTER]);
}

static void phaser_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_FEEDBACK], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_FEEDBACK_POS, prog_data[PHASER_FEEDBACK]);
}

static void phaser_stages_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_TYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_TYPE_POS, prog_data[PHASER_TYPE]);
}

static void phaser_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_HPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_HPF_POS, prog_data[PHASER_HPF]);
}

static void phaser_position_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[PHASER_PREPOST], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_PREPOST_POS, prog_data[PHASER_PREPOST]);
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
