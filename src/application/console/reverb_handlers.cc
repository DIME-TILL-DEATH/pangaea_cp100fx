#include "reverb_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void reverb_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[reve], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_REVERB, currentPreset.modules.rawData[ENABLE_REVERB]);
}

static void reverb_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_MIX], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_MIX_POS, currentPreset.modules.rawData[REVERB_MIX]);
}

static void reverb_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_TYPE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_TYPE_POS, currentPreset.modules.rawData[REVERB_TYPE]);
}

static void reverb_time_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_TIME], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_TIME_POS, currentPreset.modules.rawData[REVERB_TIME]);
}

static void reverb_size_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_SIZE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_SIZE_POS, currentPreset.modules.rawData[REVERB_SIZE]);
}

static void reverb_damping_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_DAMPING], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_DAMPING_POS, currentPreset.modules.rawData[REVERB_DAMPING]);
}

static void reverb_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_HPF], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_HPF_POS, currentPreset.modules.rawData[REVERB_HPF]);
}

static void reverb_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_LPF], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_LPF_POS, currentPreset.modules.rawData[REVERB_LPF]);
}

static void reverb_detune_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_DETUNE], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_DETUNE_POS, currentPreset.modules.rawData[REVERB_DETUNE]);
}

static void reverb_diffusion_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_DIFFUSION], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_DIFFUSION_POS, currentPreset.modules.rawData[REVERB_DIFFUSION]);
}

static void reverb_predelay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_PREDELAY], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_PREDELAY_POS, currentPreset.modules.rawData[REVERB_PREDELAY]);
}

static void reverb_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[REVERB_TAIL], rl, args, count);
	DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_TAIL_POS, currentPreset.modules.rawData[REVERB_TAIL]);
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
