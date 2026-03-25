#include "font.h"

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
