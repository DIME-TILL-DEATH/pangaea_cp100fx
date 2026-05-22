#ifndef GPIO_H_
#define GPIO_H_

#include "appdefs.h"

void HW_GpioInit();
void HW_AdcPinInit();
void HW_PinUsbInit();

void HW_ExtiInit();

void HW_PortBConf(uint8_t mode);

#endif /* GPIO_H_ */
