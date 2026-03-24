#include "serial.h"

#include "periphery.h"
#include "sharc.h"

#include "system.h"
#include "codec.h"

#include "display_task.h"
#include "midi_task.h"
#include "tuner_task.h"
#include "console.h"


//-------------------------------------------------SPI_init------------------------------------------------
void HW_SpiInit(uint8_t type)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_Cmd(SPI2, DISABLE);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_I2S_DeInit (SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	if(type)
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
}

ad_data_t adc_data[2];
da_data_t dac_data[2];
void HW_I2sInit()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	RCC_PLLI2SCmd (DISABLE);

	// Codec I2S
	SPI_I2S_DeInit (SPI3);
	SPI_I2S_DeInit (I2S3ext);

	I2S_InitTypeDef I2S_InitStructure;
	I2S_StructInit(&I2S_InitStructure);

	I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
	I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_Init(SPI3, &I2S_InitStructure);
	I2S_FullDuplexConfig(I2S3ext, &I2S_InitStructure);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd(I2S3ext, SPI_I2S_DMAReq_Rx, ENABLE);

	I2S_Cmd(SPI3, ENABLE);
	I2S_Cmd(I2S3ext, ENABLE);
	RCC_PLLI2SCmd (ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream2, DISABLE);
	DMA_Cmd(DMA1_Stream7, DISABLE);
	DMA_DeInit (DMA1_Stream2);
	DMA_DeInit (DMA1_Stream7);

	DMA_InitStructure.DMA_Channel = DMA_Channel_2;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2S3ext->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&adc_data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 8;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream2, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream2, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PRIORITY_CODEC_I2S;
	NVIC_Init(&NVIC_InitStructure);

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&dac_data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init(DMA1_Stream7, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream7, ENABLE);
}

volatile uint8_t key_reg_out[2] = {0, 0};
volatile uint16_t key_reg_in = 0xFFFF;
void HW_UartInit()
{
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_StructInit(&USART_InitStructure);

	//================== Midi USART ======================================================
	USART_InitStructure.USART_BaudRate = 31250;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	//================== Keys, fsw USART ====================================================
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_BaudRate = 2000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_Init(USART2, &USART_InitStructure);
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

	USART_ClockStructInit(&USART_ClockStructure);

	USART_ClockStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockStructure.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART2, &USART_ClockStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA_DeInit (DMA1_Stream5);
	DMA_DeInit (DMA1_Stream6);

	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&key_reg_in;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&key_reg_out;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PRIORITY_MIDI_UART;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PRIORITY_KEYS_READ;
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t HW_KeyInValue()
{
	return key_reg_in;
}

void HW_ReadKeysEnable()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	USART_Cmd(USART2, ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

//==============================ISR================================
extern "C" void USART1_IRQHandler()
{
	ISR_midi_recieve();
}

extern "C" void DMA1_Stream5_IRQHandler()
{
	ISR_buttons_read();
}

extern "C" void DMA1_Stream2_IRQHandler()	//I2S
{
	uint8_t dma_ht_fl;

	if(tap_temp < 131071) tap_temp += 1;

	if(DMA_GetITStatus(DMA1_Stream2, DMA_IT_HTIF2))
	{
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_HTIF2);
		dma_ht_fl = 0;
	}
	else
	{
		DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
		dma_ht_fl = 1;
	}

	GPIO_SetBits(GPIOC, GPIO_Pin_5); // LED4G(mono) - LED ST(stereo)

	uint32_t cr = ror16(adc_data[dma_ht_fl].left.sample);
	uint32_t cl = ror16(adc_data[dma_ht_fl].right.sample);
	int32_t ccl = cl;
	ccl = ccl >> 8;
	int32_t ccr = cr;
	ccr = ccr >> 8;

	DisplayTask->VolIndRoutine(ccl);
	if(TunerTask->IsrRoutine(ccl)) return;

	dac_data[dma_ht_fl].left.sample = ror16(cr);
	dac_data[dma_ht_fl].right.sample = ror16(cr);
}
