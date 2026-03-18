#ifndef ADC_H_
#define ADC_H_

#include "appdefs.h"

extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;
extern volatile uint16_t adc_bu;

void ADC_Calibrate(void);
void ADC_Init(uint8_t state);
void ADC_Routine(void);

#endif /* ADC_H_ */
