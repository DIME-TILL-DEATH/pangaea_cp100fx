#include "eq_handlers.h"
#include "console_helpers.h"
#include "../modules.h"
#include "../BF706_send.h"

static void eq_on_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[eq], rl, args, count);
	DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EQ, currentPreset.modules.rawData[ENABLE_EQ]);
}

// LEGACY mode

static void eq_gain_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	 uint8_t pos_in_data_array = args[1][0] - 48 + eq1;

	 if (count == 2)
	 {
		 char hex[4] = {0,0,0,0};
		 i2hex(currentPreset.modules.rawData[pos_in_data_array], hex + 1);

		 if((int8_t) currentPreset.modules.rawData[pos_in_data_array] < 0) hex[0] = '-';
		 else hex[0] = ' ';

		 msg_console("%s\n", hex);
		 return;
	 }

	 if (count == 3)
	 {
		 char* end;
		 int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
	     currentPreset.modules.rawData[pos_in_data_array] = val;
	     //set data to DSP
	 }
}

static void eq_freq_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	 uint8_t pos_in_data_array = args[1][0] - 48 + f1;

	 if (count == 2)
	 {
		 char hex[4] = {0,0,0,0};
		 i2hex(currentPreset.modules.rawData[pos_in_data_array], hex + 1);

		 if((int8_t) currentPreset.modules.rawData[pos_in_data_array] < 0) hex[0] = '-';
		 else hex[0] = ' ';

		 msg_console("%s\n", hex);
		 return;
	 }

	 if (count == 3)
	 {
		 char* end;
		 int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
		 currentPreset.modules.rawData[pos_in_data_array] = val;
		 //set data to DSP
		 msg_console("\n");
	 }
}

static void eq_q_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	 uint8_t pos_in_data_array = args[1][0] - 48 + q1;

	 if (count == 2)
	 {
		 char hex[4] = {0,0,0,0};
		 i2hex(currentPreset.modules.rawData[pos_in_data_array], hex + 1);

		 if((int8_t) currentPreset.modules.rawData[pos_in_data_array] < 0) hex[0] = '-';
		 else hex[0] = ' ';

		 msg_console("%s\n", hex);
		 return;
	 }

	 if (count == 3)
	 {
		 char* end;
		 int32_t val = kgp_sdk_libc::strtol(args[2], &end, 16);
	     currentPreset.modules.rawData[pos_in_data_array] = val;
	     //set data to DSP
	 }
}

static void eq_position_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[eq_pr_po], rl, args, count);
}

static void lpf_freq_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[lpf_v], rl, args, count);
}

static void hpf_freq_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.modules.rawData[hpf_v], rl, args, count);
}

void set_eq_handlers(TReadLine *rl)
{
	rl->AddCommandHandler("eq_on",  eq_on_command_handler);
	rl->AddCommandHandler("eq_g",  eq_gain_command_handler);
	rl->AddCommandHandler("eq_f",  eq_freq_command_handler);
	rl->AddCommandHandler("eq_q",  eq_q_command_handler);
	rl->AddCommandHandler("eq_pp",  eq_position_command_handler);
	rl->AddCommandHandler("lp_f",  lpf_freq_command_handler);
	rl->AddCommandHandler("hp_f",  hpf_freq_command_handler);
}
