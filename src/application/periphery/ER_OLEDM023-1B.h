#ifndef ER_OLEDM023_1B_H_
#define ER_OLEDM023_1B_H_

#include "appdefs.h"

#define CS GPIO_Pin_1
#define RS GPIO_Pin_0

void oled023_1_disp_init(void);
void oled023_1_disp_clear(void);
void oled023_1_send_data(uint32_t data);
void oled023_1_write_data(uint32_t data);
void oled023_1_write_data(uint32_t data, uint16_t dataSize);
void oled023_1_write_data(const uint8_t* data_ptr, uint16_t dataSize);
void oled023_1_write_instruction(uint32_t instr);

void Set_Page_Address(unsigned char add);
void Set_Column_Address(unsigned char add);
void Set_Col_Pag_Adress(uint8_t col, uint8_t pag);


#endif /* ER_OLEDM023_1B_H_ */
