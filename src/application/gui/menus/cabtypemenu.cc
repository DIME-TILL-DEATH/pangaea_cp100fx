#include "cabtypemenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

constexpr static uint8_t cab_list_menu[][9] =
{"Cabinet1", "Cabinet2"};
constexpr static uint8_t cab_list[][8] =
{"Pan", "Browser", "Volume"};
//constexpr static uint8_t cab_out_list[][6] =
//{"1 L+R", "1R AP", "2 L+R", "1R A ", "1R P ", " 1 R "};

extern uint8_t cab_type;
extern uint8_t name_run_fl;

CabTypeMenu::CabTypeMenu(AbstractMenu *parent)
{
	m_menuType = MENU_CABTYPE;
	topLevelMenu = parent;

}

void CabTypeMenu::show(TShowMode showMode)
{
	current_menu = m_menuType;
	currentMenu = this;

	DisplayTask->Clear();
	if(cab_type!=2)
	{
		name_run_fl = 1;
		DisplayTask->StringOut(1, 2, TDisplayTask::fntSystem, 0, (uint8_t*)cab_list+8);
		DisplayTask->StringOut(1, 3, TDisplayTask::fntSystem, 0, (uint8_t*)cab_list+16);
		DisplayTask->ParamIndicNum(42, 3, presetData[vol]);
		vol_vol = presetData[vol];
		gui_send(7, 0);
		current_menu = MENU_CABSIM;
		DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_CAB1);
		m_paramNum = 1;
	}
	else
	{
		DisplayTask->StringOut(6, 0, TDisplayTask::fntSystem, 0, (uint8_t*)cab_list_menu);
		DisplayTask->StringOut(6, 1, TDisplayTask::fntSystem, 0, (uint8_t*)cab_list_menu+9);
		current_menu = MENU_CABTYPE;
		m_paramNum = 0;
	}
}

void CabTypeMenu::task()
{
	DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2*tim5_fl, (uint8_t*)&cab_list_menu[m_paramNum]);
}

void CabTypeMenu::encoderPressed()
{
	select();
}

void CabTypeMenu::encoderClockwise()
{
	if(!m_paramNum)
	{
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)&cab_list_menu[m_paramNum++]);
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2, (uint8_t*)&cab_list_menu[m_paramNum]);
	}
	tim5_start(0);
	clean_flag();
}

void CabTypeMenu::encoderCounterClockwise()
{
	if(m_paramNum)
	{
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)&cab_list_menu[m_paramNum--]);
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2, (uint8_t*)&cab_list_menu[m_paramNum]);

	}
	tim5_start(0);
	clean_flag();
}

void CabTypeMenu::keyDown()
{
	select();
}

void CabTypeMenu::select()
{
	cab_num = m_paramNum;
	DisplayTask->Clear();
	if(!m_paramNum)
	{
		if(name_buf[0]==0)
		{
			name_run_fl = 0;
			DisplayTask->StringOut(38, 0, TDisplayTask::fntSystem, 0, (uint8_t*)no_loaded);
		}
		else
			name_run_fl = 1;

		DisplayTask->ParamIndicPan(72, 1, presetData[cab1_pan]);
		DisplayTask->ParamIndicNum(42, 3, presetData[vol]);
		vol_vol = presetData[vol];
		DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB1);
	}
	else
	{
		if(name_buf1[0]==0)
		{
			name_run_fl = 0;
			DisplayTask->StringOut(38, 0, TDisplayTask::fntSystem, 0, (uint8_t*)no_loaded);
		}
		else
			name_run_fl = 1;

		DisplayTask->ParamIndicPan(72, 1, presetData[cab2_pan]);
		DisplayTask->ParamIndicNum(42, 3, presetData[cab2_vol]);
		vol_vol = presetData[cab2_vol];
		DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_VOLUME, DSP_INDICATOR_CAB2);
	}

	for(uint8_t i = 0; i<3; i++)
		DisplayTask->StringOut(1, i+1, TDisplayTask::fntSystem, 0, (uint8_t*)cab_list+i*8);

	current_menu = MENU_CABSIM;
	ind_poin = 500;
	vol_fl = 1;

	if(cab_num&&!name_buf1[0])
	{
		name_run_fl = 0;
		encoder_knob_pressed = 1;
		k_down = 0;
		m_paramNum = 1;
		CSTask->Give();
		tim5_start(0);
		return;
	}

	if(!cab_num&&!name_buf[0])
	{
		name_run_fl = 0;
		encoder_knob_pressed = 1;
		k_down = 0;
		m_paramNum = 1;
		CSTask->Give();
		tim5_start(0);
		return;
	}

	tim5_start(0);
	clean_flag();
}
