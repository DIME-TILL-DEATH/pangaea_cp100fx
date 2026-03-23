#include <bitmaps.h>
#include "SystemFont5x7.h"


uint32_t Arsys_sym(uint8_t col, uint8_t pag, uint16_t sym, uint8_t curs)
{
	uint8_t j;
	if(sym & 0x80)
	{
		sym &= ~0x80;
		curs = 2;
	}
	sym = (sym - 32) * 6;
	LCD_SetPageAddress(pag);
	LCD_SetColumnAddress(col);
	for(uint8_t i = 0; i < 6; i++)
	{
		j = SystemFont5x7[sym + i];
		if(curs == 1) j |= 0x80;
		if(curs == 2) j = (~j) & 0x7f;
		if(curs == 3) j = 0;

		LCD_WriteData(j);
	}
	return col + 6;
}

uint32_t Arsys_sym_up(uint8_t col, uint8_t pag, uint16_t sym, uint8_t curs)
{
	uint8_t j;
	sym = (sym - 32) * 6;
	LCD_SetPageAddress(pag);
	LCD_SetColumnAddress(col);
	for(uint8_t i = 0; i < 6; i++)
	{
		j = SystemFont5x7[sym + i] << 4;
		if(curs == 1)
			j |= 0x80;
		if(curs == 2)
			j = ~j;
		LCD_WriteData(j);
	}
	return col + 6;
}

uint32_t Arsys_sym_down(uint8_t col, uint8_t pag, uint16_t sym, uint8_t curs)
{
	uint8_t j;
	sym = (sym - 32) * 6;
	LCD_SetPageAddress(pag);
	LCD_SetColumnAddress(col);
	for(uint8_t i = 0; i < 6; i++)
	{
		j = SystemFont5x7[sym + i] >> 4;
		if(curs == 1)
			j |= 0x80;
		if(curs == 2)
			j = ~j;
		LCD_WriteData(j);
	}
	return col + 6;
}

void Arsys_line(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs)
{
	uint8_t i = 0;
	while(adr[i])
	{
		col = Arsys_sym(col, pag, adr[i++], curs);
		if(col > 121)
		{
			col = 4;
			pag++;
		}
	}
}

void Arsys_clean(uint8_t col, uint8_t pag, uint8_t *adr)
{
	uint8_t i = 0;
	while(adr[i])
	{
		col = Arsys_sym(col, pag, 32, 0);
		i++;
	}
}

void Arsys_clean_(uint8_t col, uint8_t pag, uint8_t size)
{
	for(uint8_t i = 0; i < size; i++)
	{
		col = Arsys_sym(col, pag, 32, 0);
	}
}
