#include "font.h"

#include "lcd.h"

#include "SystemFont5x7.h"

uint8_t Font::symbolWidth(Font::TFontName fontName)
{
	switch(fontName)
	{
		case Font::fntSystem:
			return FONT_SYSTEM5X7_WIDTH;
		default:
			return FONT_SYSTEM5X7_WIDTH;
	}
}


uint8_t Font::symbolsOnLine(Font::TFontName fontName)
{
	switch(fontName)
	{
		case Font::fntSystem:
			return LCD_SCREEN_WIDTH / FONT_SYSTEM5X7_WIDTH;
		default:
			return LCD_SCREEN_WIDTH / FONT_SYSTEM5X7_WIDTH;
	}
}
