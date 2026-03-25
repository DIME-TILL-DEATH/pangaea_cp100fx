#ifndef PAR_BITMAP_H_
#define PAR_BITMAP_H_

#include "appdefs.h"

#define IND_WIDTH 33

void param_ind(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_num(uint8_t col, uint8_t pag, uint32_t val);
void param_transparent_ind(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_pan(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_mix(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_note(uint8_t col, uint8_t pag, uint32_t note);

#endif
