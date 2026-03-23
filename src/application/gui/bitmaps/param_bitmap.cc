#include <bitmaps.h>
#include <param_bitmap.h>

const uint8_t dry[] = "Dry";
const uint8_t wet[] = "Wet";
const uint8_t dw_[] = "D+W";

void param_ind(uint8_t col, uint8_t pag, uint32_t val)
{
	LCD_SetColumnAddress(col);
	LCD_SetPageAddress(pag);
	GPIO_ResetBits(GPIOB, CS);
	uint8_t width = 33;
	for(uint8_t i = 0; i<width; i++)
	{
		if(((val>>2)>=i)||(i==0)||(i==width-1))
			LCD_WriteData(0x7e);
		else
			LCD_WriteData(0x42);
	}
	GPIO_SetBits(GPIOB, CS);
	param_ind_num_(col+width+1, pag, val);
}

void param_ind_num_(uint8_t col, uint8_t pag, uint16_t val)
{
	uint8_t prog_num[3];
	uint8_t a;
	Arsys_clean_(col, pag, 3);
	prog_num[0] = val/100;
	prog_num[1] = (val%100)/10;
	prog_num[2] = (val%100)%10;
	if(val>99)
		a = 0;
	else
	{
		if(val>9)
			a = 1;
		else
			a = 2;
	}
	col += (a*3);
	for(uint8_t i = a; i<3; i++)
		col = Arsys_sym(col, pag, prog_num[i]+48, 0);
}

void param_ind_note(uint8_t col, uint8_t pag, uint8_t note)
{
//	kgp_sdk_libc::memset(str, 0, 8);

	const char* note_list[12] =
	{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

	const char* noteChar = note_list[note % 12];
	int8_t noteNum;

	std::emb_string str;

	if(note < 12)
		noteNum = -2;
	else if(note >= 12 && note < 24)
		noteNum = -1;
	else
		noteNum = note / 12 - 2;

	char noteStr[6];
	ksprintf(noteStr, "%s%d", noteChar, noteNum);

	uint8_t i = 0;
	while(noteStr[i])
		col = Arsys_sym(col, pag, noteStr[i++], 0);
}

uint8_t ind_num_pan(uint8_t col, uint8_t pag, uint8_t val)
{
	uint8_t prog_num[2];
	uint8_t a;
	uint8_t b;
	if(val<63)
	{
		b = 0;
		val = 63-val;
	}
	else
	{
		b = 1;
		val = val-63;
		if(!val)
			b = 2;
	}
	Arsys_clean_(col, pag, 3);
	prog_num[0] = val/10;
	prog_num[1] = val%10;
	if(val<10)
		a = 1;
	else
		a = 0;
	col += (a*3+3);
	for(uint8_t i = a; i<2; i++)
		col = Arsys_sym(col, pag, prog_num[i]+48, 0);
	return b;
}

void param_transparent_ind(uint8_t col, uint8_t pag, uint32_t val)
{
	uint8_t val1 = val>>2;
	LCD_SetColumnAddress(col);
	LCD_SetPageAddress(pag);
	for(uint8_t i = 0; i<32; i++)
	{
		if(((val1+1)==i)||(i==0))
			LCD_WriteData(0x7e);
		else
			LCD_WriteData(0x42);
	}
	LCD_SetColumnAddress(col+31);

	if((val1+1)==32)
		LCD_WriteData(0x7e);
	else
		LCD_SetColumnAddress(col+32);
	LCD_WriteData(0x7e);
}

void param_ind_pan(uint8_t col, uint8_t pag, uint32_t val)
{
	param_transparent_ind(col, pag, val);

	switch(ind_num_pan(col, pag, val))
	{
		case 0:
			Arsys_sym(col-40, pag, 76, 0);
		break;
		case 1:
			Arsys_sym(col-40, pag, 82, 0);
		break;
		case 2:
			Arsys_sym(col-40, pag, 32, 0);
		break;
	}
}

void param_ind_mix(uint8_t col, uint8_t pag, uint32_t val)
{
	param_transparent_ind(col, pag, val);

	switch(ind_num_pan(col, pag, val))
	{
		case 0:
			if(col<98)
				Arsys_line(col-54, pag, (uint8_t*)dry, 0);
		break;
		case 1:
			if(col<98)
				Arsys_line(col-54, pag, (uint8_t*)wet, 0);
		break;
		case 2:
			if(col<98)
				Arsys_line(col-54, pag, (uint8_t*)dw_, 0);
		break;
	}
}
