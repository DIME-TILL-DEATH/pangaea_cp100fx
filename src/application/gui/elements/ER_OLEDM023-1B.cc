#include "appdefs.h"
#include "ER_OLEDM023-1B.h"
#include "allFonts.h"

#include "preset.h"
#include "system.h"

const uint16_t att_db_num [56] = {510,459,421,390,368,344,331,320,311,303,297,291,287,283,279,277,274,272,269,268,
266,264,263,262,261,260,259,258,257,256,253,249,227,207,182,144,128,105,100,89,80,70,62,55,48,43,38,34,30,27,24,22,19,17,15,13};

uint8_t oled_ini[30] = {0xae,0xd5,0xa0,0xa8,0x1f,0xd3,0x00,0xad,
		                0x8e,0xd8,0x05,0xa0,0xc8,0xda,0x12,0x91,
		                0x3f,0x3f,0x3f,0x3f,0x81,0x7f,0xd9,0xd2,
		                0xdb,0x34,0xa6,0xa4,0xc0,0xaf
};
uint8_t led_buf[3];
void oled023_1_disp_delay(uint32_t val)
{
  for(uint32_t i = 0 ; i < val ; i++)NOP();
}
void oled023_1_send_data(uint32_t dat)
{
  while(!USART_GetFlagStatus(USART3,USART_FLAG_TXE));
  USART_SendData(USART3,__RBIT(dat) >> 24);
  while(!USART_GetFlagStatus(USART3,USART_FLAG_TC));
}
void oled023_1_send_data_pot(uint32_t dat)
{
	for(uint8_t i = 0 ; i < 8 ; i++)
    {
      GPIO_ResetBits(GPIOB,GPIO_Pin_12);
      if((dat & 0x80) == 0)GPIO_ResetBits(GPIOB,GPIO_Pin_10);
      else GPIO_SetBits(GPIOB,GPIO_Pin_10);
      oled023_1_disp_delay(10);
      GPIO_SetBits(GPIOB,GPIO_Pin_12);
      oled023_1_disp_delay(10);
      dat = dat << 1;
    }
}
void oled023_1_write_data(uint32_t dat)
{
  GPIO_ResetBits(GPIOB,CS);
  oled023_1_send_data(dat);
  GPIO_SetBits(GPIOB,CS);
}
void oled023_1_write_instruction(uint32_t dat)
{
  GPIO_ResetBits(GPIOB,RS);
  GPIO_ResetBits(GPIOB,CS);
  oled023_1_send_data(dat);
  GPIO_SetBits(GPIOB,CS);
  GPIO_SetBits(GPIOB,RS);
}
void Set_Page_Address(unsigned char add)
{
	add=0xb0|add;
	oled023_1_write_instruction(add);
    NOP();
}
void Set_Column_Address(unsigned char add)
{
	if(disp_orient)add +=4;
	oled023_1_write_instruction((0x10|(add>>4)));
	oled023_1_write_instruction((0x0f&add));
}
void Set_Col_Pag_Adress(uint8_t col,uint8_t pag)
{
	Set_Column_Address(col);
	Set_Page_Address(pag);
}
void oled023_1_disp_clear(void)
{
  for (uint16_t j = 0 ; j < 4 ; j++)
    {
	  Set_Page_Address(j);
	  Set_Column_Address(0);
	  GPIO_ResetBits(GPIOB,CS);
      for(uint16_t i = 0 ; i < 128 ; i++)oled023_1_write_data(0);
      GPIO_SetBits(GPIOB,CS);
    }
}
void led_disp_write(void)
{
	for(int i = 0 ; i < 3 ; i++)oled023_1_send_data(led_buf[i]);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}
inline void port_B_conf(uint8_t mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;
	if(mode)
	{
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //sd | clk
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
		  GPIO_Init(GPIOB, &GPIO_InitStructure);

		  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
		  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_USART3);
	}
	else {
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //sd | clk
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12;
		  GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
}
void oled023_1_disp_reset(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
	oled023_1_disp_delay(0x6ff);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
	oled023_1_disp_delay(0x6ff);

	if(disp_orient){oled_ini[11] = 0xa1;oled_ini[28] = 0xc8;}
	for(int i = 0 ; i < 30 ; i++)oled023_1_write_instruction(oled_ini[i]);
}
void oled023_1_disp_init(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC,ENABLE);
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_SetBits(GPIOB,GPIO_Pin_0);
  GPIO_SetBits(GPIOB,GPIO_Pin_1);
  GPIO_SetBits(GPIOB,GPIO_Pin_12);
  GPIO_SetBits(GPIOC,GPIO_Pin_6);
  GPIO_ResetBits(GPIOB,GPIO_Pin_15);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 |// oled_cs | oled_d/c
		  	  	  	  	  	  	GPIO_Pin_15;             //  reset_oled
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//Pot_cs
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  port_B_conf(1);

  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockStructure;

  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );
  USART_StructInit(&USART_InitStructure);

  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  USART_InitStructure.USART_BaudRate = 2000000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_Init(USART3,&USART_InitStructure);

  USART_ClockStructInit(&USART_ClockStructure);

  USART_ClockStructure.USART_Clock = USART_Clock_Enable;
  USART_ClockStructure.USART_LastBit = USART_LastBit_Enable;
  USART_ClockStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockStructure.USART_CPOL = USART_CPOL_High;
  USART_ClockInit(USART3,&USART_ClockStructure);
  USART_Cmd(USART3,ENABLE);

  oled023_1_disp_delay(0x6ff);
  GPIO_SetBits(GPIOB,GPIO_Pin_15);
  oled023_1_disp_delay(0x6ff);

  if(disp_orient){oled_ini[11] = 0xa1;oled_ini[28] = 0xc8;}
  for(int i = 0 ; i < 30 ; i++)oled023_1_write_instruction(oled_ini[i]);

  //oled023_1_disp_reset();

  oled023_1_disp_clear();
}
void pot_send_data(uint32_t dat,uint8_t num)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	oled023_1_disp_delay(6);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    oled023_1_disp_delay(6);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    if(num & 2)GPIO_SetBits(GPIOB,GPIO_Pin_10);
    else GPIO_ResetBits(GPIOB,GPIO_Pin_10);
    oled023_1_disp_delay(6);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    oled023_1_disp_delay(6);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    if(num & 1)GPIO_SetBits(GPIOB,GPIO_Pin_10);
    else GPIO_ResetBits(GPIOB,GPIO_Pin_10);
    oled023_1_disp_delay(6);
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
    oled023_1_disp_delay(6);
    oled023_1_send_data_pot(dat);
}

void write_pot(void)
{
	port_B_conf(0);
	uint16_t att_val;
	if(sys_para[System::ATTENUATOR_MODE])
		att_val = att_db_num[currentPreset.modules.paramData.attenuator];
	else
		att_val = att_db_num[sys_para[System::ATTENUATOR]];

	uint8_t b;
	if(att_val > 255)
	{
		b = att_val - 255;
		att_val = 0;
	}
	else {
		b = 0;
		att_val = 255 - att_val;
	}
	  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	  pot_send_data(sys_para[System::PHONES_VOLUME] << 1,0);
	  GPIO_SetBits(GPIOC,GPIO_Pin_6);
	  oled023_1_disp_delay(6);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	  pot_send_data(att_val,1);
	  GPIO_SetBits(GPIOC,GPIO_Pin_6);
	  oled023_1_disp_delay(6);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	  pot_send_data(sys_para[System::PHONES_VOLUME] << 1,2);
	  GPIO_SetBits(GPIOC,GPIO_Pin_6);
	  oled023_1_disp_delay(6);
	  GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	  pot_send_data(b,3);
	  GPIO_SetBits(GPIOC,GPIO_Pin_6);
	  port_B_conf(1);
}



