#include "adc.h"

#include "gpio.h"
#include "adc.h"

#include "system.h"
#include "preset.h"
#include "controller.h"

#include "controllers_task.h"
#include "midi_task.h"
#include "sharc_task.h"
#include "ui_task.h"

volatile uint16_t adc_low;
volatile uint16_t adc_high;
volatile uint16_t adc_val;
volatile float adc_val1;

volatile uint8_t adc_inv_fl = 0;

void ADC_LoadCal(void)
{
	adc_low = sys_para[System::EXPR_CAL_MIN_HI];
	adc_low |= sys_para[System::EXPR_CAL_MIN_LO]<<8;
	adc_high = sys_para[System::EXPR_CAL_MAX_HI];
	adc_high |= sys_para[System::EXPR_CAL_MAX_LO]<<8;
	adc_inv_fl = 0;
	if(adc_high < adc_low)
	{
		volatile uint16_t a = adc_high;
		adc_high = adc_low;
		adc_low = a;
		adc_inv_fl = 1;
	}
	adc_val = adc_high-adc_low;
	adc_val1 = 127.0f/adc_val;
}


void ADC_SetState(uint8_t state)
{
	if(state)
	{
		ADC_LoadCal();
		HW_AdcPinInit();

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

volatile uint16_t adc_bu;
volatile uint16_t adc_bu_cont;
volatile uint16_t adc_bu1[2];
volatile uint16_t adc_bu2;
uint16_t adc_buff[32];
volatile uint8_t adc_po;
volatile uint8_t adc_point = 0;
void EXPR_Routine(void)
{
	if(sys_para[System::EXPR_CC_NUM] > 0)
	{
		MidiTask->exprSend(adc_bu2);
	}

	if((sys_para[System::EXPR_TYPE] & 0x7f) < EXPR_TYPE_CC_STD)
	{
		SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, adc_bu2);
		UITask->refreshMenu();
	}
	else
	{
		ControllersTask->extCommand(Controller::Src::Expression, adc_bu2);
	}
	adc_bu1[1] = adc_bu1[0];
	adc_bu1[0] = adc_bu2;
}

void EXPR_StoreLevel()
{
	if((sys_para[System::EXPR_TYPE] & 0x7f) < EXPR_TYPE_CC_STD)
	{
		if(sys_para[System::EXPR_STORE_LEVEL])
			SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, adc_bu2);
		else
			SharcTask->setParameter(DSP_ADDRESS_MASTER_VOLUME_CONTROL, 127);
	}
	adc_bu1[1] = adc_bu1[0];
	adc_bu1[0] = adc_bu2;
}

uint16_t mediann(uint16_t *array, int length)  // массив и его длина
{
	uint16_t slit = length / 2;
	for(uint16_t i = 0; i < length; i++)
	{
		uint16_t s1 = 0, s2 = 0;
		uint16_t val = array[i];
		for(int j = 0; j < length; j++)
		{
			if(array[j] < val)
			{
				if(++s1 > slit)
					goto aaa;
			}
			else if(array[j] > val)
			{
				if(++s2 > slit)
					goto aaa;
			}
		}
		return val;
aaa:	;
	}
	return 0;  // чистая формальность, досюда исполнение никогда не доходит
}

extern "C" void ADC_IRQHandler()
{
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	adc_bu = ADC1->DR;
	adc_bu_cont = adc_bu;
	//----------------------------------------------------------------------------------
	int a = adc_bu_cont - adc_low;
	if(a < 0)
		a = 0;
	adc_bu_cont = a * adc_val1;
	if(adc_bu_cont > 127)
		adc_bu_cont = 127;
	if(adc_inv_fl)
		adc_bu_cont = 127 - adc_bu_cont;

	adc_buff[adc_po++] = adc_bu_cont;
	if(adc_po == 32)
		adc_po = 0;
	adc_bu2 = mediann(adc_buff, 32);

	if(adc_bu2 != adc_bu1[0])
	{
		if((adc_bu2 != adc_bu1[1]) && (sys_para[System::EXPR_TYPE] & 0x80))
		{
			if(adc_point++ == 100)
			{
				adc_point = 0;
				EXPR_Routine();
			}
		}
	}
	if((sys_para[System::EXPR_TYPE] & EXPR_TYPE_VOL_STD) || (sys_para[System::EXPR_TYPE] & EXPR_TYPE_CC_STD))
		ADC_RegularChannelConfig(ADC1, 10, 1, ADC_SampleTime_480Cycles);
	else
		ADC_RegularChannelConfig(ADC1, 11, 1, ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv (ADC1);
}
