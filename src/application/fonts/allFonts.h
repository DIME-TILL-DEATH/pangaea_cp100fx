#include "appdefs.h"
#include "ER_OLEDM023-1B.h"
#include "SystemFont5x7.h"
#include "12x13.h"
#include "33x30.h"
#include "eq_bitmap.h"
#include "par_bitmap.h"
#include "amt.h"
#include "init.h"
#include "tun_bit.h"

#define disp_orient 0

extern const uint8_t cs[];
extern const uint8_t ph[];
extern const uint8_t ch[];
extern const uint8_t dl[];
extern const uint8_t rm[];
extern const uint8_t er[];
extern const uint8_t rv[];
extern const uint8_t tr[];
extern const uint8_t syst[];
extern const uint8_t equ[];
extern const uint8_t volu[];
extern const uint8_t in[];
extern const uint8_t out[];
extern const uint8_t cm[];
extern const uint8_t am[];
extern const uint8_t vl[];
extern const uint8_t na[];
extern const uint8_t pr[];
extern const uint8_t gt[];
extern const uint8_t cm[];
extern const uint8_t rf[];
extern const uint8_t inpp[];
extern const uint8_t outt[];
extern const uint8_t amt [];

extern const uint8_t sys[];
extern const uint8_t cc_of[];
extern const uint8_t sys_menu_list [][12];
extern const uint8_t expr_type [][12];
extern const uint8_t expr_menu [][10];
extern const uint8_t expr_on_off [][5];
extern const uint8_t foot_sw_type[][12];
extern const uint8_t footsw_menu[][12];
extern const uint8_t ext_switch  [][12];
extern const uint8_t mode_list   [][12];
extern const uint8_t int_sw_list [][12];
extern const uint8_t contr_ext_l [][12];
extern const uint8_t fsw_t[][12];
extern const uint8_t spdif_type [][12];
extern const uint8_t tempo_type [][10];
extern const uint8_t time_type [][4];
extern uint32_t ind_in_p[];
extern uint32_t ind_out_l[];
extern uint16_t ind_poin;
extern uint8_t vol_fl;
extern uint8_t vol_vol;
extern uint8_t inp_ind_fl;
extern uint8_t out_ind_fl;
//extern uint8_t edit_modules_fl;
extern uint8_t imya_temp;
extern uint8_t prog_cur;

extern uint8_t t_po;
extern volatile uint8_t t_no;

void oled023_1_disp_reset(void);
void disp_start(uint8_t num);
void vol_ind();
void menu_init(void);
void main_screen(void);
void prog_ind(uint32_t val);
void clear_str(uint8_t col , uint8_t pag , uint8_t font , uint8_t count);
void Arsys_ef(uint8_t col , uint8_t pag , uint8_t* adr ,uint8_t curs);
void disp_contr(uint8_t val);
void mode_ind(uint8_t val);
void tap_ind (uint8_t cur);
void sys_menu_init(void);
void icon_print(uint8_t num , uint8_t strel);
void pot32x32_(uint8_t col , uint8_t pag , uint16_t sym);
void tun_ini(void);
void tun_ind(void);
void ind_foot(uint8_t num , uint8_t val);
void strel_print(uint8_t col , uint8_t pag , uint8_t dir);

inline void del_sec_ind(uint8_t col , uint8_t pag , uint32_t d)
{
	uint16_t t3[4];
	t3[0] = d / 1000;
	t3[2] = d % 1000;
	t3[1] = t3[2] / 100 ;
	t3[3] = t3[2] % 100;
	t3[2] = t3[3] / 10;
	t3[3] = t3[3] % 10;
	Set_Column_Address(col);
	Set_Page_Address(pag);
	col = Arsys_sym(col,pag,t3[0]+48,0);
	col = Arsys_sym(col,pag,44,0);
	for(uint8_t i = 1 ; i < 4 ; i++)col = Arsys_sym(col,pag,t3[i]+48,0);
	col = Arsys_sym(col,pag,115,0);
}
inline void del_tim_ind(uint8_t col , uint8_t pag , uint32_t d)
{
	if(!sys_para[56])del_sec_ind(col , pag , d);
	else {
		Arsys_line(90,pag,(uint8_t*)"bpm",0);
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
			for(uint8_t i = 0 ; i < 4 ; i++)col = Arsys_sym(col,pag,t3[i]+48,0);
			col = Arsys_sym(col,pag,44,0);
			return;
		}
		if(a >= 100.0f)
		{
			a = (a - b) * 10.0f;
			t3[0] = b / 100;
			t3[2] = b % 100;
			t3[1] = t3[2] / 10;
			t3[2] = t3[2] % 10;
			for(uint8_t i = 0 ; i < 3 ; i++)col = Arsys_sym(col,pag,t3[i]+48,0);
			col = Arsys_sym(col,pag,44,0);
			Arsys_sym(col,pag,a+48,0);
			return;
		}
		a = (a - b) * 100.0f;
		t3[0] = b / 10;
		t3[1] = b % 10;
		for(uint8_t i = 0 ; i < 2 ; i++)col = Arsys_sym(col,pag,t3[i]+48,0);
		col = Arsys_sym(col,pag,44,0);
		b = a;
		t3[0] = b / 10;
		t3[1] = b % 10;
		for(uint8_t i = 0 ; i < 2 ; i++)col = Arsys_sym(col,pag,t3[i]+48,0);
	}
}
