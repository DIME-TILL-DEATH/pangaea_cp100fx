#include "cabbrowsermenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

extern uint8_t sd_init_fl;
extern volatile uint8_t imp_dir_fl;

extern volatile uint8_t file_fl;
extern volatile uint8_t action_fl;

extern uint8_t cab_type;

CabBrowserMenu::CabBrowserMenu(AbstractMenu *parent, uint8_t cabNumber)
{
	topLevelMenu = parent;
	m_menuType = MENU_CABBROWSER;

	m_cabNumber = cabNumber;
}

void CabBrowserMenu::show(TShowMode showMode)
{
	if(sd_init_fl==1)
	{

		DisplayTask->Clear();

		if(imp_dir_fl)
		{
			FSTask->SendCommand(TFsBrowser::bcCurrent);
			FSTask->SendCommand(TFsBrowser::bcLoadImp);
		}
		else
		{
			DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"There is no directory"); //imp_dir_n
			DisplayTask->StringOut(42, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"IMPULSE");	//imp_dir_no
			CSTask->CS_del(1000);
			DisplayTask->Clear();

			topLevelMenu->returnFromChildMenu();
		}
	}
	else
	{
		DisplayTask->Clear();

		if(!sd_init_fl)
			DisplayTask->StringOut(6, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"MicroSD is not ready"); //sd_nr
		else
			DisplayTask->StringOut(0, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"MicroSD is loading..");  //sd_lo

		CSTask->CS_del(1000);
		DisplayTask->Clear();

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::keyUp()
{
	DisplayTask->Clear();

	if(m_cabNumber == 0)
	{
//		if(name_buf[0]==0)
//		{
//			gui_send(16, 0);
//			vol_vol = presetData[pres_lev];
//			DisplayTask->ParamIndicNum(42, 3, presetData[pres_lev]);
//			//??????????? Почему в MainMenu??????
//			//return_to_main_menu without clean flag
//
//			DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
//			current_menu = MENU_MAIN;
//			DisplayTask->Menu_init();
//			tim5_start(0);
//
//			presetData[cab] = 0;
//			gui_send(18, 0|(presetData[cab]<<8));
//		}
//		else
//		{
			gui_send(16, 0);
			vol_ind_level_pos = presetData[IR_VOLUME1];
			gui_send(7, 0);

			topLevelMenu->returnFromChildMenu();
//		}
	}
	else
	{
		gui_send(17, 0);

		vol_ind_level_pos = presetData[IR_VOLUME2];
		gui_send(7, 3);

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::encoderPressed()
{
	FSTask->SendCommand(TFsBrowser::bcAction);

	while(!action_fl);
	action_fl = 0;

	if(file_fl)
	{
		file_fl = 0;
		DisplayTask->Clear();

		if(m_cabNumber==0)
		{
			kgp_sdk_libc::memcpy(cab1_data, preset_temp, 12288);

			if(cab_type!=2) kgp_sdk_libc::memcpy(cab2_data, preset_temp+12288, 12288);

			send_cab_data(0, 0, 0);

			kgp_sdk_libc::memcpy(cab1_name_buf, name_buf_temp, 64);

//			if(name_buf[0]==0)
//			{
//				vol_vol = presetData[pres_lev];
//				DisplayTask->ParamIndicNum(42, 3, presetData[pres_lev]);
//				// return to main menu without clean flag
//				encoder_knob_selected = 0;
//				DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
//				current_menu = MENU_MAIN;
//				DisplayTask->Menu_init();
//				tim5_start(0);
//				//
//				presetData[cab] = 0;
//				gui_send(18, 0|(presetData[cab]<<8));
//
//			}

			vol_ind_level_pos = presetData[vol];
			gui_send(7, 0);

		}
		else
		{
			kgp_sdk_libc::memcpy(cab2_data, preset_temp, 12288);
			send_cab_data1(0, 0);
			kgp_sdk_libc::memcpy(cab2_name_buf, name_buf_temp, 64);

			vol_ind_level_pos = presetData[IR_VOLUME2];
			gui_send(7, 3);
		}

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::encoderCounterClockwise()
{
	FSTask->SendCommand(TFsBrowser::bcUp);
}

void CabBrowserMenu::encoderClockwise()
{
	FSTask->SendCommand(TFsBrowser::bcDown);
}


