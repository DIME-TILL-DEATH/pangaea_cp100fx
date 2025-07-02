#include "midimapmenu.h"

#include "cs.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

MidiMapMenu::MidiMapMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MIDI_PC;
}

void MidiMapMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	DisplayTask->ParamIndicNum(3, 0, m_pcNum + 1);
	DisplayTask->StringOut(24, 0, TDisplayTask::fntSystem, 0, (uint8_t*)"->");
	DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
}

void MidiMapMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(blinkFlag_fl)
			DisplayTask->Clear_str(3, 0, TDisplayTask::fntSystem, 3);
		else
			DisplayTask->ParamIndicNum(3, 0, m_pcNum + 1);
	}
	else
	{
		if(blinkFlag_fl)
			DisplayTask->Clear_str(36, 0, TDisplayTask::fntSystem, 3);
		else
			DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
	}
}

void MidiMapMenu::encoderPressed()
{
	if(m_encoderKnobSelected==0)
	{
		m_encoderKnobSelected = 1;
		DisplayTask->ParamIndicNum(3, 0, m_pcNum + 1);
	}
	else
	{
		DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
		m_encoderKnobSelected = 0;
	}
	tim5_start(0);
}

void MidiMapMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_pcNum < 127)
		{
			m_pcNum = enc_speed_inc(m_pcNum, 127);
			DisplayTask->ParamIndicNum(3, 0, m_pcNum + 1);
			DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
		}
	}
	else
	{
		if(sys_para[MIDI_MAP_START + m_pcNum] < 98)
		{
			sys_para[MIDI_MAP_START + m_pcNum] = enc_speed_inc(sys_para[MIDI_MAP_START + m_pcNum], 98);
			DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
		}
	}

	tim5_start(1);
}

void MidiMapMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_pcNum > 0)
		{
			m_pcNum = enc_speed_dec(m_pcNum, 0);
			DisplayTask->ParamIndicNum(3, 0, m_pcNum + 1);
			DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
		}
	}
	else
	{
		if(sys_para[MIDI_MAP_START + m_pcNum] > 0)
		{
			sys_para[MIDI_MAP_START + m_pcNum] = enc_speed_dec(sys_para[MIDI_MAP_START + m_pcNum], 0);
			DisplayTask->ParamIndicNum(36, 0, sys_para[MIDI_MAP_START + m_pcNum] + 1);
		}
	}

	tim5_start(1);
}
