#include "appdefs.h"
#include "allFonts.h"

#include "gui_task.h"
#include "fs.h"
#include "eepr.h"
#include "cs.h"
#include "amt.h"

#include "preset.h"

uint32_t ind_in_p[2];
uint32_t ind_out_l[2];

uint8_t imya_temp;

uint8_t t_po;
uint8_t t_po1;
volatile uint8_t t_no;

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

void disp_start(uint8_t num)
{
	uint16_t a = 0;
	if(!num)
	{
		for(uint8_t j = 0; j < 4; j++)
		{
			Set_Page_Address(j);
			Set_Column_Address(27);
			GPIO_ResetBits(GPIOB, CS);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			GPIO_SetBits(GPIOB, RS);
			for(uint8_t i = 0; i < 75; i++)
			{
				oled023_1_send_data(amt_logo[a++]);
				if(a > 299)
					break;
			}
			GPIO_SetBits(GPIOB, CS);
		}
		//for(uint32_t d = 0 ; d < 0x1ffffff ; d++)NOP();
	}
	else
	{
		for(uint8_t i = 0; i < 128; i++)
		{
			Set_Column_Address(i);
			for(uint8_t j = 0; j < 4; j++)
			{
				Set_Page_Address(j);
				oled023_1_write_data(0);
			}
			for(uint32_t d = 0; d < 0xffff; d++)
				NOP();
		}
		Arsys_line(50, 2, (uint8_t*)amt_ver, 0);
		for(uint32_t d = 0; d < 0x7fffff; d++)
			NOP();
		for(uint8_t i = 127; i > 0; i--)
		{
			Set_Column_Address(i);
			for(uint8_t j = 0; j < 4; j++)
			{
				Set_Page_Address(j);
				oled023_1_write_data(0);
			}
			for(uint32_t d = 0; d < 0xffff; d++)
				NOP();
		}
		GPIO_SetBits(GPIOB, CS);
	}
	extern volatile uint8_t ind_en;
	ind_en = 1;
}


void tap_ind(uint8_t cur)
{
	Set_Page_Address(3);
	Set_Column_Address(90);
	for(uint8_t i = 0; i < 19; i++)
	{
		if(cur == 1)
			oled023_1_write_data(0xc0);
		else
			oled023_1_write_data(0);
	}
	Set_Page_Address(4);
	Set_Column_Address(90);
	if(cur == 1)
	{
		oled023_1_write_data(0xff);
		oled023_1_write_data(0xff);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Page_Address(5);
	Set_Column_Address(90);
	if(cur == 1)
	{
		oled023_1_write_data(7);
		oled023_1_write_data(7);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Column_Address(107);
	if(cur == 1)
	{
		oled023_1_write_data(7);
		oled023_1_write_data(7);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Page_Address(4);
	Set_Column_Address(107);
	if(cur == 1)
	{
		oled023_1_write_data(0xff);
		oled023_1_write_data(0xff);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
}
void mode_ind(uint8_t val)
{
	uint8_t a1, a2;
	Set_Page_Address(0);
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
	Set_Column_Address(2);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 15; i++)
		oled023_1_send_data(a1);
	GPIO_SetBits(GPIOB, CS);
	Set_Column_Address(38);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 15; i++)
		oled023_1_send_data(a2);
	GPIO_SetBits(GPIOB, CS);
	Set_Column_Address(74);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 15; i++)
		oled023_1_send_data(0x7);
	GPIO_SetBits(GPIOB, CS);
}

extern uint8_t fsw1_in_fl1;
extern uint8_t fsw2_in_fl1;
extern uint8_t fsw3_in_fl1;
extern uint8_t contr_kn[];
extern uint8_t contr_kn1[];
inline void ind_foot_proc(uint8_t col, uint8_t val)
{
	Set_Page_Address(3);
	Set_Column_Address(col);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i < 7; i++)
	{
		if(!i || (i == 6))
			oled023_1_send_data(0x7c);
		else
			oled023_1_send_data((val * 0xfe) | 0x82);
	}
	GPIO_SetBits(GPIOB, CS);
}

void ind_foot(uint8_t num, uint8_t val)
{
	switch(num)
	{
		case 0:
			if(!fsw1_in_fl1 && (sys_para[FSW1_PRESS_TYPE] == 1))
				ind_foot_proc(0, contr_kn[0]);
			if(sys_para[FSW1_MODE] && (sys_para[FSW1_HOLD_TYPE] == 1))
				ind_foot_proc(7, contr_kn1[0]);
		break;
		case 1:
			if(!fsw2_in_fl1 && (sys_para[FSW2_PRESS_TYPE] == 1))
			{
				if(sys_para[FSW2_MODE])
					ind_foot_proc(57, contr_kn[1]);
				else
					ind_foot_proc(60, contr_kn[1]);
			}
			if(sys_para[FSW2_MODE] && (sys_para[FSW2_HOLD_TYPE] == 1))
				ind_foot_proc(64, contr_kn1[1]);
		break;
		case 2:
			if(!fsw3_in_fl1 && (sys_para[FSW3_PRESS_TYPE] == 1))
			{
				if(sys_para[FSW3_MODE])
					ind_foot_proc(113, contr_kn[2]);
				else
					ind_foot_proc(120, contr_kn[2]);
			}
			if(sys_para[FSW3_MODE] && (sys_para[FSW3_HOLD_TYPE] == 1))
				ind_foot_proc(120, contr_kn1[2]);
		break;
	}
}

const uint8_t inpp[] = "In";
const uint8_t outt[] = "Out";
//void main_screen(void)
//{
//	oled023_1_disp_clear();
////	ind_poin = 0;
//	Arsys_clean(2, 0, (uint8_t*)currentPreset.name);
//	Arsys_clean(2, 1, (uint8_t*)currentPreset.name);
//	if(!imya_temp)
//	{
//		Arsys_line(2, 0, (uint8_t*)currentPreset.name, 0);
//		Arsys_line(2, 1, (uint8_t*)currentPreset.comment, 0);
//	}
//	else
//	{
//		Arsys_line(2, 0, (uint8_t*)imya_t, 0);
//		Arsys_line(2, 1, (uint8_t*)imya1_t, 0);
//		imya_temp = 0;
//	}
//}

void prog_ind(uint32_t val, bool filled)
{
	val++;
	uint8_t col = 87;
	uint8_t prog_num[2];
	Set_Page_Address(0);
	t33x30_clear(col, 0, 39);
	prog_num[0] = val / 10;
	prog_num[1] = val % 10;
	for(uint8_t i = 0; i < 2; i++)
		col += t33x30_sym(col, 0, prog_num[i], filled);
}

void clear_str(uint8_t col, uint8_t pag, uint8_t font, uint8_t count)
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

void Arsys_ef(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs)
{
	uint8_t fl = 0;
	uint32_t sym;
	sym = adr[0];
	sym = (sym - 32) * 6;
	for(uint8_t i = 0; i < 8; i++)
	{
		Set_Column_Address(col + i);
		Set_Page_Address(pag);
		if(i > 1)
		{
			if((SystemFont5x7[sym + i - 2] & 0xc0) == 0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					oled023_1_write_data((SystemFont5x7[sym + i - 2] << 2) | 1);
				break;
				case 1:
					oled023_1_write_data((~(SystemFont5x7[sym + i - 2] << 2)) | 3);
					if(fl == 1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					oled023_1_write_data(1);
					fl = 0;
				break;
			}
		}
		if((i == 1) && (curs != 1))
			oled023_1_write_data(1);
		else
			oled023_1_write_data(0xff);
		Set_Column_Address(col + i);
		Set_Page_Address(pag + 1);
		if(i != 0)
		{
			if(i == 1)
			{
				if(curs != 1)
					oled023_1_write_data(4);
				else
					oled023_1_write_data(7);
			}
			else
			{
				if(curs != 1)
					oled023_1_write_data(4 + fl);
				else
					oled023_1_write_data((4 + fl) | 2);
			}
		}
		else
			oled023_1_write_data(7);
	}
	sym = adr[1];
	sym = (sym - 32) * 6;
	for(uint8_t i = 0; i < 7; i++)
	{
		Set_Column_Address(col + i + 8);
		Set_Page_Address(pag);
		if(i < 6)
		{
			if((SystemFont5x7[sym + i] & 0xc0) == 0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					oled023_1_write_data((SystemFont5x7[sym + i] << 2) | 1);
				break;
				case 1:
					oled023_1_write_data((~(SystemFont5x7[sym + i] << 2)) | 3);
					if(fl == 1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					oled023_1_write_data(1);
					fl = 0;
				break;
			}
		}
		if(i == 6)
			oled023_1_write_data(0xff);
		Set_Column_Address(col + i + 8);
		Set_Page_Address(pag + 1);
		if(i != 6)
		{
			if(curs != 1)
				oled023_1_write_data(4 + fl);
			else
				oled023_1_write_data((4 + fl) | 2);
		}
		else
			oled023_1_write_data(7);
	}
}
void disp_contr(uint8_t val)
{
	//erc12864_Set_Contrast_Control_Register(val * (22.0/31.0) + 14.0);
}

void sys_menu_init(void)
{

}
void tun_ini(void)
{
	oled023_1_disp_clear();
	scal_tun();
}
void tun_ind(void)
{
	strel_tun();
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
	Set_Column_Address(col);
	Set_Page_Address(pag);
	col = Arsys_sym(col, pag, t3[0] + 48, 0);
	col = Arsys_sym(col, pag, 44, 0);
	for(uint8_t i = 1; i < 4; i++)
		col = Arsys_sym(col, pag, t3[i] + 48, 0);
	col = Arsys_sym(col, pag, 115, 0);
}

void del_tim_ind(uint8_t col, uint8_t pag, uint32_t d)
{
	if(!sys_para[56])
		del_sec_ind(col, pag, d);
	else
	{
		Arsys_line(90, pag, (uint8_t*)"bpm", 0);
		Set_Column_Address(col);
		Set_Page_Address(pag);
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
