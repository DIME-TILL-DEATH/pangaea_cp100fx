#ifndef ER_OLEDM023_1B_H_
#define ER_OLEDM023_1B_H_

#include "appdefs.h"

#include "gui/gui_task.h"

#define CS GPIO_Pin_1
#define RS GPIO_Pin_0

void oled023_1_disp_init(void);
void oled023_1_send_data(uint32_t dat);
void oled023_1_write_data(uint32_t dat);
void oled023_1_write_instruction(uint32_t dat);
void Set_Page_Address(unsigned char add);
void Set_Column_Address(unsigned char add);
void Set_Col_Pag_Adress(uint8_t col,uint8_t pag);
void oled023_1_disp_clear(void);
void led_disp_write(void);
void write_pot(void);
extern uint8_t pot_val;
extern uint8_t sys_para[];
//extern const uint16_t att_db_num [56];

#endif /* ER_OLEDM023_1B_H_ */
