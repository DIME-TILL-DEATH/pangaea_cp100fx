#ifndef ALLFONTS_H_
#define ALLFONTS_H_

#include "appdefs.h"

#include "init.h"
#include "ER_OLEDM023-1B.h"

#include "amt.h"
#include "SystemFont5x7.h"
#include "12x13.h"
#include "33x30.h"

#include "eq_bitmap.h"
#include "par_bitmap.h"
#include "tun_bit.h"
#include "icon_bit.h"

#define disp_orient 0

namespace Font
{
typedef enum
{
	fnsBlack = 0,
	fnsWhite,
	fnsBlackUnderline,
	fnsCount
} TFontState;

typedef enum
{
	fnt12x13 = 0,
	fnt33x30,
	fntSystem,
	fntCount
} TFontName;

typedef struct
{
	TFontName name;
	uint8_t curs;
} TFontStruct;

uint8_t symbolWidth(TFontName fontName);
};

extern const uint8_t syst[];
extern const uint8_t equ[];
extern const uint8_t volu[];
extern const uint8_t in[];
extern const uint8_t out[];
extern const uint8_t inpp[];
extern const uint8_t outt[];
extern const uint8_t amt[];

extern const uint8_t sys[];
extern const uint8_t cc_of[];

extern uint32_t ind_in_p[];
extern uint32_t ind_out_l[];
extern uint16_t ind_poin;

extern uint8_t imya_temp;
extern uint8_t t_po;
extern volatile uint8_t t_no;

void oled023_1_disp_reset(void);
void disp_start(uint8_t num);
void vol_ind(uint8_t xPos, uint8_t indLength);
void menu_init(void);
void main_screen(void);
void prog_ind(uint32_t val, bool filled = true);
void clear_str(uint8_t col, uint8_t pag, uint8_t font, uint8_t count);
void Arsys_ef(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs);
void disp_contr(uint8_t val);
void mode_ind(uint8_t val);
void tap_ind(uint8_t cur);
void sys_menu_init(void);
void pot32x32_(uint8_t col, uint8_t pag, uint16_t sym);
void tun_ini(void);
void tun_ind(void);
void ind_foot(uint8_t num, uint8_t val);
void strel_print(uint8_t col, uint8_t pag, uint8_t dir);

void del_sec_ind(uint8_t col, uint8_t pag, uint32_t d);
void del_tim_ind(uint8_t col, uint8_t pag, uint32_t d);

#endif
