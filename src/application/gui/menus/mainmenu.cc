#include "mainmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"

#include "system.h"

#include "modules.h"
#include "footswitch.h"

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

	DisplayTask->Clear();
	refresh();
}

void MainMenu::task()
{
	if(m_preselectedPresetNum != currentPresetNumber)
	{
		if(blinkFlag_fl == 0)
		{
			bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab1Name[0];
			DisplayTask->Prog_ind(m_preselectedPresetNum, filled);
		}
		else
		{
			if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update) == 1)
				DisplayTask->Clear_str(87 , 0 , Font::fnt33x30 , 39);
		}
	}

	if((sys_para[System::FSW1_PRESS_TYPE] == 1) || ((sys_para[System::FSW1_HOLD_TYPE] == 1) && sys_para[System::FSW1_MODE] == Footswitch::Double)) DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[System::FSW2_PRESS_TYPE] == 1) || ((sys_para[System::FSW2_HOLD_TYPE] == 1) && sys_para[System::FSW2_MODE] == Footswitch::Double)) DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[System::FSW3_PRESS_TYPE] == 1) || ((sys_para[System::FSW3_HOLD_TYPE] == 1) && sys_para[System::FSW3_MODE] == Footswitch::Double)) DisplayTask->IndFoot(2, contr_kn[2]);

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

	tim5_start(0);
}

void MainMenu::keyDown()
{
	if(m_preselectedPresetNum != currentPresetNumber) return;

	shownChildMenu = &modulesMenu;
	modulesMenu.show();

	tim5_start(0);
}

void MainMenu::key1()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new AttenuatorMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key2()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new MasterVolumeMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key3()
{
	if(shownChildMenu) delete shownChildMenu;
	shownChildMenu = nullptr;

	shownChildMenu = new MasterEqMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key4()
{
	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = SystemMenu::create(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::key5()
{
	if(shownChildMenu) delete shownChildMenu;
	shownChildMenu = nullptr;

	shownChildMenu = new TunerMenu(this);
	shownChildMenu->show();

	tim5_start(0);
}

void MainMenu::refresh()
{
	EEPROM_loadBriefPreset(m_preselectedPresetNum, &m_selectedPresetBrief);

	DisplayTask->Clear_str(2, 0, Font::fntSystem, 14);
	DisplayTask->Clear_str(2, 1, Font::fntSystem, 14);

	DisplayTask->StringOut(2, 0, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.name);
	DisplayTask->StringOut(2, 1, Font::fntSystem, 0, (uint8_t*)m_selectedPresetBrief.comment);

	bool filled = m_selectedPresetBrief.cab1Name[0] || m_selectedPresetBrief.cab2Name[0];

	DisplayTask->Prog_ind(m_preselectedPresetNum, filled);

	if((sys_para[System::FSW1_PRESS_TYPE] == 1) || ((sys_para[System::FSW1_HOLD_TYPE] == 1) && sys_para[System::FSW1_MODE] == Footswitch::Double)) DisplayTask->IndFoot(0, contr_kn[0]);
	if((sys_para[System::FSW2_PRESS_TYPE] == 1) || ((sys_para[System::FSW2_HOLD_TYPE] == 1) && sys_para[System::FSW2_MODE] == Footswitch::Double)) DisplayTask->IndFoot(1, contr_kn[1]);
	if((sys_para[System::FSW3_PRESS_TYPE] == 1) || ((sys_para[System::FSW3_HOLD_TYPE] == 1) && sys_para[System::FSW3_MODE] == Footswitch::Double)) DisplayTask->IndFoot(2, contr_kn[2]);

}

void MainMenu::presetUp()
{
	if(m_preselectedPresetNum == 98) m_preselectedPresetNum = 0;
	else m_preselectedPresetNum++;

	refresh();

	tim5_start(0);
	TIM_SetCounter(TIM6, 0x8000);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
}

void MainMenu::presetDown()
{
	if(m_preselectedPresetNum == 0) m_preselectedPresetNum = 98;
	else m_preselectedPresetNum -= 1;

	refresh();

	tim5_start(0);
	TIM_SetCounter(TIM6,0xa000);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	TIM_Cmd(TIM6,ENABLE);
}

void MainMenu::presetChoose(uint8_t presetNum)
{
	m_preselectedPresetNum = presetNum;
	refresh();
}

void MainMenu::presetConfirm()
{
	currentPresetNumber = m_preselectedPresetNum;

	refresh();

	sys_para[System::LAST_PRESET_NUM] = currentPresetNumber;
	prog_ch();

	show();
	tim5_start(1);
}
