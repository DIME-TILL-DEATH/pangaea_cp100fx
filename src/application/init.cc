#include "init.h"

#include "eepr.h"
#include "allFonts.h"
#include "AT45DB321.h"

#include "system.h"

#include "midi_task.h"

#include "dsp.h"

#include "filesystem_task.h"
#include "sdtest_task.h"
#include "spectrum_task.h"
#include "ui_task.h"

#include "periphery.h"
#include "serial.h"
#include "gpio.h"
#include "hw_timers.h"

uint8_t cab_type = 0;

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

//---------------------------------------------------------------------------

void init(void)
{
	oled023_1_disp_init();
	disp_start(0);
	HW_delay(0xfffff);

	AT45DB321_Init();
	HW_delay(0xfffff);

	EEPROM_start();

	HW_gpio_init();
	HW_exti_init();

	HW_i2s_init();
	HW_uart_init();

	HW_timers_init();

//-----------------------------------------------Load_Shark-------------------------------
	HW_spi_init(1);
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

	HW_spi_init(0);
	HW_delay(0x3ffffff);
	GPIO_SetBits(GPIOA, GPIO_Pin_10);

	cab1.data = &ccmCommonCabBuffer[0];
	cab2.data = &ccmCommonCabBuffer[4096 * 3];

//------------------------------------------------Start codec-------------------------------------
	const uint16_t fff[3] = {0xa103, 0xa381, 0xa265};
	for(int i = 0; i<3; i++)
	{
		send_codec(fff[i]);
	}

	EEPROM_load_all_ir();
	HW_pin_usb_init();
}


