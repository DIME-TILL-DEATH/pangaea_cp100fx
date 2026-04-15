#include "midimapmenu.h"

#include "system.h"

#include "display_task.h"

MidiMapMenu::MidiMapMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MIDI_PC;
}

void MidiMapMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	DisplayTask->ParamIndNum(3, 0, m_pcNum + 1);
	DisplayTask->StringOut(24, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"->");
	DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
}

void MidiMapMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(blinkFlag)
			DisplayTask->ClearString(3, 0, Font::fntSystem, 3);
		else
			DisplayTask->ParamIndNum(3, 0, m_pcNum + 1);
	}
	else
	{
		if(blinkFlag)
			DisplayTask->ClearString(36, 0, Font::fntSystem, 3);
		else
			DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
	}
}

void MidiMapMenu::encoderPressed()
{
	if(m_encoderKnobSelected==0)
	{
		m_encoderKnobSelected = 1;
		DisplayTask->ParamIndNum(3, 0, m_pcNum + 1);
	}
	else
	{
		DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
		m_encoderKnobSelected = 0;
	}
	restartBlinking(0);
}

void MidiMapMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_pcNum < 127)
		{
			m_pcNum = BaseParam::encSpeedInc(m_pcNum, 127);
			DisplayTask->ParamIndNum(3, 0, m_pcNum + 1);
			DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
		}
	}
	else
	{
		if(sys_para[System::MIDI_MAP_START + m_pcNum] < 98)
		{
			sys_para[System::MIDI_MAP_START + m_pcNum] = BaseParam::encSpeedInc(sys_para[System::MIDI_MAP_START + m_pcNum], 98);
			DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
		}
	}

	restartBlinking(1);
}

void MidiMapMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_pcNum > 0)
		{
			m_pcNum = BaseParam::encSpeedDec(m_pcNum, 0);
			DisplayTask->ParamIndNum(3, 0, m_pcNum + 1);
			DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
		}
	}
	else
	{
		if(sys_para[System::MIDI_MAP_START + m_pcNum] > 0)
		{
			sys_para[System::MIDI_MAP_START + m_pcNum] = BaseParam::encSpeedDec(sys_para[System::MIDI_MAP_START + m_pcNum], 0);
			DisplayTask->ParamIndNum(36, 0, sys_para[System::MIDI_MAP_START + m_pcNum] + 1);
		}
	}

	restartBlinking(1);
}
