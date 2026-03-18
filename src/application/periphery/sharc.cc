#include "sharc.h"

#include "periphery.h"
#include "serial.h"

#include "system.h"
#include "modules.h"

void SHARC_SpiInit(TSharcSpiMode mode)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_Cmd(SPI2, DISABLE);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_I2S_DeInit (SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	if(mode == TSharcSpiMode::SPI_SLAVE)
	{
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	}
	else
	{
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	}
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	HW_Delay(0xffffff); // 0x1ffffff start, 0x3ffffff end
}

void SHARC_StartupLoad()
{
	SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Reset);
	SHARC_SendData(0);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);	//reset pin

	for(size_t i = 0; i<3; i++)
		SHARC_SendData(0);

	uint32_t binarySize;
	uint8_t* binaryStart;
#ifdef __STEREO_MOD__
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_start;
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_end;

	binaryStart = &_binary_Pangaea_CP100FX_stereo_ldr_start;
	binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
#endif

#ifdef __MONO_MOD__
	if(cab_type != CAB_CONFIG_STEREO)
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_end;

		binaryStart = &_binary_Pangaea_CP100FX_1_ldr_start;
		binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
	}
	else
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_end;

		binaryStart = &_binary_Pangaea_CP100FX_1_duble_fir_ldr_start;
		binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_duble_fir_ldr_end-&_binary_Pangaea_CP100FX_1_duble_fir_ldr_start);
	}
#endif
	for(uint32_t i = 0; i<binarySize; i++)
		SHARC_SendData(binaryStart[i]);
}

void SHARC_WaitForReady()
{

}

void SHARC_SendData(uint16_t data)
{
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, data);
}

void SHARC_dma_send_data(uint32_t* data_ptr, uint16_t size)
{
	SPI_Cmd(SPI2, DISABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream4, DISABLE);
	DMA_DeInit (DMA1_Stream4);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)data_ptr;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI2, ENABLE);
	DMA_Cmd(DMA1_Stream4, ENABLE);

//	while(DMA1_Stream4->NDTR > 0);
}
