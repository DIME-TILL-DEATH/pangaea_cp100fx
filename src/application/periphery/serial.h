#ifndef SERIAL_H_
#define SERIAL_H_

#include "appdefs.h"

void HW_spi_init(uint8_t type);
void HW_i2s_init();
void HW_uart_init();

void HW_read_keys_enable();

extern volatile uint16_t key_reg_in;

#endif /* SERIAL_H_ */
