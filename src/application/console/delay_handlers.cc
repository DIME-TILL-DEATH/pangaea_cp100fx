#include "delay_handlers.h"
#include "console_helpers.h"

#include "modules.h"

#include "sharc_task.h"

static void delay_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[delay], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_DELAY, currentPreset.modulesBuf[ENABLE_DELAY]);
}

static void delay_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_MIX], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_MIX_POS, currentPreset.modulesBuf[DELAY_MIX]);
}

static void delay_volume2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_VOLUME2], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_VOLUME2_POS, currentPreset.modulesBuf[DELAY_VOLUME2]);
}

static void delay_pan1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_PAN1], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_PAN1_POS, currentPreset.modulesBuf[DELAY_PAN1]);
}

static void delay_pan2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_PAN2], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_PAN2_POS, currentPreset.modulesBuf[DELAY_PAN2]);
}

static void delay_time_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[5] = {0, 0, 0, 0, 0};

	if(count > 0)
	{
		if (count == 2)
		{
			char *end;
			currentPreset.delayTime = kgp_sdk_libc::strtol(args[1], &end, 16);
			currentPreset.modulesBuf[delay_tim_lo] = currentPreset.delayTime >> 8;
			currentPreset.modulesBuf[delay_tim_hi] = currentPreset.delayTime & 0xFF;
		}

		hex[0] = currentPreset.delayTime >> 12;
		hex[1] = currentPreset.delayTime >> 8;
		hex[2] = currentPreset.delayTime >> 4;
		hex[3] = currentPreset.delayTime & 0xF;

		msg_console("%s\r%s\n", args[0], hex);
	}

	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, currentPreset.modulesBuf[DELAY_TIME_LO]);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, currentPreset.modulesBuf[DELAY_TIME_HI]);
}

static void delay_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_FEEDBACK], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS, currentPreset.modulesBuf[DELAY_FEEDBACK]);
}

static void delay_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_LPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_LPF_POS, currentPreset.modulesBuf[DELAY_LPF]);
}

static void delay_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_HPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_HPF_POS, currentPreset.modulesBuf[DELAY_HPF]);
}

static void delay_offset_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_OFFSET], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_OFFSET_POS, currentPreset.modulesBuf[DELAY_OFFSET]);
}

static void delay_modulation_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_MODULATION], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_MODULATION_POS, currentPreset.modulesBuf[DELAY_MODULATION]);
}

static void delay_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_MODULATION_RATE], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_MODULATION_RATE_POS, currentPreset.modulesBuf[DELAY_MODULATION_RATE]);
}

static void delay_direction_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_DIRECTION], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_DIRECTION_POS, currentPreset.modulesBuf[DELAY_DIRECTION]);
}

static void delay_tap_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[d_tap_t], rl, args, count);
}

static void delay_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[DELAY_TAIL], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TAIL_POS, currentPreset.modulesBuf[DELAY_TAIL]);
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
