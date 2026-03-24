#ifndef FONT_1213_H_
#define FONT_1213_H_

#include "appdefs.h"
#include "lcd.h"

uint32_t t12x13_symbol(uint8_t col , uint8_t pag , uint16_t sym , uint8_t curs);
void t12x13_line(uint32_t col , uint8_t pag , uint8_t* adr , uint8_t curs);
void t12x13_clear(uint8_t col , uint8_t pag , uint8_t size);

#endif
