#include "presetactionsmenu.h"


#include "fs.h"
#include "eepr.h"
#include "display.h"
#include "eepr.h"
#include "cc.h"

#include "modulesmenu.h"
#include "copyselectmenu.h"

extern uint8_t cab_type;

PresetActionsMenu::PresetActionsMenu(AbstractMenu *parent, TActionType actionType)
{
	m_actionType = actionType;

	if(m_actionType == TActionType::Copy)
		m_menuType = MENU_COPY;
	else
		m_menuType = MENU_PRESET_WRITE;

	topLevelMenu = parent;
}

void PresetActionsMenu::show(TShowMode showMode)
{
	currentMenu = this;

	if(m_actionType == TActionType::Save) targetPresetNum = currentPresetNumber;

	DisplayTask -> Clear();
	updatePresetData();
}

void PresetActionsMenu::task()
{
	if(blinkFlag_fl==0)
	{

		bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab1Name[0];
		DisplayTask->Prog_ind(targetPresetNum, filled);
	}
	else if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)==1)
	{
		DisplayTask->Clear_str(87, 0, Font::fnt33x30, 39);
	}
}

void PresetActionsMenu::encoderPressed()
{
	tim5_start(0);

	if(m_actionType==TActionType::Copy)
	{
		CopySelectMenu* copySelectMenu = new CopySelectMenu(this);
		copySelectMenu->setTargetPreset(targetPresetNum);
		showChild(copySelectMenu);
	}
	else
	{
		savePreset();
		topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
	}
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

	topLevelMenu->returnFromChildMenu();

	tim5_start(0);
}

void PresetActionsMenu::keyDown()
{
	encoderPressed();
}

void PresetActionsMenu::updatePresetData()
{
	EEPROM_LoadPresetToBuffer(targetPresetNum, presetBuffer);

	EEPROM_loadBriefPreset(targetPresetNum, &m_selectedPresetBrief);

	DisplayTask->Clear_str(2, 0, Font::fntSystem, 14);
	DisplayTask->Clear_str(2, 1, Font::fntSystem, 14);

	DisplayTask->StringOut(2, 0, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.name);
	DisplayTask->StringOut(2, 1, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.comment);

	if(m_actionType==TActionType::Copy)
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Copy to ->");
	else
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Save to ->");

	bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab2Name[0];
	DisplayTask->Prog_ind(targetPresetNum, filled);

	TIM_SetCounter(TIM6, 0x8000);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);

	tim5_start(0);
}

void PresetActionsMenu::savePreset()
{
	DisplayTask->Clear();
	DisplayTask->StringOut(38, 2, Font::fnt12x13, 0, (uint8_t*)" Save");

	currentPreset.modules.rawData[147] = delay_time;
	currentPreset.modules.rawData[148] = delay_time>>8;
	EEPR_writePreset(targetPresetNum);

	send_cab_data(0, targetPresetNum+1, 0);
	if(cab_type==CAB_CONFIG_STEREO)
		send_cab_data1(0, targetPresetNum+1);

	targetPresetNum = currentPresetNumber;
	Preset::Change();

//	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}
