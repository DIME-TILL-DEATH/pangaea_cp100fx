#include "syssettings_handlers.h"

#include "console_helpers.h"
#include "modules.h"
#include "BF706_send.h"

#include "display.h"

#include "eepr.h"

static void sysset_command_handler(TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	sys_para[MASTER_EQ_FREQ_LO] = (mstEqMidFreq>>8) & 0xF;
	sys_para[MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xF;

	for(size_t i = 0; i < 512; i++)
	{
		char hex[3] =
		{0, 0, 0};
		i2hex(sys_para[i], hex);
		msg_console("%s" , hex );
	}
	msg_console("\n");
}

static void attenuator_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[ATTENUATOR], rl, args, count);
	DisplayTask->Pot_Write();
}

static void master_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[MASTER_VOLUME], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MASTER, sys_para[MASTER_VOLUME], 0);
}

static void phones_volume_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[PHONES_VOLUME], rl, args, count);
	DisplayTask->Pot_Write();
}

static void meq_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[MASTER_EQ_ON], rl, args, count);
	DSP_gui_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[MASTER_EQ_ON]);
}

static void meq_low_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[MASTER_EQ_LOW], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[MASTER_EQ_LOW]);
}

static void meq_mid_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[MASTER_EQ_MID], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[MASTER_EQ_MID]);
}

static void meq_high_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&sys_para[MASTER_EQ_HIGH], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MASTER, EQ_MASTER_LOW_GAIN_POS, sys_para[MASTER_EQ_HIGH]);
}

static void meq_mid_freq_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	if(count > 0)
	{
		if (count == 2)
		{
			char *end;
			mstEqMidFreq = kgp_sdk_libc::strtol(args[1], &end, 16);
		}

		msg_console("%s\n", args[1]);
	}

	sys_para[MASTER_EQ_FREQ_LO] = mstEqMidFreq >> 8;
	sys_para[MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xFF;

	DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[MASTER_EQ_FREQ_LO]);
	DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[MASTER_EQ_FREQ_HI]);
}

void set_syssettings_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("sysset", sysset_command_handler);

	rl->AddCommandHandler("vl_at", attenuator_command_handler);
	rl->AddCommandHandler("vl_ms", master_volume_command_handler);
	rl->AddCommandHandler("vl_ph", phones_volume_command_handler);

	rl->AddCommandHandler("meq_on", meq_on_command_handler);
	rl->AddCommandHandler("meq_lg", meq_low_command_handler);
	rl->AddCommandHandler("meq_mg", meq_mid_command_handler);
	rl->AddCommandHandler("meq_hg", meq_high_command_handler);
	rl->AddCommandHandler("meq_mf", meq_mid_freq_command_handler);
}
