#ifndef LED_H_
#define LED_H_

#include "appdefs.h"

typedef enum{
	LED_TAP_GREEN = 1,
//	LED_TAP_RED, // unavaliable. Controlling by SHARC
	LED_FX_GREEN, // unavaliable. Controlling by SHARC
	LED_FX_RED,

	LED_FSW_DOWN_RED,
	LED_FSW_CONFIRM_RED,
	LED_FSW_UP_RED,

	LED_FSW_DOWN_GREEN,
	LED_FSW_CONFIRM_GREEN,
	LED_FSW_UP_GREEN,
}TLedType;

typedef enum{
	DISABLED,
	ENABLED
}TLedState;

void LED_SetState(TLedType ledType, TLedState ledState);
void HW_WriteLed();

#endif /* LED_H_ */
