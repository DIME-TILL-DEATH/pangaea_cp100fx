#include "tapmenu.h"

#include "system.h"
#include "preset.h"

#include "display_task.h"

#include "bitmaps.h"

TapMenu::TapMenu(AbstractMenu* parent, System::TapDestination tapDst)
{
	m_menuType = MENU_TAP_SCREEN;
	topLevelMenu = parent;
	m_tapDst = tapDst;
	m_delay = 4;
}

void TapMenu::show(TShowMode showMode)
{
	currentMenu = this;
	System::TapTempo(m_tapDst);
	m_delay = 4;
	restartBlinking(0);
}

void TapMenu::refresh()
{
	DisplayTask->Clear();

	char string[24];
	if(Preset::moog_time != 0) ksprintf(string, "RF LFO: %d", Preset::moog_time);
	else ksprintf(string, "RF LFO: --");
	DisplayTask->StringOut(Font::symbolWidth(Font::fntSystem) * 2, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)string);

	ksprintf(string, "Delay time: %d", currentPreset.delayTime);
	DisplayTask->StringOut(Font::symbolWidth(Font::fntSystem) * 2, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)string);

	if(Preset::trem_time !=0) ksprintf(string, "Tremolo time: %d", Preset::trem_time);
	else ksprintf(string, "Tremolo time: --");
	DisplayTask->StringOut(Font::symbolWidth(Font::fntSystem) * 2, 2, Font::fntSystem, Font::fnsNormal, (uint8_t*)string);

	if(currentPreset.paramData.bpm > 0)
	{
		ksprintf(string, "(%d BPM)", currentPreset.paramData.bpm);
		DisplayTask->StringOut(Font::symbolWidth(Font::fntSystem) * 6, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)string);
	}
}


void TapMenu::task()
{
	if(m_delay == 4)
	{
		TapMenu::refresh();
	}

	if(m_delay > 0)
	{
		m_delay--;
	}
	else
	{
		topLevelMenu->returnFromChildMenu();
	}
}

void TapMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu();
}

void TapMenu::keyDown()
{
	System::TapTempo(m_tapDst);
	m_delay = 4;
	restartBlinking(0);
}
