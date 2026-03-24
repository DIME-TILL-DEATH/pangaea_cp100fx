#include "bitmaps.h"

#include "periphery.h"
#include "serial.h"

#include "preset.h"
#include "footswitch.h"
#include "system.h"

#include "amt.h"
#include "bitmaps.h"

#include "display_task.h"

uint8_t Font::symbolWidth(Font::TFontName fontName)
{
	switch(fontName)
	{
		case Font::fntSystem:
			return 6;
		default:
			return 6;
	}
}

void display_start(uint8_t num)
{
	uint16_t a = 0;
	if(!num)
	{
		for(uint8_t j = 0; j < 4; j++)
		{
			LCD_SetPageAddress(j);
			LCD_SetColumnAddress(27);
			GPIO_ResetBits(GPIOB, CS);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			GPIO_SetBits(GPIOB, RS);
			for(uint8_t i = 0; i < 75; i++)
			{
				LCD_WriteData(amt_logo[a++]);
				if(a > 299)
					break;
			}
			GPIO_SetBits(GPIOB, CS);
		}
	}
	else
	{
		uint8_t nullData = 0;
		for(uint8_t i = 0; i < 128; i++)
		{
			LCD_SetColumnAddress(i);
			for(uint8_t j = 0; j < 4; j++)
			{
				LCD_SetPageAddress(j);
				LCD_WriteData(nullData);
			}

			HW_Delay(0x1ffff);
		}
#ifdef __MONO_MOD__
		Arsys_line(35, 1, (uint8_t*)"CP100-FX", 0);
#endif

#ifdef __STEREO_MOD__
		Arsys_line(29, 1, (uint8_t*)"CP100-FX-S", 0);
#endif
		Arsys_line(38, 2, (uint8_t*)amt_ver, 0);
	}
}


void tap_ind(uint8_t cur)
{
	LCD_SetPageAddress(3);
	LCD_SetColumnAddress(90);
	for(uint8_t i = 0; i < 19; i++)
	{
		if(cur == 1)
			LCD_WriteData(0xc0);
		else
			LCD_WriteData(0);
	}
	LCD_SetPageAddress(4);
	LCD_SetColumnAddress(90);
	if(cur == 1)
	{
		LCD_WriteData(0xff);
		LCD_WriteData(0xff);
	}
	else
	{
		LCD_WriteData(0);
		LCD_WriteData(0);
	}
	LCD_SetPageAddress(5);
	LCD_SetColumnAddress(90);
	if(cur == 1)
	{
		LCD_WriteData(7);
		LCD_WriteData(7);
	}
	else
	{
		LCD_WriteData(0);
		LCD_WriteData(0);
	}
	LCD_SetColumnAddress(107);
	if(cur == 1)
	{
		LCD_WriteData(7);
		LCD_WriteData(7);
	}
	else
	{
		LCD_WriteData(0);
		LCD_WriteData(0);
	}
	LCD_SetPageAddress(4);
	LCD_SetColumnAddress(107);
	if(cur == 1)
	{
		LCD_WriteData(0xff);
		LCD_WriteData(0xff);
	}
	else
	{
		LCD_WriteData(0);
		LCD_WriteData(0);
	}
}
void mode_ind(uint8_t val)
{
	uint8_t a1, a2;
	LCD_SetPageAddress(0);
	switch(val)
	{
		case 0:
			a1 = a2 = 0x7;
		break;
		case 1:
			a1 = 0;
			a2 = 0x7;
		break;
		case 2:
			a1 = a2 = 0;
		break;
	}
	LCD_SetColumnAddress(2);
	LCD_WriteData(a1, 15);

	LCD_SetColumnAddress(38);
	LCD_WriteData(a2, 15);

	LCD_SetColumnAddress(74);
	LCD_WriteData(0x7, 15);

}

void checkbox(uint8_t x, uint8_t y, uint8_t val)
{
	LCD_SetPageAddress(y);
	LCD_SetColumnAddress(x);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 7; i++)
	{
		if(!i || (i == 6))
			LCD_WriteData(0x7c);
		else
			LCD_WriteData((val * 0xfe) | 0x82);
	}
	GPIO_SetBits(GPIOB, CS);
}

void ind_foot_proc(uint8_t col, uint8_t val)
{
	LCD_SetPageAddress(3);
	LCD_SetColumnAddress(col);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 7; i++)
	{
		if(!i || (i == 6))
			LCD_WriteData(0x7c);
		else
			LCD_WriteData((val * 0xfe) | 0x82);
	}
	GPIO_SetBits(GPIOB, CS);
}

void fsw_ind(uint8_t num, uint8_t pressState, uint8_t holdState)
{
	struct{
		uint8_t pressSinglePos;
		uint8_t pressDualPos;
		uint8_t holdDualPos;
	}indPos[3] = {
			{0, 0, 7},
			{57, 60, 64},
			{113, 113, 120}
	};

	if((sys_para[System::FSW1_PRESS_TYPE + num] == Footswitch::FswType::Controller))
	{
		if(sys_para[System::FSW1_MODE + num] == Footswitch::FswMode::Double)
			ind_foot_proc(indPos[num].pressSinglePos, pressState);
		else
			ind_foot_proc(indPos[num].pressDualPos, pressState);
	}

	if(sys_para[System::FSW1_MODE + num] == Footswitch::FswMode::Double
				&& (sys_para[System::FSW1_HOLD_TYPE + num] == Footswitch::FswType::Controller))
		ind_foot_proc(indPos[num].holdDualPos, holdState);
}

void preset_ind(uint32_t val, bool filled)
{
	val++;
	uint8_t col = 87;
	uint8_t prog_num[2];
	LCD_SetPageAddress(0);
	t33x30_clear(col, 0, 39);
	prog_num[0] = val / 10;
	prog_num[1] = val % 10;
	for(uint8_t i = 0; i < 2; i++)
		col += t33x30_sym(col, 0, prog_num[i], filled);
}

void clear_string(uint8_t col, uint8_t pag, uint8_t font, uint8_t count)
{
	switch(font)
	{
		case 0:
			t12x13_clear(col, pag, count);
		break;
		case 1:
			t33x30_clear(col, pag, count);
		break;
		case 2:
			Arsys_clean_(col, pag, count);
		break;
	}
}

void effect_icon(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs)
{
	uint8_t fl = 0;
	uint32_t sym;
	sym = adr[0];
	sym = (sym - 32) * 6;
	for(uint8_t i = 0; i < 8; i++)
	{
		LCD_SetColumnAddress(col + i);
		LCD_SetPageAddress(pag);
		if(i > 1)
		{
			if((SystemFont5x7[sym + i - 2] & 0xc0) == 0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					LCD_WriteData((SystemFont5x7[sym + i - 2] << 2) | 1);
				break;
				case 1:
					LCD_WriteData((~(SystemFont5x7[sym + i - 2] << 2)) | 3);
					if(fl == 1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					LCD_WriteData(1);
					fl = 0;
				break;
			}
		}
		if((i == 1) && (curs != 1))
			LCD_WriteData(1);
		else
			LCD_WriteData(0xff);
		LCD_SetColumnAddress(col + i);
		LCD_SetPageAddress(pag + 1);
		if(i != 0)
		{
			if(i == 1)
			{
				if(curs != 1)
					LCD_WriteData(4);
				else
					LCD_WriteData(7);
			}
			else
			{
				if(curs != 1)
					LCD_WriteData(4 + fl);
				else
					LCD_WriteData((4 + fl) | 2);
			}
		}
		else
			LCD_WriteData(7);
	}
	sym = adr[1];
	sym = (sym - 32) * 6;
	for(uint8_t i = 0; i < 7; i++)
	{
		LCD_SetColumnAddress(col + i + 8);
		LCD_SetPageAddress(pag);
		if(i < 6)
		{
			if((SystemFont5x7[sym + i] & 0xc0) == 0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					LCD_WriteData((SystemFont5x7[sym + i] << 2) | 1);
				break;
				case 1:
					LCD_WriteData((~(SystemFont5x7[sym + i] << 2)) | 3);
					if(fl == 1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					LCD_WriteData(1);
					fl = 0;
				break;
			}
		}
		if(i == 6)
			LCD_WriteData(0xff);
		LCD_SetColumnAddress(col + i + 8);
		LCD_SetPageAddress(pag + 1);
		if(i != 6)
		{
			if(curs != 1)
				LCD_WriteData(4 + fl);
			else
				LCD_WriteData((4 + fl) | 2);
		}
		else
			LCD_WriteData(7);
	}
}

void del_sec_ind(uint8_t col, uint8_t pag, uint32_t d)
{
	uint16_t t3[4];
	t3[0] = d / 1000;
	t3[2] = d % 1000;
	t3[1] = t3[2] / 100;
	t3[3] = t3[2] % 100;
	t3[2] = t3[3] / 10;
	t3[3] = t3[3] % 10;
	LCD_SetColumnAddress(col);
	LCD_SetPageAddress(pag);
	col = Arsys_sym(col, pag, t3[0] + 48, 0);
	col = Arsys_sym(col, pag, 44, 0);
	for(uint8_t i = 1; i < 4; i++)
		col = Arsys_sym(col, pag, t3[i] + 48, 0);
	col = Arsys_sym(col, pag, 115, 0);
}

void delay_time_ind(uint8_t col, uint8_t pag, uint32_t d)
{
	if(!sys_para[System::TIME_FORMAT])
		del_sec_ind(col, pag, d);
	else
	{
		Arsys_line(90, pag, (uint8_t*)"bpm", 0);
		LCD_SetColumnAddress(col);
		LCD_SetPageAddress(pag);
		uint16_t t3[4];
		volatile float a = 60.0f / (d * 0.001f);
		uint16_t b = a;
		if(a >= 1000.0f)
		{
			t3[0] = b / 1000;
			t3[2] = b % 1000;
			t3[1] = t3[2] / 100;
			t3[3] = t3[2] % 100;
			t3[2] = t3[3] / 10;
			t3[3] = t3[3] % 10;
			for(uint8_t i = 0; i < 4; i++)
				col = Arsys_sym(col, pag, t3[i] + 48, 0);
			col = Arsys_sym(col, pag, 44, 0);
			return;
		}
		if(a >= 100.0f)
		{
			a = (a - b) * 10.0f;
			t3[0] = b / 100;
			t3[2] = b % 100;
			t3[1] = t3[2] / 10;
			t3[2] = t3[2] % 10;
			for(uint8_t i = 0; i < 3; i++)
				col = Arsys_sym(col, pag, t3[i] + 48, 0);
			col = Arsys_sym(col, pag, 44, 0);
			Arsys_sym(col, pag, a + 48, 0);
			return;
		}
		a = (a - b) * 100.0f;
		t3[0] = b / 10;
		t3[1] = b % 10;
		for(uint8_t i = 0; i < 2; i++)
			col = Arsys_sym(col, pag, t3[i] + 48, 0);
		col = Arsys_sym(col, pag, 44, 0);
		b = a;
		t3[0] = b / 10;
		t3[1] = b % 10;
		for(uint8_t i = 0; i < 2; i++)
			col = Arsys_sym(col, pag, t3[i] + 48, 0);
	}
}

void progress_bar(uint8_t col, uint8_t pag, uint32_t val)
{
	LCD_SetColumnAddress(col);
	LCD_SetPageAddress(pag);
	GPIO_ResetBits(GPIOB, CS);
	uint8_t width = 50;
	for(uint8_t i = 0; i<50; i++)
	{
		if(((val/2)>=i)||(i==0)||(i==width-1))
			LCD_WriteData(0x7e);
		else
			LCD_WriteData(0x42);
	}
	GPIO_SetBits(GPIOB, CS);
}

void vol_indicator(uint8_t xPos, uint8_t indLength, TVolIndicatorType volIndicatorType, uint8_t* volIndPar_ptr)
{
	uint8_t volIndPosition = 32;

	if(volIndPar_ptr) volIndPosition = (*volIndPar_ptr >> 1) + 1;
	else volIndPosition = 0;

	uint8_t outLevel;

	if(ind_out_l[1] < 8388000)
		outLevel = /*vsqrt*/(ind_out_l[1]) * ((float)(indLength - 1) / (8384000.0f));
	else
		outLevel = indLength - 1;

	ind_out_l[1] = 0;

	LCD_SetColumnAddress(xPos);
	LCD_SetPageAddress(3);

	for(uint8_t i = 0; i < indLength; i++)
	{
		if(volIndicatorType == TVolIndicatorType::VOL_INDICATOR_VOLUME && i >= volIndPosition - 2 && i <= volIndPosition)
		{
			if(i == volIndPosition - 1) LCD_WriteData(0x42);
			else LCD_WriteData(0x3c);
		}
		else
		{
			if((i == indLength - 1) || (i == 0))
			{
				LCD_WriteData(0x7e, 1); //3c
			}
			else
			{
				if(i < outLevel) LCD_WriteData(0x3c);
				else LCD_WriteData(0x0);
			}
		}
	}
}

