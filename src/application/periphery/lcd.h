#ifndef ER_OLEDM023_1B_H_
#define ER_OLEDM023_1B_H_

#include "appdefs.h"

#define CS GPIO_Pin_1
#define RS GPIO_Pin_0

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SendData(uint32_t data);
void LCD_WriteData(uint32_t data, uint16_t count = 1);
void LCD_WriteData(const uint8_t* data_ptr, uint16_t dataSize);
void LCD_WriteInstruction(uint32_t instr);

void LCD_SetPageAddress(unsigned char add);
void LCD_SetColumnAddress(unsigned char add);

#endif /* ER_OLEDM023_1B_H_ */
