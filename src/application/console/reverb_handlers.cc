#include "reverb_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void reverb_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[reve], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_REVERB, presetData[ENABLE_REVERB]);
}

static void reverb_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_MIX_POS, presetData[REVERB_MIX]);
}

static void reverb_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_TYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_TYPE_POS, presetData[REVERB_TYPE]);
}

static void reverb_time_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_TIME], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_TIME_POS, presetData[REVERB_TIME]);
}

static void reverb_size_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_SIZE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_SIZE_POS, presetData[REVERB_SIZE]);
}

static void reverb_damping_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_DAMPING], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_DAMPING_POS, presetData[REVERB_DAMPING]);
}

static void reverb_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_HPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_HPF_POS, presetData[REVERB_HPF]);
}

static void reverb_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_LPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_LPF_POS, presetData[REVERB_LPF]);
}

static void reverb_detune_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_DETUNE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_DETUNE_POS, presetData[REVERB_DETUNE]);
}

static void reverb_diffusion_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_DIFFUSION], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_DIFFUSION_POS, presetData[REVERB_DIFFUSION]);
}

static void reverb_predelay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_PREDELAY], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_PREDELAY_POS, presetData[REVERB_PREDELAY]);
}

static void reverb_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[REVERB_TAIL], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_TAIL_POS, presetData[REVERB_TAIL]);
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
