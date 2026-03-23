#ifndef SERIAL_H_
#define SERIAL_H_

#include "appdefs.h"

void HW_SpiInit(uint8_t type);
void HW_I2sInit();
void HW_UartInit();

void HW_ReadKeysEnable();

uint16_t HW_KeyInValue();

extern uint32_t ind_out_l[2];

#endif /* SERIAL_H_ */
