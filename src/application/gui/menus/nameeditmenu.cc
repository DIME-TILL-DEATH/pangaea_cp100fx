#include "nameeditmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"

NameEditMenu::NameEditMenu(AbstractMenu* parent)
{
	m_menuType = MENU_NAME_EDIT;
	topLevelMenu = parent;
}

void NameEditMenu::show(TShowMode swhoMode)
{
	current_menu_type = MENU_NAME_EDIT;
	currentMenu = this;
	// было своё copyMenu

	DisplayTask->Clear();

	keyShift = 0;
	symbolCursorPos = 0;
	nameCursorPos = 0;
	symbol = 32;
	encoderKnobPressed = 0;

	DisplayTask->StringOut(2, 0, TDisplayTask::fntSystem, 0, (uint8_t*)presetName);
	DisplayTask->StringOut(2, 1, TDisplayTask::fntSystem, 0, (uint8_t*)presetComment);
	DisplayTask->SymbolOut(86, 0, TDisplayTask::fntSystem, 0, 46);
	DisplayTask->SymbolOut(86, 1, TDisplayTask::fntSystem, 0, 46);
	DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low);
	DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low + chartStringLength);
}

void NameEditMenu::task()
{
	if(nameCursorPos < 14)
	{
		DisplayTask->SymbolOut(nameCursorPos*6+2, 0,TDisplayTask::fntSystem, tim5_fl*2-encoderKnobPressed, presetName[nameCursorPos]);
	}
	else
	{
		DisplayTask->SymbolOut((nameCursorPos-14)*6+2 , 1,TDisplayTask::fntSystem, tim5_fl*2-encoderKnobPressed, presetComment[(nameCursorPos-14)]);
	}

	if(encoderKnobPressed)
	{
		const uint8_t* symbolChart;
		if(!keyShift) symbolChart = ascii_low;
		else symbolChart = ascii_high;

		DisplayTask->SymbolOut((symbolCursorPos % chartStringLength)*6, 2 + (symbolCursorPos / chartStringLength),
										TDisplayTask::fntSystem, tim5_fl, symbolChart[symbolCursorPos]);
	}
}

void NameEditMenu::encoderPressed()
{
	if(!encoderKnobPressed)
	{
		encoderKnobPressed = 1;
		if(nameCursorPos < 14) DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 1, presetName[nameCursorPos]);
		else DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem, 1, presetComment[nameCursorPos-14]);
	}
	else
	{
		encoderKnobPressed = 0;
		if(nameCursorPos < 14)
		{
			presetName[nameCursorPos] = symbol;
			DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 0, presetName[nameCursorPos]);
		}
		else
		{
			presetComment[nameCursorPos-14] = symbol;
			DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem, 0, presetComment[nameCursorPos-14]);
		}

		const uint8_t* symbolChart;
		if(!keyShift) symbolChart = ascii_low;
		else symbolChart = ascii_high;

		DisplayTask->SymbolOut((symbolCursorPos % chartStringLength)*6, 2 + (symbolCursorPos / chartStringLength),
										TDisplayTask::fntSystem, 0, symbolChart[symbolCursorPos]);
	}
	tim5_start(0);
	clean_flag();
}

void NameEditMenu::encoderClockwise()
{
    if(!encoderKnobPressed)
	{
		if(nameCursorPos < 27)
		{
			if(nameCursorPos < 14) DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 0, presetName[nameCursorPos++]);
			else DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem, 0, presetComment[(nameCursorPos++ -14)]);
		}
	}
	else
	{
		const uint8_t* symbolChart;
		if(!keyShift) symbolChart = ascii_low;
		else symbolChart = ascii_high;

		if(symbolCursorPos == chartStringLength*2 - 1)
		{
			DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, symbolChart[symbolCursorPos]);
			symbolCursorPos = 0;
		}
		else
		{
			DisplayTask->SymbolOut((symbolCursorPos % chartStringLength)*6, 2 + (symbolCursorPos / chartStringLength),
													TDisplayTask::fntSystem, 0, symbolChart[symbolCursorPos++]);
		}
		symbol = symbolChart[symbolCursorPos];
	}

    tim5_start(0);
    clean_flag();
}

void NameEditMenu::encoderCounterClockwise()
{
	if(!encoderKnobPressed)
	{
		if(nameCursorPos > 0)
		{
			if(nameCursorPos < 14) DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 0, presetName[nameCursorPos--]);
			else DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1,TDisplayTask::fntSystem, 0, presetComment[(nameCursorPos-- -14)]);
		}
	}
	else
	{
		const uint8_t* symbolChart;
		if(!keyShift) symbolChart = ascii_low;
		else symbolChart = ascii_high;

		if(symbolCursorPos == 0)
		{
			symbolCursorPos = chartStringLength*2 - 1;
			DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, symbolChart[symbolCursorPos]);
		}
		else
		{
			DisplayTask->SymbolOut((symbolCursorPos % chartStringLength)*6, 2 + (symbolCursorPos / chartStringLength),
													TDisplayTask::fntSystem, 0, symbolChart[symbolCursorPos--]);
		}
		symbol = symbolChart[symbolCursorPos];
	}

	tim5_start(0);
	clean_flag();
}

void NameEditMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);

	clean_flag();
}

void NameEditMenu::keyDown()
{
	if(!encoderKnobPressed)
	{
		if(nameCursorPos < 14)
		{
			presetName[nameCursorPos] = 32;
			DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem,1, presetName[nameCursorPos]);
		}
		else
		{
			presetComment[nameCursorPos-14] = 32;
			DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem,1, presetComment[nameCursorPos-14]);
		}
	}
	else
	{
		if(keyShift)
		{
			keyShift = 0;
			DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low);
			DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low + chartStringLength);
			symbol = ascii_low[symbolCursorPos];
		}
		else
		{
			keyShift = 1;
			DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_high);
			DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_high + chartStringLength);
			symbol = ascii_high[symbolCursorPos];
		}
	}

	tim5_start(0);
	clean_flag();
}

void NameEditMenu::key1()
{
	topLevelMenu->key1();
	clean_flag();
}

void NameEditMenu::key2()
{
	topLevelMenu->key2();
	clean_flag();
}

void NameEditMenu::key3()
{
	topLevelMenu->key3();
	clean_flag();
}

void NameEditMenu::key4()
{
	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
	clean_flag();
}

void NameEditMenu::key5()
{
	topLevelMenu->key5();
	clean_flag();
}
