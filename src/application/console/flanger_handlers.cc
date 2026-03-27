#include "flanger_handlers.h"
#include "console_helpers.h"

#include "modules.h"

#include "sharc_task.h"

static void flanger_on_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[ENABLE_FLANGER], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_FLANGER, currentPreset.modulesBuf[ENABLE_FLANGER]);
}

static void flanger_mix_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_MIX], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_MIX_POS, currentPreset.modulesBuf[FLANGER_MIX]);
}

static void flanger_gen_type_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_LFO], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_LFO_POS, currentPreset.modulesBuf[FLANGER_LFO]);
}

static void flanger_rate_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_RATE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_RATE_POS, currentPreset.modulesBuf[FLANGER_RATE]);
}

static void flanger_width_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_WIDTH], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_WIDTH_POS, currentPreset.modulesBuf[FLANGER_WIDTH]);
}

static void flanger_delay_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_DELAY], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_DELAY_POS, currentPreset.modulesBuf[FLANGER_DELAY]);
}

static void flanger_feedback_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_FEEDBACK], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_FEEDBACK_POS, currentPreset.modulesBuf[FLANGER_FEEDBACK]);
}

static void flanger_hpf_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_HPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_HPF_POS, currentPreset.modulesBuf[FLANGER_HPF]);
}

static void flanger_position_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[FLANGER_PREPOST], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_PREPOST_POS, currentPreset.modulesBuf[FLANGER_PREPOST]);
}

void set_flanger_handlers(TTranslator *rl)
{
	rl->AddCommandHandler("fl_on",  flanger_on_command_handler);
	rl->AddCommandHandler("fl_mx",  flanger_mix_command_handler);
	rl->AddCommandHandler("fl_lf",  flanger_gen_type_command_handler);
	rl->AddCommandHandler("fl_rt",  flanger_rate_command_handler);
	rl->AddCommandHandler("fl_wd",  flanger_width_command_handler);
	rl->AddCommandHandler("fl_dl",  flanger_delay_command_handler);
	rl->AddCommandHandler("fl_fb",  flanger_feedback_command_handler);
	rl->AddCommandHandler("fl_hp",  flanger_hpf_command_handler);
	rl->AddCommandHandler("fl_pp",  flanger_position_command_handler);
}
