#include "copymenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

#include "modulesmenu.h"

extern uint8_t cab_type;

CopyMenu::CopyMenu(AbstractMenu* parent)
{
	m_menuType = MENU_COPY;
	topLevelMenu = parent;

	targetPresetNum = prog1;
}

void CopyMenu::show(TShowMode showMode)
{
	current_menu = MENU_COPY;
	currentMenu = this;

	read_prog_temp(targetPresetNum);
	imya_temp = 1;
	for(uint8_t i = 0 ; i < 15 ; i++) imya_t[i] = preset_temp[i];
	for(uint8_t i = 0 ; i < 15 ; i++) imya1_t[i] = preset_temp[15 + i];

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(targetPresetNum);
	DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");

	clean_flag();
}

void CopyMenu::task()
{
	if(tim5_fl == 0) DisplayTask->Prog_ind(targetPresetNum);
	else if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1) DisplayTask->Clear_str(87 , 0 , TDisplayTask::fnt33x30 , 39);
}

void CopyMenu::encoderPressed()
{
	copyPreset();
}

void CopyMenu::encoderClockwise()
{
	if(targetPresetNum < 98) targetPresetNum +=1;
	else targetPresetNum = 0;

	updatePresetData();
}

void CopyMenu::encoderCounterClockwise()
{
	if(targetPresetNum > 0) targetPresetNum -=1;
	else targetPresetNum = 98;

	updatePresetData();
}

void CopyMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}

void CopyMenu::keyDown()
{
	copyPreset();
}

void CopyMenu::updatePresetData()
{
	eepr_read_imya(targetPresetNum);
	read_prog_temp(targetPresetNum);
	for(uint8_t i = 0 ; i < 15 ; i++) imya_t[i] = preset_temp[i];
	for(uint8_t i = 0 ; i < 15 ; i++) imya1_t[i] = preset_temp[15 + i];
	imya_temp = 1;
	DisplayTask->Main_scr();
	DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");
	DisplayTask->Prog_ind(targetPresetNum);

	TIM_SetCounter(TIM6,0x8000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);

	tim5_start(0);
	clean_flag();
}

void CopyMenu::copyPreset()
{
	  DisplayTask->Clear();
	  DisplayTask->StringOut(38,2,TDisplayTask::fnt12x13,0,(uint8_t*)"Copy OK!");

	  // parameter to copy in class?
	  ModulesMenu* modulesMenu = static_cast<ModulesMenu*>(topLevelMenu);

	  switch(modulesMenu->numMenu()){
	  case 0:for(uint8_t i = 0 ; i < 11 ; i++)preset_temp[30 + mog_mix + i] = prog_data[mog_mix + i];
			 preset_temp[30 + moog] = prog_data[moog];
			 preset_temp[30 + mog_gen_t] = prog_data[mog_gen_t];
			 break;
	  case 1:for(uint8_t i = 0 ; i < 3 ; i++)preset_temp[30 + gate_thr + i] = prog_data[gate_thr + i];
			 preset_temp[30 + gate] = prog_data[gate];break;
	  case 2:for(uint8_t i = 0 ; i < 5 ; i++)preset_temp[30 + comp_thr + i] = prog_data[comp_thr + i];
			 preset_temp[30 + compr] = prog_data[compr];break;
	  case 3:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + pre_gain + i] = prog_data[pre_gain + i];
			 preset_temp[30 + pream] = prog_data[pream];
			 break;
	  case 4:for(uint8_t i = 0 ; i < 3 ; i++)preset_temp[30 + am_v + i] = prog_data[am_v + i];
			 preset_temp[30 + pre_v] = prog_data[pre_v];preset_temp[30 + amp] = prog_data[amp];
			 break;
	  case 5:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + vol + i] = prog_data[vol + i];
			 for(uint8_t i = 0 ; i < 64 ; i++)preset_temp[13344 + i] = name_buf[i];
			 for(uint8_t i = 0 ; i < 64 ; i++)preset_temp[25696 + i] = name_buf1[i];
			 for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[1056 + i] = cab_data[i];
			 if(cab_type == 2)for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[13408 + i] = cab_data1[i];
			 else for(uint16_t i = 0 ; i < 12288 ; i++)preset_temp[25760 + i] = cab_data1[i];
			 for(uint16_t i = 0 ; i < 512 ; i++)preset_temp[38048 + i] = impulse_path[i];
			 send_cab_data(0, targetPresetNum + 1,0);
			 if(cab_type == 2) send_cab_data1(0, targetPresetNum + 1);
			 preset_temp[30 + cab] = prog_data[cab];
			 break;
	  case 6:for(uint8_t i = 0 ; i < 8 ; i++)preset_temp[30 + eq1 + i] = prog_data[eq1 + i];
			 for(uint8_t i = 0 ; i < 10 ; i++)preset_temp[30 + f1 + i] = prog_data[f1 + i];
			 preset_temp[30 + eq] = prog_data[eq];
			 preset_temp[30 + eq_pr_po] = prog_data[eq_pr_po];
			 break;
	  case 7:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + phaser_vol + i] = prog_data[phaser_vol + i];
			 preset_temp[30 + phas] = prog_data[phas];
			 preset_temp[30 + hpf_ph] = prog_data[hpf_ph];
			 preset_temp[30 + phas_pos] = prog_data[phas_pos];
			 break;
	  case 8:for(uint8_t i = 0 ; i < 6 ; i++)preset_temp[30 + fl_v + i] = prog_data[fl_v + i];
			 preset_temp[30 + fl] = prog_data[fl];
			 preset_temp[30 + hpf_fl] = prog_data[hpf_fl];
			 preset_temp[30 + flan_pos] = prog_data[flan_pos];
			 break;
	  case 9:for(uint8_t i = 0 ; i < 5 ; i++)preset_temp[30 + chor_volum + i] = prog_data[chor_volum + i];
			 preset_temp[30 + chor] = prog_data[chor];preset_temp[30 + hpf_ch] = prog_data[hpf_ch];
			 break;
	  case 10:for(uint8_t i = 0 ; i < 11 ; i++)preset_temp[30 + d_vol + i] = prog_data[d_vol + i];
			 preset_temp[1054] = delay_time;preset_temp[1055] = delay_time >> 8;
			 preset_temp[30 + delay] = prog_data[delay];
			 preset_temp[30 + d_tap_t] = prog_data[d_tap_t];
			 preset_temp[30 + d_tail] = prog_data[d_tail];
			 break;
	  case 11:for(uint8_t i = 0 ; i < 2 ; i++)preset_temp[30 + early_vol + i] = prog_data[early_vol + i];
			 preset_temp[30 + early] = prog_data[early];
			 break;
	  case 12:for(uint8_t i = 0 ; i < 7 ; i++)preset_temp[30 + r_vol + i] = prog_data[r_vol + i];
			 preset_temp[30 + reve] = prog_data[reve];
			 preset_temp[30 + rev_t] = prog_data[rev_t];
			 preset_temp[30 + rev_di] = prog_data[rev_di];
			 preset_temp[30 + r_pre] = prog_data[r_pre];
			 preset_temp[30 + r_tail] = prog_data[r_tail];
			 break;
	  case 13:for(uint8_t i = 0 ; i < 4 ; i++)preset_temp[30 + tr_vol + i] = prog_data[tr_vol + i];
			 preset_temp[30 + trem] = prog_data[trem];
			 preset_temp[30 + t_tap_t] = prog_data[t_tap_t];
			 preset_temp[30 + tr_lfo_t] = prog_data[tr_lfo_t];
			 break;
	  }


//	  switch(sub_men)
//	  {
//	  case 1:preset_temp[30 + pres_lev] = prog_data[pres_lev];
//			  break;
//	  case 2:for(uint16_t i = 0 ; i < 512 ; i++)preset_temp[542 + i] = control[i];
//			 break;
//	  case 3:for(uint8_t i = 0 ; i < 15 ; i++)preset_temp[i] = imya[i];
//			 for(uint8_t i = 0 ; i < 15 ; i++)preset_temp[15 + i] = imya1[i];
//			  break;
//	  }

	  imya_temp = 0;
	  send_cab_data(1, targetPresetNum + 1, 1);
	  write_prog_temp(targetPresetNum);

	  topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}
