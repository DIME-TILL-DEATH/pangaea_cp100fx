#ifndef ADC_H_
#define ADC_H_

#include "appdefs.h"

extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;
extern volatile uint16_t adc_bu;

void ADC_calibrate(void);
void ADC_init(uint8_t state);
void ADC_routine(void);

#endif /* ADC_H_ */
