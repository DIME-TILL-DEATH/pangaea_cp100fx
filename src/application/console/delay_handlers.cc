#include "delay_handlers.h"

#include "console_helpers.h"

#include "eepr.h"

static void delay_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[delay], rl, args, count);
}

static void delay_mix_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_vol], rl, args, count);
}

static void delay_volume2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[dp_vol], rl, args, count);
}

static void delay_pan1_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_pan], rl, args, count);
}

static void delay_pan2_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[dp_pan], rl, args, count);
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
			prog_data[delay_tim_lo] = delay_time >> 8;
			prog_data[delay_tim_hi] = delay_time & 0xFF;
		}

		hex[0] = delay_time >> 12;
		hex[1] = delay_time >> 8;
		hex[2] = delay_time >> 4;
		hex[3] = delay_time & 0xF;

		msg_console("%s %s\n", args[0], hex);
	}
}

static void delay_feedback_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_fed], rl, args, count);
}

static void delay_lpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_lp], rl, args, count);
}

static void delay_hpf_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_hp], rl, args, count);
}

static void delay_offset_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[dp_d], rl, args, count);
}

static void delay_modulation_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_mod], rl, args, count);
}

static void delay_rate_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_ra], rl, args, count);
}

static void delay_direction_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_dir], rl, args, count);
}

static void delay_tap_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_tap_t], rl, args, count);
}

static void delay_tail_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&prog_data[d_tail], rl, args, count);
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
