#include "init.h"

#include "periphery.h"
#include "gpio.h"

#include "eepr.h"
#include "allFonts.h"
#include "AT45DB321.h"

#include "system.h"

#include "midi_task.h"

#include "dsp.h"
#include "tasks/filesystem_task.h"
#include "tasks/sdtest_task.h"
#include "tasks/spectrum_task.h"
#include "tasks/ui_task.h"


uint8_t cab_type = 0;

ad_data_t adc_data[2];
da_data_t dac_data[2];

volatile uint8_t key_reg_out[2] = {0, 0};
volatile uint16_t key_reg_in = 0xFFFF;

void adc_calib(void)
{
	adc_low = sys_para[System::EXPR_CAL_MIN_HI];
	adc_low |= sys_para[System::EXPR_CAL_MIN_LO]<<8;
	adc_high = sys_para[System::EXPR_CAL_MAX_HI];
	adc_high |= sys_para[System::EXPR_CAL_MAX_LO]<<8;
	extern volatile uint8_t adc_inv_fl;
	adc_inv_fl = 0;
	if(adc_high<adc_low)
	{
		volatile uint16_t a = adc_high;
		adc_high = adc_low;
		adc_low = a;
		adc_inv_fl = 1;
	}
	adc_val = adc_high-adc_low;
	adc_val1 = 127.0f/adc_val;
}
//-------------------------------------------Setting ADC------------------------------------------------
void adc_init(uint8_t state)
{
	if(state)
	{
		adc_calib();
		HW_adc_pin_init();
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		ADC_InitTypeDef ADC_InitStructure;
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
		NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

		NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
		NVIC_Init(&NVIC_InitStructure);

		ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
		ADC_Cmd(ADC1, ENABLE);
		ADC_SoftwareStartConv (ADC1);
	}
	else
	{
		ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
		ADC_Cmd(ADC1, DISABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
	}
}
//------------------------------------------------Send to codec-------------------------------------------
void send_codec(uint16_t data)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, data);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOC, GPIO_Pin_7);

	HW_delay(0xff);
}
//-------------------------------------------------SPI_init------------------------------------------------
void spi_init(uint8_t type)
{
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
	if(f_mount(&fs, "1:", 1)!=FR_OK)
	{
		f_mkfs("1:", 0, 0);
		f_setlabel("PANGEA");
		f_mount(&fs, "1:", 1);
	}
	res = f_opendir(&dir, "1:PRESETS");
	if(res==FR_OK)
		f_closedir(&dir);
	else
		f_mkdir("1:PRESETS");

	res = f_open(&file, "1:system.pan", FA_READ|FA_WRITE|FA_OPEN_ALWAYS);
	if(f_size(&file)==0)
	{
		sys_para[System::TUNER_EXTERNAL] = 0x81;
		f_write(&file, sys_para, 512, &file_size);
	}

	f_read(&file, sys_para, 512, &file_size);
	f_close(&file);
	f_mount(0, "1:", 0);
	if(!sys_para[System::MIDI_PC_IND])
	{
		for(uint8_t i = 0; i<128; i++)
		{
			if(i<99)
				sys_para[i+128] = i;
			else
				sys_para[i+128] = i-99;
		}
		sys_para[System::MIDI_PC_IND] = 1;
		void write_sys(void);
		write_sys();
	}
}
//---------------------------------------------------------------------------------------------------------
void init(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStruct;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockStructure;

	HW_delay(0xfffff);
	AT45DB321_Init();
	HW_delay(0xfffff);
	eepr_init();

//-------------------------------------------------GPIO_init-----------------------------------------------
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	HW_gpio_init();


	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource15);

	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;

	EXTI_InitStruct.EXTI_Line = EXTI_Line9;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line14;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Line = EXTI_Line15;
	EXTI_Init(&EXTI_InitStruct);

	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStruct.EXTI_Line = EXTI_Line8;
	EXTI_Init(&EXTI_InitStruct);

//----------------------------------------------------------I2S_init----------------------------------------------
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2|RCC_APB1Periph_SPI3, ENABLE);
	RCC_PLLI2SCmd (DISABLE);
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_StructInit(&USART_InitStructure);

	USART_InitStructure.USART_BaudRate = 31250;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

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

	DMA_Cmd(DMA1_Stream2, DISABLE);
	DMA_Cmd(DMA1_Stream7, DISABLE);
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA_DeInit (DMA1_Stream2);
	DMA_DeInit (DMA1_Stream7);
	DMA_DeInit (DMA1_Stream5);
	DMA_DeInit (DMA1_Stream6);

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

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&NVIC_InitStructure);


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5|
			RCC_APB1Periph_TIM6|RCC_APB1Periph_TIM14, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// FSW hold timer
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x1fff;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(TIM3, TIM_OPMode_Single);
	TIM_SetCounter(TIM3, 0);

	// Blink timer
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x200;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	// Running string timer
	TIM_TimeBaseStructure.TIM_Period = 0x4fff;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x750;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	// Encoder speed up timer
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 0xff;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);
	TIM_SetCounter(TIM6, 0);

	// Anti-drebezg timer
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 5;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(TIM9, TIM_OPMode_Single);
	TIM_Cmd(TIM9, ENABLE);

	// MIDI Timing clock timer
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_Prescaler = 840-1; //219; // tick = 10us
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	TIM_SelectOnePulseMode(TIM14, TIM_OPMode_Single);
	TIM_SetCounter(TIM14, 0);

	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

//-----------------------------------------------Start_Shark-------------------------------
	spi_init(1);

	HW_delay(0x1ffffff);

	SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Reset);
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	SPI_I2S_SendData(SPI2, 0);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	for(size_t i = 0; i<3; i++)
	{
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, 0);
	}

	cab_type = sys_para[System::CAB_SIM_CONFIG];

	cab1.data = &ccmCommonCabBuffer[0];
	cab2.data = &ccmCommonCabBuffer[4096 * 3];

#ifdef __STEREO_MOD__
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_start;
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_end;

	size_t _binary_Pangaea_CP100FX_1_ldr_size = (size_t)(
			&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
	for(size_t i = 0; i<_binary_Pangaea_CP100FX_1_ldr_size; i++)
	{
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, (&_binary_Pangaea_CP100FX_1_ldr_start)[i]);
	}
#endif

#ifdef __MONO_MOD__
	if(cab_type != CAB_CONFIG_STEREO)
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_end;

		size_t _binary_Pangaea_CP100FX_1_ldr_size = (size_t)(
				&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
		for(size_t i = 0; i<_binary_Pangaea_CP100FX_1_ldr_size; i++)
		{
			while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
			SPI_I2S_SendData(SPI2, (&_binary_Pangaea_CP100FX_1_ldr_start)[i]);
		}
	}
	else
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_end;

		size_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_size = (size_t)(
				&_binary_Pangaea_CP100FX_1_duble_fir_ldr_end-&_binary_Pangaea_CP100FX_1_duble_fir_ldr_start);
		for(size_t i = 0; i<_binary_Pangaea_CP100FX_1_duble_fir_ldr_size; i++)
		{
			while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
			SPI_I2S_SendData(SPI2, (&_binary_Pangaea_CP100FX_1_duble_fir_ldr_start)[i]);
		}
	}
#endif

	spi_init(0);

	HW_delay(0x3ffffff);

	GPIO_SetBits(GPIOA, GPIO_Pin_10);
//-----------------------------------------------StartScreen------------------------------------
	oled023_1_disp_init();
	extern volatile uint8_t ind_en;
	ind_en = 0;
	disp_start(0);
//------------------------------------------------Start codec-------------------------------------
	const uint16_t fff[3] = {0xa103, 0xa381, 0xa265};
	for(int i = 0; i<3; i++)
	{
		send_codec(fff[i]);
	}

	load_mass_imp();

	HW_pin_usb_init();
}


//==============================ISR EXTI===================================
extern "C" void EXTI15_10_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line8))
	{
		EXTI_ClearITPendingBit(EXTI_Line8);

		if(SDTestTask) SDTestTask->Give();
	}

	ISR_encoder_read();
}
//==============================ISR DMA================================

extern "C" void DMA1_Stream5_IRQHandler()
{
	ISR_buttons_read();
}

int32_t __CCM_BSS__ ccl;
int32_t __CCM_BSS__ ccr;
volatile uint32_t __CCM_BSS__ cl;
volatile uint32_t __CCM_BSS__ cr;
uint8_t dma_ht_fl;
uint8_t tun_del;
uint8_t tun_del_val;
extern "C" void DMA1_Stream2_IRQHandler()
{
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
	//GPIO_SetBits(GPIOC,GPIO_Pin_5);

	cr = ror16(adc_data[dma_ht_fl].left.sample);
	cl = ror16(adc_data[dma_ht_fl].right.sample);
	ccl = cl;
	ccl = ccl >> 8;
	ccr = cr;
	ccr = ccr >> 8;
//-------------------------------------------------------
	float in = ccl * 0.000000119f;

//---------------------Vol indicator----------------------------

	ind_out_l[0] = abs(ccl);
	if(ind_out_l[0] > ind_out_l[1])
		ind_out_l[1] = ind_out_l[0];


	DisplayTask->VolIndicatorTask();

//--------------------Tuner------------------------------
	if(currentMenu)
	{
		if(currentMenu->menuType() == MENU_TUNER)
		{
			SpectrumBuffsUpdate(COMP_Out(in));
			if(tun_del > tun_del_val)
			{
				tun_del = 0;

				DisplayTask->TunStrel();
			}
			tun_del++;
			return;
		}
	}

	if(SpectrumTask)
	{
		if(SpectrumTask->backgroundTunerEnabled)
		{
			SpectrumBuffsUpdate(COMP_Out(in));
			if(tun_del > tun_del_val)
			{
				tun_del = 0;
				if(SpectrumTask->samplesCount > 0)
				{
					SpectrumTask->samplesCount--;
					if(ind_out_l[0] > 1500)
						ConsoleTask->UnsafePrintF("tn get\r%s\r%d\n", SpectrumTask->note_name, SpectrumTask->cents);
					else
						ConsoleTask->UnsafePrintF("tn get\r-\r0\n");
				}
				else
				{
					SpectrumTask->backgroundTunerEnabled = false;

					DSP_GuiSendParameter(DSP_ADDRESS_TUN_PROC, 1, 0);
					GPIO_ResetBits(GPIOB, GPIO_Pin_11);
					send_codec(0xa103);
				}
			}
			tun_del++;
			return;
		}
	}

	GPIOB->BSRRH |= GPIO_Pin_11;

	dac_data[dma_ht_fl].left.sample = ror16(cr);
	dac_data[dma_ht_fl].right.sample = ror16(cr);
}

//==============================ISR Timers=============================

extern "C" void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	ISR_fsw_hold_timer();
}

extern "C" void TIM4_IRQHandler()
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	AbstractMenu::blinkRoutine();
	UITask->task();
}

extern "C" void TIM5_IRQHandler()
{
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

	UITask->runningString();
}

//==============================ISR USART================================
extern "C" void USART1_IRQHandler()
{
	ISR_midi_recieve();
}
