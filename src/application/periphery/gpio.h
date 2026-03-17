#ifndef GPIO_H_
#define GPIO_H_

#include "appdefs.h"

void HW_gpio_init();
void HW_adc_pin_init();
void HW_pin_usb_init();

void HW_port_B_conf(uint8_t mode);

#endif /* SRC_APPLICATION_PERIPHERY_GPIO_H_ */
