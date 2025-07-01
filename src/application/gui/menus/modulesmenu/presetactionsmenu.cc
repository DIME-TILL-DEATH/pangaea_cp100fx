#include "presetactionsmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"
#include "cc.h"
#include "BF706_send.h"

#include "modulesmenu.h"


extern uint8_t cab_type;

PresetActionsMenu::PresetActionsMenu(AbstractMenu *parent, TActionType actionType)
{
	m_actionType = actionType;

	if(actionType==TActionType::Copy)
		m_menuType = MENU_COPY;
	else
		m_menuType = MENU_PRESET_WRITE;

	topLevelMenu = parent;

	targetPresetNum = currentPresetNumber;
}

void PresetActionsMenu::show(TShowMode showMode)
{
	current_menu_type = m_menuType;
	currentMenu = this;

	read_prog_temp(targetPresetNum);
	imya_temp = 1;
	for(uint8_t i = 0; i<15; i++)
		imya_t[i] = preset_temp[i];
	for(uint8_t i = 0; i<15; i++)
		imya1_t[i] = preset_temp[15+i];

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(targetPresetNum);

	if(m_actionType==TActionType::Copy)
		DisplayTask->StringOut(10, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Copy to ->");
	else
		DisplayTask->StringOut(10, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Save to ->");
//
//	clean_flag();
}

void PresetActionsMenu::task()
{
	if(tim5_fl==0)
		DisplayTask->Prog_ind(targetPresetNum);
	else if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)==1)
		DisplayTask->Clear_str(87, 0, TDisplayTask::fnt33x30, 39);
}

void PresetActionsMenu::encoderPressed()
{
	if(m_actionType==TActionType::Copy)
		copyPreset();
	else
		savePreset();

	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);

	tim5_start(0);
	clean_flag();
}

void PresetActionsMenu::encoderClockwise()
{
	if(targetPresetNum<98)
		targetPresetNum += 1;
	else
		targetPresetNum = 0;

	updatePresetData();

	clean_flag();
}

void PresetActionsMenu::encoderCounterClockwise()
{
	if(targetPresetNum>0)
		targetPresetNum -= 1;
	else
		targetPresetNum = 98;

	updatePresetData();

	clean_flag();
}

void PresetActionsMenu::keyUp()
{
	if(m_actionType==TActionType::Save) return;

	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);

	tim5_start(0);
	clean_flag();
}

void PresetActionsMenu::keyDown()
{
	encoderPressed();
}

void PresetActionsMenu::updatePresetData()
{
	eepr_read_imya(targetPresetNum);
	read_prog_temp(targetPresetNum);
	for(uint8_t i = 0; i<15; i++)
		imya_t[i] = preset_temp[i];
	for(uint8_t i = 0; i<15; i++)
		imya1_t[i] = preset_temp[15+i];
	imya_temp = 1;
	DisplayTask->Main_scr();
	DisplayTask->StringOut(10, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Copy to ->");
	DisplayTask->Prog_ind(targetPresetNum);

	TIM_SetCounter(TIM6, 0x8000);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);

	tim5_start(0);
//	clean_flag();
}

void PresetActionsMenu::copyPreset()
{
	DisplayTask->Clear();
	DisplayTask->StringOut(38, 2, TDisplayTask::fnt12x13, 0, (uint8_t*)"Copy OK!");

	// parameter to copy in class?
	ModulesMenu *modulesMenu = static_cast<ModulesMenu*>(topLevelMenu);

	switch(modulesMenu->numMenu())
	{
		case 0:
			for(uint8_t i = 0; i<11; i++)
				preset_temp[30+mog_mix+i] = presetData[mog_mix+i];
			preset_temp[30+moog] = presetData[moog];
			preset_temp[30+mog_gen_t] = presetData[mog_gen_t];
		break;
		case 1:
			for(uint8_t i = 0; i<3; i++)
				preset_temp[30+gate_thr+i] = presetData[gate_thr+i];
			preset_temp[30+gate] = presetData[gate];
		break;
		case 2:
			for(uint8_t i = 0; i<5; i++)
				preset_temp[30+comp_thr+i] = presetData[comp_thr+i];
			preset_temp[30+compr] = presetData[compr];
		break;
		case 3:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+pre_gain+i] = presetData[pre_gain+i];
			preset_temp[30+pream] = presetData[pream];
		break;
		case 4:
			for(uint8_t i = 0; i<3; i++)
				preset_temp[30+am_v+i] = presetData[am_v+i];
			preset_temp[30+pre_v] = presetData[pre_v];
			preset_temp[30+amp] = presetData[amp];
		break;
		case 5:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+vol+i] = presetData[vol+i];
			for(uint8_t i = 0; i<64; i++)
				preset_temp[13344+i] = cab1_name_buf[i];
			for(uint8_t i = 0; i<64; i++)
				preset_temp[25696+i] = cab2_name_buf[i];
			for(uint16_t i = 0; i<12288; i++)
				preset_temp[1056+i] = cab1_data[i];
			if(cab_type==2)
				for(uint16_t i = 0; i<12288; i++)
					preset_temp[13408+i] = cab2_data[i];
			else
				for(uint16_t i = 0; i<12288; i++)
					preset_temp[25760+i] = cab2_data[i];
			for(uint16_t i = 0; i<512; i++)
				preset_temp[38048+i] = impulse_path[i];
			send_cab_data(0, targetPresetNum+1, 0);
			if(cab_type==2)
				send_cab_data1(0, targetPresetNum+1);
			preset_temp[30+cab] = presetData[cab];
		break;
		case 6:
			for(uint8_t i = 0; i<8; i++)
				preset_temp[30+eq1+i] = presetData[eq1+i];
			for(uint8_t i = 0; i<10; i++)
				preset_temp[30+f1+i] = presetData[f1+i];
			preset_temp[30+eq] = presetData[eq];
			preset_temp[30+eq_pr_po] = presetData[eq_pr_po];
		break;
		case 7:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+phaser_vol+i] = presetData[phaser_vol+i];
			preset_temp[30+phas] = presetData[phas];
			preset_temp[30+hpf_ph] = presetData[hpf_ph];
			preset_temp[30+phas_pos] = presetData[phas_pos];
		break;
		case 8:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+fl_v+i] = presetData[fl_v+i];
			preset_temp[30+fl] = presetData[fl];
			preset_temp[30+hpf_fl] = presetData[hpf_fl];
			preset_temp[30+flan_pos] = presetData[flan_pos];
		break;
		case 9:
			for(uint8_t i = 0; i<5; i++)
				preset_temp[30+chor_volum+i] = presetData[chor_volum+i];
			preset_temp[30+chor] = presetData[chor];
			preset_temp[30+hpf_ch] = presetData[hpf_ch];
		break;
		case 10:
			for(uint8_t i = 0; i<11; i++)
				preset_temp[30+d_vol+i] = presetData[d_vol+i];
			preset_temp[1054] = delay_time;
			preset_temp[1055] = delay_time>>8;
			preset_temp[30+delay] = presetData[delay];
			preset_temp[30+d_tap_t] = presetData[d_tap_t];
			preset_temp[30+d_tail] = presetData[d_tail];
		break;
		case 11:
			for(uint8_t i = 0; i<2; i++)
				preset_temp[30+early_vol+i] = presetData[early_vol+i];
			preset_temp[30+early] = presetData[early];
		break;
		case 12:
			for(uint8_t i = 0; i<7; i++)
				preset_temp[30+r_vol+i] = presetData[r_vol+i];
			preset_temp[30+reve] = presetData[reve];
			preset_temp[30+rev_t] = presetData[rev_t];
			preset_temp[30+rev_di] = presetData[rev_di];
			preset_temp[30+r_pre] = presetData[r_pre];
			preset_temp[30+r_tail] = presetData[r_tail];
		break;
		case 13:
			for(uint8_t i = 0; i<4; i++)
				preset_temp[30+tr_vol+i] = presetData[tr_vol+i];
			preset_temp[30+trem] = presetData[trem];
			preset_temp[30+t_tap_t] = presetData[t_tap_t];
			preset_temp[30+tr_lfo_t] = presetData[tr_lfo_t];
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
	send_cab_data(1, targetPresetNum+1, 1);
	write_prog_temp(targetPresetNum);
}

void PresetActionsMenu::savePreset()
{
	DisplayTask->Clear();
	DisplayTask->StringOut(38, 2, TDisplayTask::fnt12x13, 0, (uint8_t*)" Save");
	presetData[147] = delay_time;
	presetData[148] = delay_time>>8;
	eepr_write(preselectedPresetNumber);

	send_cab_data(0, preselectedPresetNumber+1, 0);
	if(cab_type==2)
		send_cab_data1(0, preselectedPresetNumber+1);

	preselectedPresetNumber = currentPresetNumber;
	vol_fl = write_fl = 0;
	prog_ch();

//	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}
