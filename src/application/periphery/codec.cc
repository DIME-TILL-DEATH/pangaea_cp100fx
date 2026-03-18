#include "codec.h"

#include "periphery.h"

void CODEC_Start()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_10); // codec pwd
	const uint16_t fff[3] = {0xa103, 0xa381, 0xa265};
	for(int i = 0; i<3; i++)
	{
		CODEC_Send(fff[i]);
	}
}

void CODEC_Send(uint16_t data)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, data);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOC, GPIO_Pin_7);

	HW_Delay(0xff);
}
