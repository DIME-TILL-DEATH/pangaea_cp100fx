#ifndef BF706_SEND_H_
#define BF706_SEND_H_

#include "appdefs.h"
#include "modules.h"

extern uint32_t send_buf;
extern uint8_t spi_stat;
extern uint8_t __CCM_BSS__ preset_temp[];

inline void dsp_send(uint8_t type , uint16_t data)
{
	send_buf = (data << 16) | type;
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit(EXTI_Line9);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)){};

	SPI_I2S_SendData(SPI2 , send_buf >> 16);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2 , send_buf);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

void DSP_gui_set_parameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value);
void DSP_contr_set_parameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value);

void master_volum(uint8_t val);
void master_volu(uint8_t val);
void DSP_reverb_param(uint8_t num);
void Early_par(uint8_t num);
void del_par(uint8_t num);
void cab_volume(uint8_t num);
void amp_volume(uint8_t num);
void send_cab_data(uint8_t val,uint8_t num,uint8_t menu_fl);
void send_cab_data1(uint8_t val,uint8_t num);
void eq_par(uint8_t num);
void eq_band_par(uint8_t num);
void Phaser_par(uint8_t num);
void chor_par(uint8_t num);
void tr_param(uint8_t num);
void program_ch(void);
void flanger_par(uint8_t num);
void tun_proc(uint8_t num);
void cab_dry_mute();
void ind_source(uint8_t num);
void gui_send(uint8_t num , uint16_t val);
void contr_send(uint8_t num , uint16_t val);
void ext_send(uint8_t data);
void comp_para(uint32_t val);
void gate_par(uint32_t val);

#endif /* BF706_SEND_H_ */
