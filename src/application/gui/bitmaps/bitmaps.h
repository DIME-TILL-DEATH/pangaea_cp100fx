#ifndef ALLFONTS_H_
#define ALLFONTS_H_

#include <param_bitmap.h>
#include "appdefs.h"

#include "lcd.h"

#include "SystemFont5x7.h"
#include "12x13.h"
#include "33x30.h"

#include "amt.h"
#include "eq_bitmap.h"
#include "icons_bitmap.h"
#include "tuner_bitmap.h"

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

typedef enum
{
	VOL_INDICATOR_OFF = 0,
	VOL_INDICATOR_IN,
	VOL_INDICATOR_OUT,
	VOL_INDICATOR_VOLUME
}TVolIndicatorType;

void display_start(uint8_t num);
void progress_bar(uint8_t col, uint8_t pag, uint32_t val);
void vol_ind(uint8_t xPos, uint8_t indLength);
void clear_string(uint8_t col, uint8_t pag, uint8_t font, uint8_t count);
void preset_ind(uint32_t val, bool filled = true);
void checkbox(uint8_t x, uint8_t y, uint8_t val);
void effect_icon(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs);
void mode_ind(uint8_t val);
void tap_ind(uint8_t cur);
void fsw_ind(uint8_t num, uint8_t pressState, uint8_t holdState);
void arrow_print(uint8_t col, uint8_t pag, uint8_t dir);
void delay_time_ind(uint8_t col, uint8_t pag, uint32_t d);
void vol_indicator(uint8_t xPos, uint8_t indLength, uint32_t indValue, TVolIndicatorType volIndicatorType, uint8_t* volIndPar_ptr);

#endif
