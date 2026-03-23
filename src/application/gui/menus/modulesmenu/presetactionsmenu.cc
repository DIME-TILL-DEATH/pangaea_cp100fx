#include "presetactionsmenu.h"

#include "eepr.h"

#include "controllers_task.h"
#include "display_task.h"
#include "filesystem_task.h"
#include "sharc_task.h"

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
	if(blinkFlag==0)
	{
		bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab2Name[0];
		DisplayTask->PresetInd(targetPresetNum, filled);
	}
	else if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)==1)
	{
		DisplayTask->ClearString(87, 0, Font::fnt33x30, 39);
	}
}

void PresetActionsMenu::encoderPressed()
{
	restartBlinking(0);

	if(m_actionType==TActionType::Copy)
	{
		CopySelectMenu* copySelectMenu = new CopySelectMenu(this);
		copySelectMenu->setTargetPreset(targetPresetNum);
		showChild(copySelectMenu);
	}
	else
	{
		savePreset();
		topLevelMenu->subMenusToRoot = 2;
		topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
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
	if(m_actionType==TActionType::Save)
	{
		topLevelMenu->subMenusToRoot = 1;
		topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
	}
	else
	{
		topLevelMenu->returnFromChildMenu();
	}
}

void PresetActionsMenu::keyDown()
{
	encoderPressed();
}

void PresetActionsMenu::updatePresetData()
{
	EEPROM_LoadBriefPreset(targetPresetNum, &m_selectedPresetBrief);

	DisplayTask->ClearString(2, 0, Font::fntSystem, 14);
	DisplayTask->ClearString(2, 1, Font::fntSystem, 14);

	DisplayTask->StringOut(2, 0, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.name);
	DisplayTask->StringOut(2, 1, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.comment);

	if(m_actionType==TActionType::Copy)
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Copy to ->");
	else
		DisplayTask->StringOut(10, 3, Font::fntSystem, 0, (uint8_t*)"Save to ->");

	bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab2Name[0];
	DisplayTask->PresetInd(targetPresetNum, filled);

	TIM_SetCounter(TIM6, 0x8000);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);

	restartBlinking(0);
}

void PresetActionsMenu::savePreset()
{
	DisplayTask->Clear();
	DisplayTask->StringOut(38, 2, Font::fnt12x13, 0, (uint8_t*)" Save");

	currentPreset.modulesBuf[147] = currentPreset.delayTime;
	currentPreset.modulesBuf[148] = currentPreset.delayTime>>8;
	EEPROM_WritePreset(targetPresetNum);

	SharcTask->sendPrimaryData(currentPreset.cab1Data, currentPreset.cabAuxData, currentPreset.modulesBuf, currentPresetNumber+1);
	if(cab_type==CAB_CONFIG_STEREO)
		SharcTask->sendCab2Data(currentPreset.cab2Data, currentPresetNumber+1);

	currentPresetNumber = targetPresetNum;
	Preset::Change();
}
