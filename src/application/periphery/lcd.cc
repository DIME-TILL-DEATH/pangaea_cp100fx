#include <bitmaps.h>
#include <eeprom.h>
#include "lcd.h"

#include "periphery.h"
#include "gpio.h"

#include "preset.h"
#include "system.h"


uint8_t oled_ini[30] =
{0xae, 0xd5, 0xa0, 0xa8, 0x1f, 0xd3, 0x00, 0xad, 0x8e, 0xd8, 0x05, 0xa0, 0xc8, 0xda, 0x12, 0x91, 0x3f, 0x3f, 0x3f, 0x3f,
		0x81, 0x7f, 0xd9, 0xd2, 0xdb, 0x34, 0xa6, 0xa4, 0xc0, 0xaf};
uint8_t led_buf[3];

void LCD_Init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 |  // oled_cs | oled_d/c
			GPIO_Pin_15;             //  reset_oled
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;             //Pot_cs
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	HW_PortBConf(1);

	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	USART_StructInit(&USART_InitStructure);

	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_BaudRate = 2000000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_Init(USART3, &USART_InitStructure);

	USART_ClockStructInit(&USART_ClockStructure);

	USART_ClockStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockStructure.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART3, &USART_ClockStructure);
	USART_Cmd(USART3, ENABLE);

	HW_Delay(0x6ff);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
	HW_Delay(0x6ff);

	if(disp_orient)
	{
		oled_ini[11] = 0xa1;
		oled_ini[28] = 0xc8;
	}
	for(int i = 0; i < 30; i++)
		LCD_WriteInstruction(oled_ini[i]);

	LCD_Clear();
}
//
//void LCD_SendData(uint32_t dat)
//{
//	while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
//	USART_SendData(USART3, __RBIT(dat) >> 24);
//	while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
//}

void LCD_WriteData(uint32_t data, uint16_t count)
{
	GPIO_ResetBits(GPIOB, CS);

	for(uint8_t i = 0; i < count; i++)
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
		USART_SendData(USART3, __RBIT(data) >> 24);
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
	}

	GPIO_SetBits(GPIOB, CS);
}

void LCD_WriteData(const uint8_t* data_ptr, uint16_t dataSize)
{
	GPIO_ResetBits(GPIOB, CS);

	for(uint8_t i = 0; i < dataSize; i++)
	{
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
		USART_SendData(USART3, __RBIT(data_ptr[i]) >> 24);
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
	}
	GPIO_SetBits(GPIOB, CS);
}

void LCD_WriteInstruction(uint32_t instr)
{
	GPIO_ResetBits(GPIOB, RS);
	GPIO_ResetBits(GPIOB, CS);
	while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE));
	USART_SendData(USART3, __RBIT(instr) >> 24);
	while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
	GPIO_SetBits(GPIOB, CS);
	GPIO_SetBits(GPIOB, RS);
}

void LCD_SetPageAddress(unsigned char add)
{
	add = 0xb0 | add;
	LCD_WriteInstruction(add);
	NOP();
}

void LCD_SetColumnAddress(unsigned char add)
{
	if(disp_orient)
		add += 4;
	LCD_WriteInstruction((0x10 | (add >> 4)));
	LCD_WriteInstruction((0x0f & add));
}

void LCD_Clear(void)
{
	for(uint16_t j = 0; j < 4; j++)
	{
		LCD_SetPageAddress(j);
		LCD_SetColumnAddress(0);
		uint8_t nullData = 0;
		LCD_WriteData(nullData, 128);
	}
}

