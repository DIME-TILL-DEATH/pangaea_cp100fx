#include "appdefs.h"
#include "init.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "fs.h"
#include "cs.h"
#include "AT45DB321.h"

uint32_t sram_point;
uint32_t sram_point1;
uint8_t tim5_fl;
uint8_t us2_buf[2];
uint8_t us2_buf1[2] = {0xaa,0x55};
uint8_t cab_type = 0;

ad_data_t adc_data[2] ;
da_data_t dac_data[2] ;

extern volatile uint8_t key_reg_out[];
extern volatile uint16_t key_reg_in;
extern const uint8_t dsp_cod[];
void adc_calib(void)
{
	  adc_low = sys_para[7];
	  adc_low |= sys_para[8] << 8;
	  adc_high = sys_para[9];
	  adc_high |= sys_para[10] << 8;
	  extern volatile uint8_t adc_inv_fl;
	  adc_inv_fl = 0;
	  if(adc_high < adc_low)
	  {
		  volatile uint16_t a = adc_high;
		  adc_high = adc_low;
		  adc_low = a;
		  adc_inv_fl = 1;
	  }
	  adc_val = adc_high - adc_low;
	  adc_val1 = 127.0f/adc_val;
}
//-------------------------------------------Setting PINs ADC------------------------------------------
void adc_pin_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	if((sys_para[3] & 1))
	{
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	   GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_1 ;
       GPIO_Init(GPIOC, &GPIO_InitStructure);
       GPIO_SetBits(GPIOC,GPIO_Pin_1);

       GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed ;
       GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
       GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
       GPIO_Init(GPIOC, &GPIO_InitStructure);
	  }
	else {
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	   GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 ;
	   GPIO_Init(GPIOC, &GPIO_InitStructure);
	   GPIO_SetBits(GPIOC,GPIO_Pin_0);

	   GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed ;
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	   GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
}
//-------------------------------------------Setting ADC------------------------------------------------
void adc_init(uint8_t stat)
{
  if(stat)
  {
	adc_calib();
	adc_pin_init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, 10, 1, ADC_SampleTime_480Cycles);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_Init( &NVIC_InitStructure );

  	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
  	ADC_Cmd(ADC1, ENABLE);
  	ADC_SoftwareStartConv(ADC1);
  }
  else {
	  ADC_ITConfig(ADC1,ADC_IT_EOC,DISABLE);
	  ADC_Cmd(ADC1, DISABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
  }
}
//------------------------------------------------Send to codec-------------------------------------------
void send_codec(uint16_t data)
{
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2 , data);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
		for(int i = 0 ; i < 0xff ; i++)NOP();
}
//-------------------------------------------------SPI_init------------------------------------------------
void spi_init(uint8_t type)
{
	SPI_Cmd(SPI2, DISABLE);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	if(type)
	{
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
		SPI_InitStructure.SPI_NSS =  SPI_NSS_Soft;
	}
	else {
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_NSS =  SPI_NSS_Soft;
	}
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}
//---------------------------------------------------------------------------------------------------------
void eepr_init(void);
void init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockStructure;

	for(uint32_t d = 0 ; d < 0xfffff ; d++)NOP();
	AT45DB321_Init();
	for(uint32_t d = 0 ; d < 0xfffff ; d++)NOP();
	eepr_init();
	cab_type = sys_para[2];
//-------------------------------------------------GPIO_init-----------------------------------------------
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           // DSP feedback
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; // SD_detect
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13  | GPIO_Pin_14 | GPIO_Pin_15; // Encoder
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_10 /*| GPIO_Pin_13 | GPIO_Pin_14*/; // reset_dsp | reset_codec
	GPIO_Init(GPIOA, &GPIO_InitStructure);                   // SWD pins

	GPIO_SetBits(GPIOC,GPIO_Pin_4);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	GPIO_ResetBits(GPIOC,GPIO_Pin_5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // Key_reg_sc | LED4 green | Pot cs | codec_cs
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB,GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_14; // No select | LED3 green | LED4 red
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_15; // key_usart | key_usart | I2S3ws
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6  | GPIO_Pin_7 | GPIO_Pin_13;
								// I2S3sclk      I2S3ext_sd    I2S3_sd      MIDI_out      MIDI_in      SPI2_clk
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // SPI2
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_I2S3ext);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_USART2);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);

	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;

    EXTI_InitStruct.EXTI_Line = EXTI_Line9;
    EXTI_Init(&EXTI_InitStruct);

    //EXTI_InitStruct.EXTI_Line = EXTI_Line13;
    //EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Line = EXTI_Line14;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Line = EXTI_Line15;
    EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    EXTI_Init(&EXTI_InitStruct);

//----------------------------------------------------------I2S_init----------------------------------------------
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2 | RCC_APB1Periph_SPI3 , ENABLE );
	RCC_PLLI2SCmd (DISABLE);
	SPI_I2S_DeInit(SPI3);
	SPI_I2S_DeInit(I2S3ext);
	I2S_InitTypeDef I2S_InitStructure;
	I2S_StructInit(&I2S_InitStructure);

	I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
	I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
	I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	I2S_Init(SPI3, &I2S_InitStructure);
	I2S_FullDuplexConfig(I2S3ext,&I2S_InitStructure);
	SPI_I2S_DMACmd (SPI3,SPI_I2S_DMAReq_Tx,ENABLE);
	SPI_I2S_DMACmd (I2S3ext ,SPI_I2S_DMAReq_Rx,ENABLE);

	I2S_Cmd (SPI3 , ENABLE );
	I2S_Cmd (I2S3ext , ENABLE );
	RCC_PLLI2SCmd (ENABLE);

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_StructInit(&USART_InitStructure);

    USART_InitStructure.USART_BaudRate = 31250;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_BaudRate = 2000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_Init(USART2,&USART_InitStructure);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);

	USART_ClockStructInit(&USART_ClockStructure);

	USART_ClockStructure.USART_Clock = USART_Clock_Enable;
	USART_ClockStructure.USART_LastBit = USART_LastBit_Enable;
	USART_ClockStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockStructure.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART2,&USART_ClockStructure);

	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_DMA1 , ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);

	DMA_Cmd(DMA1_Stream2, DISABLE);
	DMA_Cmd(DMA1_Stream7, DISABLE);
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA_DeInit (DMA1_Stream2 );
	DMA_DeInit (DMA1_Stream7 );
	DMA_DeInit (DMA1_Stream5 );
	DMA_DeInit (DMA1_Stream6 );

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

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&dac_data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init(DMA1_Stream7, &DMA_InitStructure);
	DMA_Cmd(DMA1_Stream7, ENABLE);

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

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Init( &NVIC_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init( &NVIC_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init( &NVIC_InitStructure );

	DMA_ITConfig ( DMA1_Stream5 , DMA_IT_TC , ENABLE);
	USART_Cmd(USART2,ENABLE);
	DMA_Cmd(DMA1_Stream5, ENABLE);
	DMA_Cmd(DMA1_Stream6, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4 |
    		               RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM14 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9 , ENABLE);

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Period = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 0x1fff;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_SelectOnePulseMode(TIM3,TIM_OPMode_Single);
    TIM_SetCounter(TIM3,0);

    TIM_TimeBaseStructure.TIM_Period = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 0x200;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Period = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 219;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
    TIM_SelectOnePulseMode(TIM14,TIM_OPMode_Single);
    TIM_SetCounter(TIM14,0);

    TIM_TimeBaseStructure.TIM_Period = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 0xff;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_SelectOnePulseMode(TIM6,TIM_OPMode_Single);
    TIM_SetCounter(TIM6,0);

    TIM_TimeBaseStructure.TIM_Period = 0xffff;
    TIM_TimeBaseStructure.TIM_Prescaler = 5;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
    TIM_SelectOnePulseMode(TIM9,TIM_OPMode_Single);
    TIM_Cmd(TIM9,ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_Init( &NVIC_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_Init( &NVIC_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Init( &NVIC_InitStructure );

//-----------------------------------------------Start_Shark-------------------------------
    spi_init(1);

    for(uint32_t d = 0 ; d < 0x1ffffff ; d++)NOP();

	SPI_NSSInternalSoftwareConfig(SPI2 , SPI_NSSInternalSoft_Reset);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2 , 0);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	for(size_t i = 0 ; i < 3 ; i++)
		{
			while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
			SPI_I2S_SendData(SPI2 , 0);
		}
if(cab_type != 2)
{
	extern uint8_t _binary_Pangaea_CP100FX_1_ldr_start ;
	extern uint8_t _binary_Pangaea_CP100FX_1_ldr_end ;
	size_t _binary_Pangaea_CP100FX_1_ldr_size = (size_t)(&_binary_Pangaea_CP100FX_1_ldr_end - &_binary_Pangaea_CP100FX_1_ldr_start) ;
	for(size_t i = 0 ; i < _binary_Pangaea_CP100FX_1_ldr_size ; i++)
	{
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2 , (&_binary_Pangaea_CP100FX_1_ldr_start)[i]);
	}
}
else {
	extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_start ;
	extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_end ;
	size_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_size = (size_t)(&_binary_Pangaea_CP100FX_1_duble_fir_ldr_end - &_binary_Pangaea_CP100FX_1_duble_fir_ldr_start) ;
	for(size_t i = 0 ; i < _binary_Pangaea_CP100FX_1_duble_fir_ldr_size ; i++)
	{
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2 , (&_binary_Pangaea_CP100FX_1_duble_fir_ldr_start)[i]);
	}
}

    GPIO_SetBits(GPIOA,GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //  dsp_cs
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	spi_init(0);

	for(uint32_t d = 0 ; d < 0x3ffffff ; d++)NOP();

	GPIO_SetBits(GPIOA,GPIO_Pin_10);
//-----------------------------------------------Start_screen------------------------------------
    oled023_1_disp_init();
    extern volatile uint8_t ind_en;
   	ind_en = 0;
   	disp_start(0);
//------------------------------------------------Start codec-------------------------------------
   	const uint16_t fff[3] = {0xa103,0xa381,0xa265};
   	for(int i = 0 ; i < 3 ; i++)
   	{
   	    send_codec(fff[i]);
   	}
   	void load_mass_imp(void);
    load_mass_imp();
}
//---------------------------------------------------------------------------
void eepr_init(void)
{
	  extern uint8_t sys_para[];
	  sys_para[126] = 127;
	  DIR dir;
	  FATFS fs;
	  FIL file;
	  UINT file_size;
	  FRESULT res;
	  if(f_mount ( &fs , "1:",  1) != FR_OK)
		  {
		    f_mkfs("1:",0,0);
		    f_setlabel("PANGEA");
		    f_mount ( &fs , "1:",  1);
		  }
	  res = f_opendir(&dir,"1:PRESETS");
	  if(res == FR_OK)f_closedir(&dir);
	  else f_mkdir("1:PRESETS");
	  res = f_open(&file, "1:system.pan" , FA_READ | FA_WRITE | FA_OPEN_ALWAYS) ;
	  if(f_size(&file) == 0)f_write(&file , sys_para , 512 ,  &file_size);
	  f_read(&file, sys_para , 512 , &file_size);
	  f_close(&file);
	  f_mount(0, "1:", 0);
	  if(!sys_para[midi_pc_ind])
	  {
		  for(uint8_t i = 0 ; i < 128 ; i++)
		  {
			  if(i < 99)sys_para[i + 128] = i;
			  else sys_para[i + 128] = i - 99;
		  }
		  sys_para[midi_pc_ind] = 1;
		  void write_sys(void);
		  write_sys();
	  }
}
volatile uint8_t encoder_state;
volatile uint8_t encoder_state1;
volatile uint8_t encoder_knob;

extern "C" void EXTI15_10_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line14))
	{
	   uint16_t a = GPIOC->IDR & GPIO_Pin_13;
	   EXTI_ClearITPendingBit(EXTI_Line14);
	   if(drebezg(EXTI_Line14) == 1)
	   {
		   if(a)encoder_state = 1;
		   else encoder_state = 2;
		   encoder_state1 = 1;
		   TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE);
		   CSTask->Give();
	   }
	}
	if(EXTI_GetITStatus(EXTI_Line15))
	{
	   EXTI_ClearITPendingBit(EXTI_Line15);
	   if(drebezg(EXTI_Line15) == 1)
	   {
		   encoder_knob = 1;
	 	   tim5_fl = 1;
	 	   CSTask->Give();
	   }
	}
}
const uint16_t bpm_time[] = {2727, 2609, 2500, 2400, 2308, 2222, 2143, 2069, 2000, 1935, 1875, 1818, 1765, 1714, 1667, 1622, 1579, 1538,
		 1500, 1463,1429, 1395, 1364, 1333, 1304, 1277, 1250, 1224, 1200, 1176, 1154, 1132, 1111, 1091, 1071, 1053, 1034, 1017, 1000, 984,
		 968, 952, 938, 923, 909, 896, 882, 870, 857, 845, 833, 822, 811, 800, 789, 779, 769, 759, 750, 741, 732, 723, 714, 706,
		 698, 690, 682, 674, 667, 659, 652, 645, 638, 632, 625, 619, 612, 606, 600, 594, 588, 583, 577, 571, 566, 561, 556, 550,
		 545, 541, 536, 531, 526, 522, 517, 513, 508, 504, 500, 496, 492, 488, 484, 480, 476, 472, 469, 465, 462, 458, 455, 451,
		 448, 444, 441, 438, 435, 432, 429, 426, 423, 420, 417, 414, 411, 408, 405, 403, 400, 397, 395, 392, 390, 387, 385, 382,
		 380, 377, 375, 373, 370, 368, 366, 364, 361, 359, 357, 355, 353, 351, 349, 347, 345, 343, 341, 339, 337, 335, 333, 331,
		 330, 328, 326, 324, 323, 321, 319, 317, 316, 314, 313, 311, 309, 308, 306, 305, 303, 302, 300, 299, 297, 296, 294, 293,
		 291, 290, 288, 287, 286, 284, 283, 282, 280, 279, 278, 276, 275, 274, 273, 271,270,269,268,267,265,264,263,262,261,260,
		 259,258,256,255,254,253,252,251,250};
