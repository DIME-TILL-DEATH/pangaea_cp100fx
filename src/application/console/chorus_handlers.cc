#include "chorus_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void chorus_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[chor], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CHORUS, prog_data[ENABLE_CHORUS]);
}

static void chorus_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_MIX_POS, prog_data[CHORUS_MIX]);
}

static void chorus_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_RATE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_RATE_POS, prog_data[CHORUS_RATE]);
}

static void chorus_width_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_WIDTH], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_WIDTH_POS, prog_data[CHORUS_WIDTH]);
}

static void chorus_delay_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_DELAY], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_DELAY_POS, prog_data[CHORUS_DELAY]);
}

static void chorus_type_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_TYPE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_TYPE_POS, prog_data[CHORUS_TYPE]);
}

static void chorus_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[CHORUS_HPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_HPF_POS, prog_data[CHORUS_HPF]);
}


void set_chorus_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("ch_on",  chorus_on_command_handler);
	rl->AddCommandHandler("ch_mx",  chorus_mix_command_handler);
	rl->AddCommandHandler("ch_rt",  chorus_rate_command_handler);
	rl->AddCommandHandler("ch_wd",  chorus_width_command_handler);
	rl->AddCommandHandler("ch_dl",  chorus_delay_command_handler);
	rl->AddCommandHandler("ch_tp",  chorus_type_command_handler);
	rl->AddCommandHandler("ch_hp",  chorus_hpf_command_handler);
}



