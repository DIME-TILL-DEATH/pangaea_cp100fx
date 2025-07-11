#include "appdefs.h"

#include "sdio_sd.h"
#include "sdcard.h"

#include "appdefs.h"

extern "C" uint32_t get_fattime()
{
	return 0;
}

extern "C" void SDIO_IRQHandler(void)
{
	/* Process All SDIO Interrupt Sources */
	SD_ProcessIRQSrc();
}

/**
 * @brief  This function handles DMA2 Stream3 or DMA2 Stream6 global interrupts
 *         requests.
 * @param  None
 * @retval None
 */
extern "C" void SD_SDIO_DMA_IRQHANDLER(void)
{
	/* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
	SD_ProcessDMAIRQ();
}

extern "C" void SD_LowLevel_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< Disable SDIO Clock */
	SDIO_ClockCmd(DISABLE);

	/*!< Set Power State to OFF */
	SDIO_SetPowerState (SDIO_PowerState_OFF);

	/*!< DeInitializes the SDIO peripheral */
	SDIO_DeInit();

	/* Disable the SDIO APB2 Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_MCO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_MCO);

	/* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PD.02 CMD line */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure PC.12 pin: CLK pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * @brief  Initializes the SD Card and put it into StandBy State (Ready for
 *         data transfer).
 * @param  None
 * @retval None
 */

//#define SD_SOCKET_POWER_PIN              GPIO_Pin_0
//#define SD_SOCKET_POWER_GPIO_PORT        GPIOB
extern "C" void SD_LowLevel_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOC and GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);

	/* Configure PC.08, PC.09, PC.10, PC.11 pins: D0, D1, D2, D3 pins */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PD.02 CMD line */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Configure PC.12 pin: CLK pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Enable the SDIO APB2 Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

	/* Enable the DMA2 Clock */
	RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC for SDIO an d SDIO DMA */
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

}

/**
 * @brief  Configures the DMA2 Channel4 for SDIO Tx request.
 * @param  BufferSRC: pointer to the source buffer
 * @param  BufferSize: buffer size
 * @retval None
 */

extern "C" void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
	DMA_InitTypeDef SDDMA_InitStructure;

	DMA_ClearFlag(SD_SDIO_DMA_STREAM,
			SD_SDIO_DMA_FLAG_FEIF|SD_SDIO_DMA_FLAG_DMEIF|SD_SDIO_DMA_FLAG_TEIF|SD_SDIO_DMA_FLAG_HTIF
					|SD_SDIO_DMA_FLAG_TCIF);

	// DMA2 Stream3  or Stream6 disable
	DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

	// DMA2 Stream3  or Stream6 Config
	DMA_DeInit(SD_SDIO_DMA_STREAM);

	SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
	SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
	SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
	SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	SDDMA_InitStructure.DMA_BufferSize = 0;
	SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	SDDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
	SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
	DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
	DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

	// DMA2 Stream3  or Stream6 enable
	DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);

}

/**
 * @brief  Configures the DMA2 Channel4 for SDIO Rx request.
 * @param  BufferDST: pointer to the destination buffer
 * @param  BufferSize: buffer size
 * @retval None
 */
extern "C" void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
	DMA_InitTypeDef SDDMA_InitStructure;

	DMA_ClearFlag(SD_SDIO_DMA_STREAM,
			SD_SDIO_DMA_FLAG_FEIF|SD_SDIO_DMA_FLAG_DMEIF|SD_SDIO_DMA_FLAG_TEIF|SD_SDIO_DMA_FLAG_HTIF
					|SD_SDIO_DMA_FLAG_TCIF);

	/* DMA2 Stream3  or Stream6 disable */
	DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

	/* DMA2 Stream3 or Stream6 Config */
	DMA_DeInit(SD_SDIO_DMA_STREAM);

	SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
	SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
	SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferDST;
	SDDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	SDDMA_InitStructure.DMA_BufferSize = 0;
	SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	SDDMA_InitStructure.DMA_Priority = DMA_Priority_High;
	SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
	SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
	DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
	DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
	DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

	/* DMA2 Stream3 or Stream6 enable */
	DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
}

void SD_LowLevel_DetectInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(SD_DETECT_GPIO_CLK, ENABLE);

	/*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
	GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

	EXTI_InitTypeDef EXTI_InitStruct;

	EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

uint8_t SD_LowLevel_Detect(void)
{
	__IO uint8_t status = SD_PRESENT;
	if(GPIO_ReadInputDataBit(SD_DETECT_GPIO_PORT, SD_DETECT_PIN)!=Bit_RESET)
	{
		status = SD_NOT_PRESENT;
	}
	return status;
}
/*
 void SD_LowLevel_SocketPowerInit()
 {
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_AHB1PeriphClockCmd( SD_SOCKET_POWER_GPIO_CLK , ENABLE);

 // configure sd-socket power-on pin
 SD_SOCKET_POWER_GPIO_PORT->BSRRL=SD_SOCKET_POWER_PIN ;

 GPIO_InitStructure.GPIO_Pin = SD_SOCKET_POWER_PIN;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
 GPIO_Init(SD_SOCKET_POWER_GPIO_PORT, &GPIO_InitStructure);

 }

 void SD_LowLevel_SocketPower(uint8_t power)
 {
 if (power)
 SD_SOCKET_POWER_GPIO_PORT->BSRRH=SD_SOCKET_POWER_PIN;

 else
 SD_SOCKET_POWER_GPIO_PORT->BSRRL=SD_SOCKET_POWER_PIN;
 }
 */
