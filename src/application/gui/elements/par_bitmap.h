#ifndef PAR_BITMAP_H_
#define PAR_BITMAP_H_

#include "appdefs.h"

void par_ind_num(uint8_t val);
void par_ind_num_(uint8_t col, uint8_t pag, uint16_t val);
void par_ind_num_pan(uint8_t col, uint8_t pag, uint8_t val);
uint8_t ind_num_pan(uint8_t col, uint8_t pag, uint8_t val);
void par_ind_num_mix(uint8_t col, uint8_t pag, uint8_t val);
uint8_t par_ind(uint8_t col, uint8_t pag, uint32_t val);
void p_ind(uint8_t col, uint8_t pag, uint32_t val);
void par_ind_pan(uint8_t col, uint8_t pag, uint32_t val);
void par_ind_mix(uint8_t col, uint8_t pag, uint32_t val);

#endif
