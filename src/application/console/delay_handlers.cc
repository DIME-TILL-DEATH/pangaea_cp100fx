#include "delay_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

#include "eepr.h"

static void delay_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[delay], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_DELAY, presetData[ENABLE_DELAY]);
}

static void delay_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_MIX], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_MIX_POS, presetData[DELAY_MIX]);
}

static void delay_volume2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_VOLUME2], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_VOLUME2_POS, presetData[DELAY_VOLUME2]);
}

static void delay_pan1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_PAN1], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_PAN1_POS, presetData[DELAY_PAN1]);
}

static void delay_pan2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_PAN2], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_PAN2_POS, presetData[DELAY_PAN2]);
}

static void delay_time_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[5] = {0, 0, 0, 0, 0};

	if(count > 0)
	{
		if (count == 2)
		{
			char *end;
			delay_time = kgp_sdk_libc::strtol(args[1], &end, 16);
			presetData[delay_tim_lo] = delay_time >> 8;
			presetData[delay_tim_hi] = delay_time & 0xFF;
		}

		hex[0] = delay_time >> 12;
		hex[1] = delay_time >> 8;
		hex[2] = delay_time >> 4;
		hex[3] = delay_time & 0xF;

		msg_console("%s %s\n", args[0], hex);
	}

	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, presetData[DELAY_TIME_LO]);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, presetData[DELAY_TIME_HI]);
}

static void delay_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_FEEDBACK], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS, presetData[DELAY_FEEDBACK]);
}

static void delay_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_LPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_LPF_POS, presetData[DELAY_LPF]);
}

static void delay_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_HPF], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_HPF_POS, presetData[DELAY_HPF]);
}

static void delay_offset_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_OFFSET], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_OFFSET_POS, presetData[DELAY_OFFSET]);
}

static void delay_modulation_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_MODULATION], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_MODULATION_POS, presetData[DELAY_MODULATION]);
}

static void delay_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_MODULATION_RATE], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_MODULATION_RATE_POS, presetData[DELAY_MODULATION_RATE]);
}

static void delay_direction_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_DIRECTION], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_DIRECTION_POS, presetData[DELAY_DIRECTION]);
}

static void delay_tap_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[d_tap_t], rl, args, count);
}

static void delay_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&presetData[DELAY_TAIL], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_TAIL_POS, presetData[DELAY_TAIL]);
}

void set_delay_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("dl_on",  delay_on_command_handler);
	rl->AddCommandHandler("dl_m1",  delay_mix_command_handler);
	rl->AddCommandHandler("dl_m2",  delay_volume2_command_handler);
	rl->AddCommandHandler("dl_p1",  delay_pan1_command_handler);
	rl->AddCommandHandler("dl_p2",  delay_pan2_command_handler);
	rl->AddCommandHandler("dl_tm",  delay_time_command_handler);
	rl->AddCommandHandler("dl_fb",  delay_feedback_command_handler);
	rl->AddCommandHandler("dl_hp",  delay_hpf_command_handler);
	rl->AddCommandHandler("dl_lp",  delay_lpf_command_handler);
	rl->AddCommandHandler("dl_os",  delay_offset_command_handler);
	rl->AddCommandHandler("dl_md",  delay_modulation_command_handler);
	rl->AddCommandHandler("dl_rt",  delay_rate_command_handler);
	rl->AddCommandHandler("dl_dr",  delay_direction_command_handler);
	rl->AddCommandHandler("dl_tp",  delay_tap_command_handler);
	rl->AddCommandHandler("dl_tl",  delay_tail_command_handler);

}
