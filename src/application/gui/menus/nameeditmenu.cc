#include "nameeditmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"

const uint8_t ascii_low1     []= " abcdefghijklmnopqrst";
const uint8_t ascii_low2     []= "uvwxyz0123456789!@#$%";
const uint8_t ascii_hig1     []= "ABCDEFGHIJKLMNOPQRSTU";
const uint8_t ascii_hig2     []= "VWXYZ{}()-+_=<>?*.,/&";

NameEditMenu::NameEditMenu(AbstractMenu* parent)
{
	m_menuType = MENU_NAME_EDIT;
	topLevelMenu = parent;
}

void NameEditMenu::show(TShowMode swhoMode)
{
	current_menu = MENU_NAME_EDIT;
	currentMenu = this;
	// было своё copyMenu

	DisplayTask->Clear();

	keyShift = 0;
	symbolCursorPos = 0;
	nameCursorPos = 0;
	symbol = 32;
	encoderKnobPressed = 0;

	DisplayTask->StringOut(2, 0, TDisplayTask::fntSystem, 0, (uint8_t*)imya);
	DisplayTask->StringOut(2, 1, TDisplayTask::fntSystem, 0, (uint8_t*)imya1);
	DisplayTask->SymbolOut(86, 0, TDisplayTask::fntSystem, 0, 46);
	DisplayTask->SymbolOut(86, 1, TDisplayTask::fntSystem, 0, 46);
	DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low1);
	DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)ascii_low2);
}

void NameEditMenu::task()
{
	if(symbolCursorPos < 14) DisplayTask->SymbolOut(nameCursorPos*6+2, 0,TDisplayTask::fntSystem, tim5_fl*2-encoderKnobPressed, imya[nameCursorPos]);
	else DisplayTask->SymbolOut((nameCursorPos-14)*6+2 , 1,TDisplayTask::fntSystem, tim5_fl*2-encoderKnobPressed, imya1[(nameCursorPos-14)]);

	if(encoderKnobPressed)
	{
		if(symbolCursorPos < 21)
		{
			if(!keyShift) DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, tim5_fl, ascii_low1[symbolCursorPos]);
			else DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, tim5_fl, ascii_hig1[symbolCursorPos]);
		}
		else
		{
			if(!keyShift) DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, tim5_fl, ascii_low2[symbolCursorPos-21]);
			else DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, tim5_fl, ascii_hig2[symbolCursorPos-21]);
		}
	}
}

void NameEditMenu::encoderPressed()
{
	if(!encoderKnobPressed)
	{
		encoderKnobPressed = 1;
		if(nameCursorPos < 14)DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,1,imya[nameCursorPos]);
		else DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,1,imya1[nameCursorPos-14]);
	}
	else
	{
		encoderKnobPressed = 0;
		if(nameCursorPos < 14)
		{
			imya[nameCursorPos] = symbol;
			DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,0,imya[nameCursorPos]);
		}
		else
		{
			imya1[nameCursorPos-14] = symbol;
			DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[nameCursorPos-14]);
		}
		if(!keyShift)
		{
			if(symbolCursorPos < 21)DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_low1[symbolCursorPos]);
			else DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[symbolCursorPos -21]);
		}
		else
		{
			if(symbolCursorPos < 21)DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_hig1[symbolCursorPos]);
			else DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[symbolCursorPos -21]);
		}
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
			if(nameCursorPos > 13)
			{
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,0,imya1[(nameCursorPos++ -14)]);
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,2,imya1[(nameCursorPos -14)]);
			}

			if(nameCursorPos == 13)
			{
				DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,0,imya[nameCursorPos++]);
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,2,imya1[(nameCursorPos -14)]);
			}

			if(nameCursorPos < 13)
			{
				DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,0,imya[nameCursorPos++]);
				DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,2,imya[nameCursorPos]);
			}
		}
	}
	else
	{
		if(symbolCursorPos < 20)
		{
			if(!keyShift)
			{
				DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_low1[symbolCursorPos++]);
				DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_low1[symbolCursorPos]);
				symbol = ascii_low1[symbolCursorPos];
			}
			else
			{
				DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_hig1[symbolCursorPos++]);
				DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_hig1[symbolCursorPos]);
				symbol = ascii_hig1[symbolCursorPos];
			}
		}
		else
		{
			if(!keyShift)
			{
				if(symbolCursorPos == 41)
				{
					DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[symbolCursorPos - 21]);
					symbolCursorPos = 0;
					DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_low1[symbolCursorPos]);
					symbol = ascii_low1[symbolCursorPos];
				}
				else
				{
					if(symbolCursorPos == 20)DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_low1[symbolCursorPos++]);
					else DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[symbolCursorPos++ -21]);
					DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_low2[symbolCursorPos-21]);
					symbol = ascii_low2[symbolCursorPos - 21];
				}
			}
			else
			{
				if(symbolCursorPos == 41)
				{
					DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[symbolCursorPos - 21]);
					symbolCursorPos = 0;
					DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_hig1[symbolCursorPos]);
					symbol = ascii_hig1[symbolCursorPos];
				}
				else
				{
					if(symbolCursorPos == 20)DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem,0,ascii_hig1[symbolCursorPos++]);
					else DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[symbolCursorPos++ -21]);
					DisplayTask->SymbolOut((symbolCursorPos-21)*6,3,TDisplayTask::fntSystem,0,ascii_hig2[symbolCursorPos] -21);
					symbol = ascii_hig2[symbolCursorPos - 21];
				}
			}
		}
	}
}

void NameEditMenu::encoderCounterClockwise()
{
	if(!encoderKnobPressed)
	{
		if(nameCursorPos > 0)
		{
			if(nameCursorPos < 14)
			{
				DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 0, imya[nameCursorPos--]);
				DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 2, imya[nameCursorPos]);
			}

			if(nameCursorPos == 14)
			{
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1,TDisplayTask::fntSystem, 0, imya1[(nameCursorPos-- -14)]);
				DisplayTask->SymbolOut(nameCursorPos*6+2, 0, TDisplayTask::fntSystem, 2, imya[nameCursorPos]);
			}

			if(nameCursorPos > 14)
			{
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem, 0, imya1[(nameCursorPos-- -14)]);
				DisplayTask->SymbolOut((nameCursorPos-14)*6+2, 1, TDisplayTask::fntSystem, 2, imya1[(nameCursorPos -14)]);
			}
		}
	}
	else
	{
		if(symbolCursorPos < 21)
		{
			if(!keyShift)
			{
				if(symbolCursorPos == 0)
				{
					DisplayTask->SymbolOut((symbolCursorPos)*6, 2, TDisplayTask::fntSystem, 0, ascii_low1[symbolCursorPos]);
					symbolCursorPos = 41;
					DisplayTask->SymbolOut((symbolCursorPos - 21)*6, 3, TDisplayTask::fntSystem, 0 ,ascii_low2[symbolCursorPos - 21]);
					symbol = ascii_low2[symbolCursorPos - 21];
				}
				else
				{
					DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem, 0, ascii_low1[symbolCursorPos--]);
					DisplayTask->SymbolOut(symbolCursorPos*6,2,TDisplayTask::fntSystem, 0, ascii_low1[symbolCursorPos]);
					symbol = ascii_low1[symbolCursorPos];
				}
			}
			else
			{
				if(symbolCursorPos == 0)
				{
					DisplayTask->SymbolOut((symbolCursorPos)*6, 2, TDisplayTask::fntSystem, 0, ascii_hig1[symbolCursorPos]);
					symbolCursorPos = 41;
					DisplayTask->SymbolOut((symbolCursorPos - 21)*6, 3, TDisplayTask::fntSystem, 0, ascii_hig2[symbolCursorPos - 21]);
					symbol = ascii_hig2[symbolCursorPos - 21];
				}
				else
				{
					DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, 0, ascii_hig1[symbolCursorPos--]);
					DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, 0, ascii_hig1[symbolCursorPos]);
					symbol = ascii_hig1[symbolCursorPos];
				}
			}
		}
		else
		{
			if(symbolCursorPos > 20)
			{
				if(!keyShift)
				{
					if(symbolCursorPos == 21)
					{
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, ascii_low2[symbolCursorPos-- -21]);
						DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, 1, ascii_low1[symbolCursorPos]);
						symbol = ascii_low1[symbolCursorPos];
					}
					else
					{
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, ascii_low2[symbolCursorPos-- -21]);
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 1, ascii_low2[symbolCursorPos-21]);
						symbol = ascii_low2[symbolCursorPos - 21];
					}
				}
				else
				{
					if(symbolCursorPos == 21)
					{
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, ascii_hig2[symbolCursorPos-- -21]);
						DisplayTask->SymbolOut(symbolCursorPos*6, 2, TDisplayTask::fntSystem, 1, ascii_hig1[symbolCursorPos]);
						symbol = ascii_hig1[symbolCursorPos];
					}
					else
					{
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, ascii_hig2[symbolCursorPos-- -21]);
						DisplayTask->SymbolOut((symbolCursorPos-21)*6, 3, TDisplayTask::fntSystem, 0, ascii_hig2[symbolCursorPos-21]);
						symbol = ascii_hig2[symbolCursorPos - 21];
					}
				}
			}
		}
	}
}

void NameEditMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}

void NameEditMenu::keyDown()
{
	if(!encoderKnobPressed)
	{
		if(nameCursorPos < 14)
		{
			imya[nameCursorPos] = 32;
			DisplayTask->SymbolOut(nameCursorPos*6+2,0,TDisplayTask::fntSystem,1,imya[nameCursorPos]);
		}
		else
		{
			imya1[nameCursorPos-14] = 32;
			DisplayTask->SymbolOut((nameCursorPos-14)*6+2,1,TDisplayTask::fntSystem,1,imya1[nameCursorPos-14]);
		}
	}
	else
	{
		if(keyShift)
		{
			keyShift = 0;
			DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low1);
			DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_low2);
			if(symbolCursorPos < 21)symbol = ascii_low1[symbolCursorPos];
			else symbol = ascii_low2[symbolCursorPos-21];
		}
		else
		{
			keyShift = 1;
			DisplayTask->StringOut(0,2,TDisplayTask::fntSystem,0,(uint8_t*)ascii_hig1);
			DisplayTask->StringOut(0,3,TDisplayTask::fntSystem,0,(uint8_t*)ascii_hig2);
			if(symbolCursorPos < 21) symbol = ascii_hig1[symbolCursorPos];
			else symbol = ascii_hig2[symbolCursorPos-21];
		}
	}
	tim5_start(0);
	clean_flag();
}

void NameEditMenu::key1()
{
	topLevelMenu->key1();
}

void NameEditMenu::key2()
{

}

void NameEditMenu::key3()
{

}

void NameEditMenu::key4()
{

}

void NameEditMenu::key5()
{

}
