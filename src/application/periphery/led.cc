#include "led.h"

#include "display_task.h"

uint8_t serialLedData[2] = {0, 0};

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
#ifdef __MONO_MOD__
			if(ledState == TLedState::ENABLED) GPIO_SetBits(GPIOB, GPIO_Pin_14);
			else GPIO_ResetBits(GPIOB, GPIO_Pin_14);
#endif
			if(ledState == TLedState::ENABLED)
			{
				if(!(serialLedData[0] & 0b00100000))
				{
					serialLedData[0] |= 0b00100000;
					DisplayTask->LedWrite();
				}
			}
			else
			{
				if(serialLedData[0] & 0b00100000)
				{
					serialLedData[0] &= ~0b00100000;
					DisplayTask->LedWrite();
				}
			}
			break;
		}
	}
}

void HW_WriteLed()
{
#ifdef __STEREO_MOD__
	for(int i = 0 ; i < 0xff ; i++) NOP();

	for(int i = 0 ; i < 2 ; i++)
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
		USART_SendData(USART3, __RBIT(serialLedData[i]) >> 24);
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
	}

	GPIO_SetBits(GPIOC,GPIO_Pin_5);

	for(int i = 0 ; i < 0xff ; i++) NOP();

	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
#endif
}
