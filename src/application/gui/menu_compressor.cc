#include "menu_compressor.h"

#include "gui_task.h"

#include "../modules.h"

#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

const uint8_t compressor_param_strings[][16] = {"Thresh\0", "Ratio\0", "Volume\0", "Attack\0", "Release\0"};

uint8_t* compressor_param_ptrs[] = {
	&prog_data[COMPRESSOR_THRESHOLD],
	&prog_data[COMPRESSOR_RATIO],
	&prog_data[COMPRESSOR_VOLUME],
	&prog_data[COMPRESSOR_ATTACK],
	&prog_data[COMPRESSOR_RELEASE],
};

void compressor_show_first_page()
{
	DisplayTask->Clear();
//	DisplayTask->Icon_Strel(13, 2);
	for(uint8_t i = 0; i < 4; i++)
	{
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem , 0, (uint8_t*)&compressor_param_strings[i]);
		DisplayTask->ParamIndic(53, i, *compressor_param_ptrs[i]);
	}
}

void compressor_show_second_page()
{
	DisplayTask->Clear();
//	DisplayTask->Icon_Strel(13,1);
	DisplayTask->ParamIndic(53, par_num%3, *compressor_param_ptrs[par_num]);
}

uint8_t symbol_offset;
void GUI_compressor_select()
{
	compressor_show_first_page();
	par_num = 0;
	symbol_offset=0;
}

//uint8_t short_name[7] = {1, 2, 3, 0, 0, 0, 0};
void GUI_compressor_menu_task()
{
	if(!encoder_knob_selected)
	{
//		if(kgp_sdk_libc::strlen((char*)compressor_param_strings[par_num]) > 6)
//		{
////			if(symbol_offset == kgp_sdk_libc::strlen((char*)compressor_param_strings[par_num])) symbol_offset = 0;
//			if(symbol_offset == 3) symbol_offset = 0;
//			else symbol_offset++;
//		}
//		else symbol_offset = 0;

//		kgp_sdk_libc::memcpy(short_name, &compressor_param_strings[par_num][symbol_offset], 6);

		DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, tim5_fl*2 , (uint8_t*)&compressor_param_strings[par_num]);
//		DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, tim5_fl*2 , short_name);
	}

	if(encoder_state_updated)
	{
		if(encoder_state == ENC_COUNTERCLOCKWISE_STEP)
		{
			if(!encoder_knob_selected)
			{
				if(par_num > 0)
				{
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 0, (uint8_t*)&compressor_param_strings[par_num]);
					par_num--;
					if(par_num == 3)
					{
						compressor_show_first_page();
					}
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 2, (uint8_t*)&compressor_param_strings[par_num]);
					tim5_start(0);
				}
			}
			else
			{
				GUI_enc_param_decrase(compressor_param_ptrs, DSP_ADDRESS_COMPRESSOR);
			}
		}

		if(encoder_state == ENC_CLOCKWISE_STEP)
		{
			if(!encoder_knob_selected)
			{
				if(par_num < 4)
				{
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 0, (uint8_t*)&compressor_param_strings[par_num]);
					par_num++;
					if(par_num == 4)
					{
						compressor_show_second_page();
					}
					DisplayTask->StringOut(6, par_num%3, TDisplayTask::fntSystem, 2, (uint8_t*)&compressor_param_strings[par_num]);
					tim5_start(0);
				}
			}
			else
			{
				GUI_enc_param_incrase(compressor_param_ptrs, DSP_ADDRESS_COMPRESSOR);
			}
		}
		clean_flag();
	}

	GUI_param_select_task((uint8_t**)compressor_param_strings);

	if(k_up == 1)
	{
		gui_send(15, 0);
		GUI_return_to_main_menu();
	}
}
