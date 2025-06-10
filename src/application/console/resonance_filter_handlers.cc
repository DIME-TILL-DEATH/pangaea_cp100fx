#include "resonance_filter_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void rf_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[moog], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_RESONANCE_FILTER, prog_data[ENABLE_RESONANCE_FILTER]);
}

static void rf_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_MIX_POS, prog_data[RFILTER_MIX]);
}

static void rf_filter_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_FTYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_FTYPE_POS, prog_data[RFILTER_FTYPE]);
}

static void rf_fmod_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_FMOD], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_FMOD_POS, prog_data[RFILTER_FMOD]);
}

static void rf_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_RATE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RATE_POS, prog_data[RFILTER_RATE]);
}

static void rf_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_LPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_LPF_POS, prog_data[RFILTER_LPF]);
}

static void rf_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_HPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_HPF_POS, prog_data[RFILTER_HPF]);
}

static void rf_resonance_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_RESONANCE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RESONANCE_POS, prog_data[RFILTER_RESONANCE]);
}

static void rf_dyn_threshold_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_DYN_THRESHOLD], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_DYN_THRESHOLD_POS, prog_data[RFILTER_DYN_THRESHOLD]);
}

static void rf_dyn_attack_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_DYN_ATTACK], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_DYN_ATTACK_POS, prog_data[RFILTER_DYN_ATTACK]);
}

static void rf_dyn_release_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFLITER_DYN_RELEASE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFLITER_DYN_RELEASE_POS, prog_data[RFLITER_DYN_RELEASE]);
}

static void rf_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_VOLUME], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_VOLUME_POS, prog_data[RFILTER_VOLUME]);
}

static void rf_lfo_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[RFILTER_LFO_TYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_LFO_TYPE_POS, prog_data[RFILTER_LFO_TYPE]);
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
