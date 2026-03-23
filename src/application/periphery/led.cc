#include "led.h"

void LED_SetState(TLedType ledType, TLedState ledState)
{
	switch(ledType)
	{
		case LED_TAP_GREEN:
		{
			if(ledState == TLedState::ENABLED) GPIO_SetBits(GPIOB, GPIO_Pin_11);
			else GPIO_ResetBits(GPIOB, GPIO_Pin_11);
			break;
		}

		case LED_FX_RED:
		{
			if(ledState == TLedState::ENABLED) GPIO_SetBits(GPIOB, GPIO_Pin_14);
			else GPIO_ResetBits(GPIOB, GPIO_Pin_14);
			break;
		}
	}
}
