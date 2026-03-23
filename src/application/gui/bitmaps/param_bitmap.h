#ifndef PAR_BITMAP_H_
#define PAR_BITMAP_H_

#include "appdefs.h"

void param_ind(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_num_(uint8_t col, uint8_t pag, uint16_t val);
void param_transparent_ind(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_pan(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_mix(uint8_t col, uint8_t pag, uint32_t val);
void param_ind_note(uint8_t col, uint8_t pag, uint8_t note);

#endif
