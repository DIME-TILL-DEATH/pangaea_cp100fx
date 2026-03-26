#ifndef ADC_H_
#define ADC_H_

#include "appdefs.h"

typedef enum{
	EXPR_TYPE_OFF = 0,
	EXPR_TYPE_VOL_STD,
	EXPR_TYPE_VOL_ALT,
	EXPR_TYPE_CC_STD,
	EXPR_TYPE_CC_ALT
}TExprType;

extern volatile uint16_t adc_bu;
extern volatile uint16_t adc_bu2;

void ADC_SetState(uint8_t state);
void ADC_LoadCal();
void EXPR_Routine();
void EXPR_StoreLevel();

#endif /* ADC_H_ */
