#include "presetactionsmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"
#include "cc.h"
#include "BF706_send.h"

#include "modulesmenu.h"
#include "preset.h"


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
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Copy to ->");
	else
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Save to ->");
}

void PresetActionsMenu::task()
{
	if(blinkFlag_fl==0)
		DisplayTask->Prog_ind(targetPresetNum);
	else if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)==1)
		DisplayTask->Clear_str(87, 0, Font::fnt33x30, 39);
}

void PresetActionsMenu::encoderPressed()
{
	if(m_actionType==TActionType::Copy)
		copyPreset();
	else
		savePreset();

	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);

	tim5_start(0);
}

void PresetActionsMenu::encoderClockwise()
{
	if(targetPresetNum<98)
		targetPresetNum += 1;
	else
		targetPresetNum = 0;

	updatePresetData();
}

void PresetActionsMenu::encoderCounterClockwise()
{
	if(targetPresetNum>0)
		targetPresetNum -= 1;
	else
		targetPresetNum = 98;

	updatePresetData();
}

void PresetActionsMenu::keyUp()
{
	if(m_actionType==TActionType::Save) return;

	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);

	tim5_start(0);
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
	DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Copy to ->");
	DisplayTask->Prog_ind(targetPresetNum);

	TIM_SetCounter(TIM6, 0x8000);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);

	tim5_start(0);
}

void PresetActionsMenu::copyPreset()
{
	DisplayTask->Clear();
	DisplayTask->StringOut(38, 2, Font::fnt12x13, 0, (uint8_t*)"Copy OK!");

	// parameter to copy in class?
	ModulesMenu *modulesMenu = static_cast<ModulesMenu*>(topLevelMenu);

	switch(modulesMenu->numMenu())
	{
		case 0:
			for(uint8_t i = 0; i<11; i++)
				preset_temp[30+mog_mix+i] = currentPreset.modules.rawData[mog_mix+i];
			preset_temp[30+moog] = currentPreset.modules.rawData[moog];
			preset_temp[30+mog_gen_t] = currentPreset.modules.rawData[mog_gen_t];
		break;
		case 1:
			for(uint8_t i = 0; i<3; i++)
				preset_temp[30+gate_thr+i] = currentPreset.modules.rawData[gate_thr+i];
			preset_temp[30+gate] = currentPreset.modules.rawData[gate];
		break;
		case 2:
			for(uint8_t i = 0; i<5; i++)
				preset_temp[30+comp_thr+i] = currentPreset.modules.rawData[comp_thr+i];
			preset_temp[30+compr] = currentPreset.modules.rawData[compr];
		break;
		case 3:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+pre_gain+i] = currentPreset.modules.rawData[pre_gain+i];
			preset_temp[30+pream] = currentPreset.modules.rawData[pream];
		break;
		case 4:
			for(uint8_t i = 0; i<3; i++)
				preset_temp[30+am_v+i] = currentPreset.modules.rawData[am_v+i];
			preset_temp[30+pre_v] = currentPreset.modules.rawData[pre_v];
			preset_temp[30+amp] = currentPreset.modules.rawData[amp];
		break;
		case 5:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+vol+i] = currentPreset.modules.rawData[vol+i];
			for(uint8_t i = 0; i<64; i++)
				preset_temp[13344+i] = cab1.name[i];
			for(uint8_t i = 0; i<64; i++)
				preset_temp[25696+i] = cab2.name[i];
			for(uint16_t i = 0; i<12288; i++)
				preset_temp[1056+i] = cab1.data[i];
			if(cab_type==2)
				for(uint16_t i = 0; i<12288; i++)
					preset_temp[13408+i] = cab2.data[i];
			else
				for(uint16_t i = 0; i<12288; i++)
					preset_temp[25760+i] = cab2.data[i];
			for(uint16_t i = 0; i<512; i++)
				preset_temp[38048+i] = impulse_path[i];
			send_cab_data(0, targetPresetNum+1, 0);
			if(cab_type==2)
				send_cab_data1(0, targetPresetNum+1);
			preset_temp[30+cab] = currentPreset.modules.rawData[cab];
		break;
		case 6:
			for(uint8_t i = 0; i<8; i++)
				preset_temp[30+eq1+i] = currentPreset.modules.rawData[eq1+i];
			for(uint8_t i = 0; i<10; i++)
				preset_temp[30+f1+i] = currentPreset.modules.rawData[f1+i];
			preset_temp[30+eq] = currentPreset.modules.rawData[eq];
			preset_temp[30+eq_pr_po] = currentPreset.modules.rawData[eq_pr_po];
		break;
		case 7:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+phaser_vol+i] = currentPreset.modules.rawData[phaser_vol+i];
			preset_temp[30+phas] = currentPreset.modules.rawData[phas];
			preset_temp[30+hpf_ph] = currentPreset.modules.rawData[hpf_ph];
			preset_temp[30+phas_pos] = currentPreset.modules.rawData[phas_pos];
		break;
		case 8:
			for(uint8_t i = 0; i<6; i++)
				preset_temp[30+fl_v+i] = currentPreset.modules.rawData[fl_v+i];
			preset_temp[30+fl] = currentPreset.modules.rawData[fl];
			preset_temp[30+hpf_fl] = currentPreset.modules.rawData[hpf_fl];
			preset_temp[30+flan_pos] = currentPreset.modules.rawData[flan_pos];
		break;
		case 9:
			for(uint8_t i = 0; i<5; i++)
				preset_temp[30+chor_volum+i] = currentPreset.modules.rawData[chor_volum+i];
			preset_temp[30+chor] = currentPreset.modules.rawData[chor];
			preset_temp[30+hpf_ch] = currentPreset.modules.rawData[hpf_ch];
		break;
		case 10:
			for(uint8_t i = 0; i<11; i++)
				preset_temp[30+d_vol+i] = currentPreset.modules.rawData[d_vol+i];
			preset_temp[1054] = delay_time;
			preset_temp[1055] = delay_time>>8;
			preset_temp[30+delay] = currentPreset.modules.rawData[delay];
			preset_temp[30+d_tap_t] = currentPreset.modules.rawData[d_tap_t];
			preset_temp[30+d_tail] = currentPreset.modules.rawData[d_tail];
		break;
		case 11:
			for(uint8_t i = 0; i<2; i++)
				preset_temp[30+early_vol+i] = currentPreset.modules.rawData[early_vol+i];
			preset_temp[30+early] = currentPreset.modules.rawData[early];
		break;
		case 12:
			for(uint8_t i = 0; i<7; i++)
				preset_temp[30+r_vol+i] = currentPreset.modules.rawData[r_vol+i];
			preset_temp[30+reve] = currentPreset.modules.rawData[reve];
			preset_temp[30+rev_t] = currentPreset.modules.rawData[rev_t];
			preset_temp[30+rev_di] = currentPreset.modules.rawData[rev_di];
			preset_temp[30+r_pre] = currentPreset.modules.rawData[r_pre];
			preset_temp[30+r_tail] = currentPreset.modules.rawData[r_tail];
		break;
		case 13:
			for(uint8_t i = 0; i<4; i++)
				preset_temp[30+tr_vol+i] = currentPreset.modules.rawData[tr_vol+i];
			preset_temp[30+trem] = currentPreset.modules.rawData[trem];
			preset_temp[30+t_tap_t] = currentPreset.modules.rawData[t_tap_t];
			preset_temp[30+tr_lfo_t] = currentPreset.modules.rawData[tr_lfo_t];
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
	DisplayTask->StringOut(38, 2, Font::fnt12x13, 0, (uint8_t*)" Save");
	currentPreset.modules.rawData[147] = delay_time;
	currentPreset.modules.rawData[148] = delay_time>>8;
	eepr_write(preselectedPresetNumber);

	send_cab_data(0, preselectedPresetNumber+1, 0);
	if(cab_type==2)
		send_cab_data1(0, preselectedPresetNumber+1);

	preselectedPresetNumber = currentPresetNumber;
	prog_ch();

//	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}
