#include "menu_gate.h"

#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

const uint8_t gate_param_strings[][7] ={"Thresh", "Attack", "Decay"};

uint8_t* gate_param_ptrs[] = {
	&prog_data[GATE_THRESHOLD],
	&prog_data[GATE_ATTACK],
	&prog_data[GATE_DECAY],
};

void GUI_gate_select()
{
	DisplayTask->Clear();
	DisplayTask->Icon_Strel(14, 0);
	for(uint8_t i = 0; i < 3; i++)
	{
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem , 0, (uint8_t*)&gate_param_strings[i]);
		DisplayTask->ParamIndic(53, i, *gate_param_ptrs[i]);
	}
	par_num = 0;
}

void GUI_gate_menu_task()
{
	if(!encoder_knob_selected)
	{
		if(tim5_fl == 1) DisplayTask->StringOut(6, par_num, TDisplayTask::fntSystem, 2, (uint8_t*)&gate_param_strings[par_num]);
		else DisplayTask->StringOut(6, par_num, TDisplayTask::fntSystem, 0, (uint8_t*)&gate_param_strings[par_num]);
	}

	if(encoder_state_updated)
	{
		if(encoder_state == ENC_COUNTERCLOCKWISE_STEP)
		{
			if(!encoder_knob_selected)
			{
				if(par_num > 0)
				{
					DisplayTask->StringOut(6, par_num,TDisplayTask::fntSystem, 0, (uint8_t*)&gate_param_strings[par_num--]);
					DisplayTask->StringOut(6, par_num,TDisplayTask::fntSystem, 2, (uint8_t*)&gate_param_strings[par_num]);
					tim5_start(0);
				}
			}
			else
			{
				GUI_enc_param_decrase(gate_param_ptrs, DSP_ADDRESS_GATE);
			}
		}

		if(encoder_state == ENC_CLOCKWISE_STEP)
		{
			if(encoder_knob_selected == 0)
			{
				if(par_num < 2)
				{
					DisplayTask->StringOut(6, par_num, TDisplayTask::fntSystem, 0, (uint8_t*)&gate_param_strings[par_num++]);
					DisplayTask->StringOut(6, par_num, TDisplayTask::fntSystem, 2, (uint8_t*)&gate_param_strings[par_num]);
					tim5_start(0);
				}
			}
			else
			{
				GUI_enc_param_incrase(gate_param_ptrs, DSP_ADDRESS_GATE);
			}
		}
		clean_flag();
	}

	GUI_param_select_task((uint8_t**)gate_param_strings);

	if(k_up == 1)
	{
		GUI_return_to_main_menu();
	}
}
