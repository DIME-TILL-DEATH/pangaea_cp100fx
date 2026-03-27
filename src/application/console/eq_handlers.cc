#include "eq_handlers.h"

#include "console_helpers.h"
#include "modules.h"

#include "sharc_task.h"

static void eq_on_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[eq], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EQ, currentPreset.modulesBuf[ENABLE_EQ]);
}

// LEGACY mode

static void eq_gain_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	uint8_t bandNum = args[1][0] - 48;
	uint8_t pos_in_data_array =  EQ_G0 + bandNum; //eq1;

	if (count == 2)
	{
		char hex[4] = {0,0,0,0};
		i2hex(currentPreset.modulesBuf[pos_in_data_array], hex + 1);

		if((int8_t) currentPreset.modulesBuf[pos_in_data_array] < 0) hex[0] = '-';
		else hex[0] = ' ';

		msg_console("%s\n", hex);
		return;
	}

	if (count == 3)
	{
		char* end;
		int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
		currentPreset.modulesBuf[pos_in_data_array] = val;
		SharcTask->setParameter(DSP_ADDRESS_EQ, bandNum, val);
	}
}

static void eq_freq_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	uint8_t bandNum = args[1][0] - 48;
	uint8_t pos_in_data_array = EQ_F0 + bandNum;//f1;

	if (count == 2)
	{
		char hex[4] = {0,0,0,0};
		i2hex(currentPreset.modulesBuf[pos_in_data_array], hex + 1);

		if((int8_t) currentPreset.modulesBuf[pos_in_data_array] < 0) hex[0] = '-';
		else hex[0] = ' ';

		msg_console("%s\n", hex);
		return;
	}

	if (count == 3)
	{
		char* end;
		int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
		currentPreset.modulesBuf[pos_in_data_array] = val;

		SharcTask->setParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + bandNum, val);

		msg_console("\n");
	}
}

static void eq_q_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	uint8_t bandNum = args[1][0] - 48;
	uint8_t pos_in_data_array = EQ_Q0 + bandNum;//q1;

	if (count == 2)
	{
		char hex[4] = {0,0,0,0};
		i2hex(currentPreset.modulesBuf[pos_in_data_array], hex + 1);

		if((int8_t) currentPreset.modulesBuf[pos_in_data_array] < 0) hex[0] = '-';
		else hex[0] = ' ';

		msg_console("%s\n", hex);
		return;
	}

	if (count == 3)
	{
		char* end;
		int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
		currentPreset.modulesBuf[pos_in_data_array] = val;

		SharcTask->setParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + bandNum, val);
	}
}

static void eq_position_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[EQ_PREPOST], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_PREPOST_POS, currentPreset.modulesBuf[EQ_PREPOST]);
}

static void lpf_freq_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[EQ_LPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_LPF_POS, currentPreset.modulesBuf[EQ_LPF]);
}

static void hpf_freq_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modulesBuf[EQ_HPF], rl, args, count);
	SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_HPF_POS, currentPreset.modulesBuf[EQ_HPF]);
}

void set_eq_handlers(TTranslator *rl)
{
	rl->AddCommandHandler("eq_on",  eq_on_command_handler);
	rl->AddCommandHandler("eq_g",  eq_gain_command_handler);
	rl->AddCommandHandler("eq_f",  eq_freq_command_handler);
	rl->AddCommandHandler("eq_q",  eq_q_command_handler);
	rl->AddCommandHandler("eq_pp",  eq_position_command_handler);
	rl->AddCommandHandler("lp_f",  lpf_freq_command_handler);
	rl->AddCommandHandler("hp_f",  hpf_freq_command_handler);
}
