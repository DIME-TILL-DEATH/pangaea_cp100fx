#include "menu_pa.h"

#include "gui_task.h"

#include "../modules.h"

#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"

const uint8_t amp_param_strings[][9] ={"Volume", "Presence", "Slave", "Type"};

const uint8_t amp_types_strings[][11] = {"  PP 6L6  ","  PP EL34 ","  SE 6L6  ","  SE EL34 ",
								  " AMT TC-3 ","California","British M ","British L "," Default  ","Calif mod ","Calif vint",
                                  "PVH PR0RS0","PVH PR5RS5","PVH PR8RS7","PVH PR9RS8","PA Modern ","PP Amp 6L6","PP AmpEL34",
								  "SE Amp 6L6","PVH IC V1 ","PVH IC V2 ","PVH PR00  ","PVH PR98  "};


uint8_t* amp_param_ptrs[] = {
	&prog_data[AMP_MASTER],
	&prog_data[AMP_PRESENCE],
	&prog_data[AMP_TYPE],
	&prog_data[AMP_LEVEL]
};

//gui_param_t amp_params[] = {
//	{"Volume", &prog_data[AMP_MASTER], DSP_ADDRESS_AMP_VOLUME, 127, 58},
//	{"Presence", &prog_data[pre_v], DSP_ADDRESS_AMP_VOLUME, 127, 58}	// pos = 7
//};

void GUI_amp_select()
{
	DisplayTask->Clear();
//	DisplayTask->Icon_Strel(8,2);
	for(uint8_t i = 0 ; i < 3 ; i++)
	{
		DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)&amp_param_strings[i]);
		if(i == 1)DisplayTask->ParamIndic(58,i,prog_data[pre_v]);
		else DisplayTask->ParamIndic(58,i,prog_data[am_v + i]);
	}
	DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)&in_out_strings[1]);

	inp_ind_fl = 0;
	out_ind_fl = 1;
	vol_fl = 0;
	ind_poin = 500;
	par_num = 0;
}

void GUI_amp_menu_task()
{
	if(!encoder_knob_selected)
	{
		DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,tim5_fl * 2,(uint8_t*)amp_param_strings + par_num*9);
	}

	if(encoder_state_updated)
	{
		if(encoder_state == ENC_COUNTERCLOCKWISE_STEP)
		{
			if(encoder_knob_selected)
			{
				switch(par_num){
				case 0:case 2:
					if(prog_data[am_v + par_num] > 0)
					{
						prog_data[am_v + par_num] = enc_speed_dec(prog_data[am_v + par_num],0);
						DisplayTask->ParamIndic(58,par_num%3,prog_data[am_v + par_num]);
						gui_send(11,par_num);
					}
					break;
				case 1:
					if(prog_data[pre_v] > 0)
					{
						prog_data[pre_v] = enc_speed_dec(prog_data[pre_v],0);
						DisplayTask->ParamIndic(58,par_num%3,prog_data[pre_v]);
						gui_send(4,7);
					}
					break;
				case 3:if(prog_data[am_mode] > 0)
					{
						DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_types_strings + --prog_data[am_mode]*11);
						gui_send(23,0);
					}break;
				}
			}
			else
			{
				if(par_num > 0)
				{
					DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)amp_param_strings + par_num--*9);
					if(par_num == 2)
					{
						DisplayTask->Clear();
//						DisplayTask->Icon_Strel(8,2);
						for(uint8_t i = 0 ; i < 3 ; i++)
						{
							DisplayTask->StringOut(3,i,TDisplayTask::fntSystem , 0 , (uint8_t*)amp_param_strings + i*9);
							if(i == 1)DisplayTask->ParamIndic(58,i,prog_data[pre_v]);
							else DisplayTask->ParamIndic(58,i,prog_data[am_v + i]);
						}
						DisplayTask->StringOut(2,3,TDisplayTask::fntSystem , 0 , (uint8_t*)in_out_strings + 7);
						inp_ind_fl = 0;
						out_ind_fl = 1;
						vol_fl = 0;
						ind_poin = 500;
					}
					DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)amp_param_strings + par_num*9);
			   }
		   }
		   tim5_start(0);
		}

		if(encoder_state == ENC_CLOCKWISE_STEP)
		{
			if(encoder_knob_selected)
			{
				switch(par_num){
				case 0:case 2:
					GUI_enc_param_incrase(amp_param_ptrs, DSP_ADDRESS_AMP);
					break;

				case 1:
					if(prog_data[pre_v] < 127)
					{
						prog_data[pre_v] = enc_speed_inc(prog_data[pre_v],127);
						DisplayTask->ParamIndic(58,par_num%3,prog_data[pre_v]);
						gui_send(4, 7);
					}
					break;
				case 3:
					if(prog_data[am_mode] < 14)
					{
						DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_types_strings + ++prog_data[am_mode]*11);
						gui_send(23,0);
					}
					break;
				}
			}
			else
			{
				if(par_num < 3)
				{
					DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,0,(uint8_t*)amp_param_strings + par_num++*9);
					if(par_num == 3)
					{
						out_ind_fl = 0;
						DisplayTask->Clear();
//						DisplayTask->Icon_Strel(8,1);
						DisplayTask->StringOut(36,0,TDisplayTask::fntSystem,0,(uint8_t*)amp_types_strings + prog_data[am_mode]*11);
					}
					DisplayTask->StringOut(3,par_num%3,TDisplayTask::fntSystem,2,(uint8_t*)amp_param_strings + par_num*9);

				}
			}
		}
		tim5_start(0);
		clean_flag();
	}

	GUI_param_select_task((uint8_t**)amp_param_strings);

	if(k_up == 1)
	{
		vol_fl = 0;
		gui_send(15,0);
		GUI_return_to_main_menu();
	}
}
