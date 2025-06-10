#include "menu_preamp.h"

#include "gui_task.h"

#include "../modules.h"

#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

const uint8_t preamp_param_strings[][7] = {"Gain", "Volume", " ", "Low", "Mid", "High"};

uint8_t* preamp_param_ptrs[] = {
	&prog_data[PREAMP_GAIN],
	&prog_data[PREAMP_VOLUME],
	&prog_data[PREAMP_0],
	&prog_data[PREAMP_LOW],
	&prog_data[PREAMP_MID],
	&prog_data[PREAMP_HIGH]
};

void preamp_show_first_page()
{
	DisplayTask->Clear();
	DisplayTask->Icon_Strel(12, 2);

	for(uint8_t i = 0; i < 2; i++)
	{
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem, 0, (uint8_t*)&preamp_param_strings[i]);
		DisplayTask->ParamIndic(58, i, *preamp_param_ptrs[i]);
	}

	DisplayTask->StringOut(2, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&in_out_strings[1]);
	inp_ind_fl = 0;
	out_ind_fl = 1;
	vol_fl = 0;
	ind_poin = 500;
}

void preamp_show_second_page()
{
	DisplayTask->Clear();
	DisplayTask->Icon_Strel(12,1);

	for(uint8_t i = 0; i < 3; i++)
	{
		DisplayTask->StringOut(6,i,TDisplayTask::fntSystem , 0, (uint8_t*)&preamp_param_strings[par_num+i]);
		DisplayTask->ParamIndic(58, i, *preamp_param_ptrs[par_num + i]);
	}

	out_ind_fl = 0;
}

void GUI_preamp_select()
{
	current_menu = MENU_PREAMP;
	gui_send(15,8);

	preamp_show_first_page();

	par_num = 0;
}

void GUI_preamp_menu_task()
{
	if(!encoder_knob_selected)
	{
		DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, tim5_fl*2 , (uint8_t*)&preamp_param_strings[par_num]);
	}

	if(encoder_state_updated)
	{
		if(encoder_state == ENC_COUNTERCLOCKWISE_STEP)
		{
			if(encoder_knob_selected)
			{
				GUI_enc_param_decrase(preamp_param_ptrs, DSP_ADDRESS_PREAMP);
			}
			else
			{
				if(par_num > 0)
				{
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 0, (uint8_t*)&preamp_param_strings[par_num--]);
					if(par_num == 2)
					{
						par_num--;
						preamp_show_first_page();
					}
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 2, (uint8_t*)&preamp_param_strings[par_num]);
				}
			}
		}

		if(encoder_state == ENC_CLOCKWISE_STEP)
		{
			if(encoder_knob_selected)
			{
				GUI_enc_param_incrase(preamp_param_ptrs, DSP_ADDRESS_PREAMP);
			}
			else
			{
				if(par_num < 5)
				{
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 0, (uint8_t*)&preamp_param_strings[par_num++]);
					if(par_num == 2)
					{
						par_num++;
						preamp_show_second_page();
					}
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 2, (uint8_t*)&preamp_param_strings[par_num]);
				}
			}
		}
		tim5_start(0);
		clean_flag();
	}

	if(encoder_knob_pressed)
	{
		if(encoder_knob_selected)
		{
			DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem , 0, (uint8_t*)&preamp_param_strings[par_num]);
			encoder_knob_selected = 0;
		}
		else
		{
			DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem , 2, (uint8_t*)&preamp_param_strings[par_num]);
			encoder_knob_selected = 1;
		}
		tim5_start(1);	//1
		clean_flag();
	}

	if(k_up == 1)
	{
		GUI_return_to_main_menu(); // no gui_send(15, 0);
	}
}
