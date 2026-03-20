#ifndef ADC_H_
#define ADC_H_

#include "appdefs.h"

extern volatile uint16_t adc_bu;

void ADC_Calibrate(void);
void ADC_Init(uint8_t state);
void ADC_Routine(void);

#endif /* ADC_H_ */
