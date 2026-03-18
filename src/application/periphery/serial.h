#ifndef SERIAL_H_
#define SERIAL_H_

#include "appdefs.h"

void HW_SpiInit(uint8_t type);
void HW_I2sInit();
void HW_UartInit();

void HW_ReadKeysEnable();

extern volatile uint16_t key_reg_in;

#endif /* SERIAL_H_ */
