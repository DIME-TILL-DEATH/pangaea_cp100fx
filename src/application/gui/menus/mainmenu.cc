#include "mainmenu.h"

#include "eepr.h"
#include "system.h"
#include "modules.h"
#include "footswitch.h"

#include "controllers_task.h"
#include "display_task.h"
#include "filesystem_task.h"
#include "io_task.h"
#include "midi_task.h"
#include "ui_task.h"

#include "allFonts.h"

#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "cabnamemenu.h"
#include "attenuatormenu.h"
#include "mastervolumemenu.h"
#include "mastereqmenu.h"
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

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	m_preselectedPresetNum = currentPresetNumber;
	EEPROM_LoadBriefPreset(m_preselectedPresetNum, &m_selectedPresetBrief);

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
							|| (cab_type == CAB_CONFIG_STEREO ? m_selectedPresetBrief.cab2Name[0] : 0);
			DisplayTask->Prog_ind(m_preselectedPresetNum, filled);
		}
		else
		{
			DisplayTask->Clear_str(87 , 0 , Font::fnt33x30 , 39);
		}
		DisplayTask->IndFoot(0, m_selectedPresetBrief.modules.foot_ind_press[0], m_selectedPresetBrief.modules.foot_ind_hold[0]);
		DisplayTask->IndFoot(1, m_selectedPresetBrief.modules.foot_ind_press[1], m_selectedPresetBrief.modules.foot_ind_hold[1]);
		DisplayTask->IndFoot(2, m_selectedPresetBrief.modules.foot_ind_press[2], m_selectedPresetBrief.modules.foot_ind_hold[2]);
	}
}

void MainMenu::encoderPressed()
{
	presetConfirm();
}

void MainMenu::encoderClockwise()
{
	presetUp();
}

void MainMenu::encoderCounterClockwise()
{
	presetDown();
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

	shownChildMenu = new AttenuatorMenu(this);
	shownChildMenu->show();

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
	DisplayTask->Clear_str(2, 0, Font::fntSystem, 14);
	DisplayTask->Clear_str(2, 1, Font::fntSystem, 14);

	DisplayTask->StringOut(2, 0, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.name);
	DisplayTask->StringOut(2, 1, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.comment);

	bool filled;
	if(currentPresetNumber != m_preselectedPresetNum)
	{
		filled = m_selectedPresetBrief.cab1Name[0]
				|| (cab_type == CAB_CONFIG_STEREO ? m_selectedPresetBrief.cab2Name[0] : 0);
	}
	else
	{
		filled = currentPreset.cab1NameSize
				|| (cab_type == CAB_CONFIG_STEREO ? currentPreset.cab2NameSize : 0);
	}

	DisplayTask->Prog_ind(m_preselectedPresetNum, filled);

	DisplayTask->IndFoot(0, currentPreset.paramData.foot_ind_press[0], currentPreset.paramData.foot_ind_hold[0]);
	DisplayTask->IndFoot(1, currentPreset.paramData.foot_ind_press[1], currentPreset.paramData.foot_ind_hold[1]);
	DisplayTask->IndFoot(2, currentPreset.paramData.foot_ind_press[2], currentPreset.paramData.foot_ind_hold[2]);
}

void MainMenu::presetUp()
{
	if(m_preselectedPresetNum == 98) m_preselectedPresetNum = 0;
	else m_preselectedPresetNum++;

	EEPROM_LoadBriefPreset(m_preselectedPresetNum, &m_selectedPresetBrief);
	refresh();

	restartBlinking(1);
}

void MainMenu::presetDown()
{
	if(m_preselectedPresetNum == 0) m_preselectedPresetNum = 98;
	else m_preselectedPresetNum -= 1;

	EEPROM_LoadBriefPreset(m_preselectedPresetNum, &m_selectedPresetBrief);
	refresh();

	restartBlinking(1);
}

void MainMenu::presetChoose(uint8_t presetNum)
{
	m_preselectedPresetNum = presetNum;
	refresh();
}

void MainMenu::presetConfirm()
{
	currentPresetNumber = m_preselectedPresetNum;

	EEPROM_LoadBriefPreset(m_preselectedPresetNum, &m_selectedPresetBrief);

	sys_para[System::LAST_PRESET_NUM] = currentPresetNumber;
	Preset::Change();
	MidiTask->pcSend(TMidiTask::TPcType::PC_INTERNAL, currentPresetNumber);

	show();
	restartBlinking(1);
}
