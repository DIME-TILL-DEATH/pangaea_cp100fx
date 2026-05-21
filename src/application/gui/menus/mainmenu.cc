#include <eeprom.h>
#include "mainmenu.h"

#include "system.h"
#include "modules.h"
#include "footswitch.h"

#include "controllers_task.h"
#include "display_task.h"
#include "filesystem_task.h"
#include "io_task.h"
#include "midi_task.h"
#include "ui_task.h"

#include "bitmaps.h"

#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "cabnamemenu.h"
#include "attenuatormenu.h"
#include "mastervolumemenu.h"
#include "mastereqmenu.h"
#include "preset_accessors.h"
#include "systemmenu.h"
#include "tunermenu.h"

MainMenu::MainMenu()
{
	m_menuType = MENU_MAIN;
}

void MainMenu::show(TShowMode swhoMode)
{
	currentMenu = this;
	shownChildMenu = nullptr;

	DisplayTask->SetVolIndicator(TVolIndicatorType::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	m_preselectedPresetNum = currentPresetNumber;
	EEPROM_LoadPresetBrief(m_preselectedPresetNum, &m_selectedPresetBrief);

	DisplayTask->Clear();
	refresh();
}

void MainMenu::task()
{
	if(m_preselectedPresetNum != currentPresetNumber)
	{
		if(blinkFlag == 0)
		{
			bool filled = m_selectedPresetBrief.cab1Name[0]
							|| (System::cab_type == CAB_CONFIG_STEREO ? m_selectedPresetBrief.cab2Name[0] : 0);
			DisplayTask->PresetInd(m_preselectedPresetNum, filled);
		}
		else
		{
			DisplayTask->ClearString(87 , 0 , Font::fnt33x30 , 39);
		}
		DisplayTask->FswInd(0, m_selectedPresetBrief.paramData.foot_ind_press[0], m_selectedPresetBrief.paramData.foot_ind_hold[0]);
		DisplayTask->FswInd(1, m_selectedPresetBrief.paramData.foot_ind_press[1], m_selectedPresetBrief.paramData.foot_ind_hold[1]);
		DisplayTask->FswInd(2, m_selectedPresetBrief.paramData.foot_ind_press[2], m_selectedPresetBrief.paramData.foot_ind_hold[2]);
	}
}

void MainMenu::encoderPressed()
{
	preset_change_handler(m_preselectedPresetNum);
	refresh();
	restartBlinking(1);
}

void MainMenu::encoderClockwise()
{
	presetUp();
	refresh();
	restartBlinking(1);
}

void MainMenu::encoderCounterClockwise()
{
	presetDown();
	refresh();
	restartBlinking(1);
}

void MainMenu::keyUp()
{
	if(m_preselectedPresetNum != currentPresetNumber)
	{
		m_preselectedPresetNum = currentPresetNumber;
		refresh();
	}
	else
	{
		if(shownChildMenu) delete shownChildMenu;

		shownChildMenu = new CabNameMenu(this);
		shownChildMenu->show();
	}

	restartBlinking(0);
}

void MainMenu::keyDown()
{
	if(m_preselectedPresetNum != currentPresetNumber) return;

	shownChildMenu = new ModulesMenu(this);
	shownChildMenu->show();

	restartBlinking(0);
}

void MainMenu::key1()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

#ifdef __MONO_MOD__
	shownChildMenu = new AttenuatorMenu(this);
	shownChildMenu->show();
#endif

#ifdef __STEREO_MOD__

#endif
	restartBlinking(0);
}

void MainMenu::key2()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new MasterVolumeMenu(this);
	shownChildMenu->show();

	restartBlinking(0);
}

void MainMenu::key3()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new MasterEqMenu(this);
	shownChildMenu->show();

	restartBlinking(0);
}

void MainMenu::key4()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = SystemMenu::create(this);
	shownChildMenu->show();

	restartBlinking(0);
}

void MainMenu::key5()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new TunerMenu(this);
	shownChildMenu->show();

	restartBlinking(0);
}

void MainMenu::refresh()
{
	DisplayTask->ClearString(2, 0, Font::fntSystem, 14);
	DisplayTask->ClearString(2, 1, Font::fntSystem, 14);



	uint8_t* name;
	uint8_t* comment;

	bool filled;
	if(currentPresetNumber != m_preselectedPresetNum)
	{
		name = (uint8_t*)m_selectedPresetBrief.name;
		comment = (uint8_t*)m_selectedPresetBrief.comment;

		filled = m_selectedPresetBrief.cab1Name[0]
				|| (System::cab_type == CAB_CONFIG_STEREO ? m_selectedPresetBrief.cab2Name[0] : 0);
	}
	else
	{
		name = (uint8_t*)currentPreset.name;
			comment = (uint8_t*)currentPreset.comment;

		filled = currentPreset.cab1NameSize
				|| (System::cab_type == CAB_CONFIG_STEREO ? currentPreset.cab2NameSize : 0);
	}

	DisplayTask->StringOut(2, 0, Font::fntSystem, Font::fnsNormal, name);
	DisplayTask->StringOut(2, 1, Font::fntSystem, Font::fnsNormal, comment);

	DisplayTask->PresetInd(m_preselectedPresetNum, filled);

	DisplayTask->FswInd(0, currentPreset.paramData.foot_ind_press[0], currentPreset.paramData.foot_ind_hold[0]);
	DisplayTask->FswInd(1, currentPreset.paramData.foot_ind_press[1], currentPreset.paramData.foot_ind_hold[1]);
	DisplayTask->FswInd(2, currentPreset.paramData.foot_ind_press[2], currentPreset.paramData.foot_ind_hold[2]);
}

void MainMenu::presetUp()
{
	if(m_preselectedPresetNum == 98) m_preselectedPresetNum = 0;
	else m_preselectedPresetNum++;

	EEPROM_LoadPresetBrief(m_preselectedPresetNum, &m_selectedPresetBrief);
}

void MainMenu::presetDown()
{
	if(m_preselectedPresetNum == 0) m_preselectedPresetNum = 98;
	else m_preselectedPresetNum -= 1;

	EEPROM_LoadPresetBrief(m_preselectedPresetNum, &m_selectedPresetBrief);
}

void MainMenu::presetChoose(uint8_t presetNum)
{
	m_preselectedPresetNum = presetNum;
	EEPROM_LoadPresetBrief(m_preselectedPresetNum, &m_selectedPresetBrief);
}

void MainMenu::presetConfirm()
{
	m_preselectedPresetNum = currentPresetNumber;
	EEPROM_LoadPresetBrief(m_preselectedPresetNum, &m_selectedPresetBrief);
}
