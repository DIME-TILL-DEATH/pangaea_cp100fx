#include "resonance_filter_handlers.h"
#include "console_helpers.h"

#include "modules.h"

#include "sharc_task.h"

static void rf_on_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[ENABLE_RESONANCE_FILTER], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_RESONANCE_FILTER, currentPreset.modulesBuf[ENABLE_RESONANCE_FILTER]);
}

static void rf_mix_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_MIX], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_MIX_POS, currentPreset.modulesBuf[RFILTER_MIX]);
}

static void rf_filter_type_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_FTYPE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_FTYPE_POS, currentPreset.modulesBuf[RFILTER_FTYPE]);
}

static void rf_fmod_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_FMOD], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_FMOD_POS, currentPreset.modulesBuf[RFILTER_FMOD]);
}

static void rf_rate_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_RATE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RATE_POS, currentPreset.modulesBuf[RFILTER_RATE]);
}

static void rf_lpf_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_LPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_LPF_POS, currentPreset.modulesBuf[RFILTER_LPF]);
}

static void rf_hpf_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_HPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_HPF_POS, currentPreset.modulesBuf[RFILTER_HPF]);
}

static void rf_resonance_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_RESONANCE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RESONANCE_POS, currentPreset.modulesBuf[RFILTER_RESONANCE]);
}

static void rf_dyn_threshold_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_DYN_THRESHOLD], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_DYN_THRESHOLD_POS, currentPreset.modulesBuf[RFILTER_DYN_THRESHOLD]);
}

static void rf_dyn_attack_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_DYN_ATTACK], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_DYN_ATTACK_POS, currentPreset.modulesBuf[RFILTER_DYN_ATTACK]);
}

static void rf_dyn_release_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFLITER_DYN_RELEASE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFLITER_DYN_RELEASE_POS, currentPreset.modulesBuf[RFLITER_DYN_RELEASE]);
}

static void rf_volume_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_VOLUME], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_VOLUME_POS, currentPreset.modulesBuf[RFILTER_VOLUME]);
}

static void rf_lfo_type_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[RFILTER_LFO_TYPE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_LFO_TYPE_POS, currentPreset.modulesBuf[RFILTER_LFO_TYPE]);
}

void set_resonance_filter_handlers(TTranslator *rl)
{
	rl->AddCommandHandler("rf_on",  rf_on_command_handler);
	rl->AddCommandHandler("rf_mx",  rf_mix_command_handler);
	rl->AddCommandHandler("rf_ft",  rf_filter_type_command_handler);
	rl->AddCommandHandler("rf_fm",  rf_fmod_command_handler);
	rl->AddCommandHandler("rf_lr",  rf_rate_command_handler);
	rl->AddCommandHandler("rf_lp",  rf_lpf_command_handler);
	rl->AddCommandHandler("rf_hp",  rf_hpf_command_handler);
	rl->AddCommandHandler("rf_rs",  rf_resonance_command_handler);
	rl->AddCommandHandler("rf_dt",  rf_dyn_threshold_command_handler);
	rl->AddCommandHandler("rf_da",  rf_dyn_attack_command_handler);
	rl->AddCommandHandler("rf_dr",  rf_dyn_release_command_handler);
	rl->AddCommandHandler("rf_vl",  rf_volume_command_handler);
	rl->AddCommandHandler("rf_lt",  rf_lfo_type_command_handler);
}
