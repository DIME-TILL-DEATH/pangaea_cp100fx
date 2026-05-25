#include "io_task.h"

#include "adc.h"
#include "serial.h"
#include "led.h"

#include "preset.h"
#include "serial.h"
#include "footswitch.h"
#include "system.h"

#include "ui_task.h"

#define KEY_NO_PRESS_MASK 0x8613

#define KEY_UP_POS		14
#define KEY_DOWN_POS	3
#define KEY1_POS	13
#define KEY2_POS	12
#define KEY3_POS	11
#define KEY4_POS	6
#define KEY5_POS	5

#define FSW_DOWN_POS	8
#define FSW_CONFIRM_POS	7
#define FSW_UP_POS	2


TIOTask *IOTask = nullptr;
//------------------------------------------------------------------------------
TIOTask::TIOTask() :
		TTask()
{
	queue = new TQueue(16, sizeof(TIOCmd));
}

TIOTask::~TIOTask()
{
	delete queue;
}

void TIOTask::Code()
{
	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_IOTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	HW_ReadKeysEnable();
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	ADC_SetState(sys_para[System::EXPR_TYPE] & 0x80);

	TIOCmd cmd;
	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);
		//------------------------------------------Keys------------------------------------------------
		switch(cmd.type)
		{
			case IO_LED_TASK:
			{
				if(!mainMenu) break;

				uint8_t isLedOn = 0;
				uint16_t enabledMask = 0;
				uint8_t* arrModulesEnabled;
				enabledMask = (1 << ENABLE_AMP) | (1 << ENABLE_PREAMP) | (1 << ENABLE_CAB);

				if(mainMenu->preselectedPresetNum() != currentPresetNumber)
				{
					arrModulesEnabled = (uint8_t*)&(mainMenu->selectedPresetBrief()->paramData.switches);
				}
				else
				{
					arrModulesEnabled = (uint8_t*)&currentPreset.paramData.switches;
				}


				for(uint8_t i = 0; i < 14; i++)
				{
					if(!((enabledMask >> i) & 0x1))
						isLedOn += arrModulesEnabled[i];//currentPreset.modulesBuf[i];
				}

				if(isLedOn)
					LED_SetState(TLedType::LED_FX_RED, ENABLED);
				else
					LED_SetState(TLedType::LED_FX_RED, DISABLED);
				break;
			}
		}
	}
}

uint8_t drebezg(uint32_t line)
{
	uint8_t sss = 0;  // ИСПРАВЛЕНО: Инициализировать в 0
	
	// ИСПРАВЛЕНО: Защита от переключения EXTI без дебаунса
	// Проблема: EXTI граница переключалась ВСЕГДА, независимо от TIM9 флага
	// Это создавало ложные события при каждом вызове функции
	TIM_Cmd(TIM9, DISABLE);
	
	if(TIM_GetFlagStatus(TIM9, TIM_FLAG_Update) != SET)
	{
		// Таймер не отсчитал достаточное время - игнорируем событие
		TIM_Cmd(TIM9, ENABLE);
		return 0;
	}
	
	// Таймер отсчитал - это стабильное изменение
	if((EXTI->FTSR&line) != 0)  // Если текущая граница - FALLING
	{
		sss = 1;
		EXTI->FTSR &= ~line;   // Отключить FALLING
		EXTI->RTSR |= line;    // Включить RISING
	}
	else  // Если текущая граница - RISING
	{
		sss = 2;
		EXTI->RTSR &= ~line;   // Отключить RISING
		EXTI->FTSR |= line;    // Включить FALLING
	}
	
	TIM_SetCounter(TIM9, 0);
	TIM_ClearFlag(TIM9, TIM_FLAG_Update);
	TIM_Cmd(TIM9, ENABLE);
	return sss;
}

void ISR_encoder_read()
{
	if(EXTI_GetITStatus (EXTI_Line14))
	{
		uint16_t a = GPIOC->IDR & GPIO_Pin_13;
		EXTI_ClearITPendingBit(EXTI_Line14);

		if(drebezg(EXTI_Line14)==1)
		{
			TEncoderEvents encEvents = {};
			if(a) encEvents.state = ENC_COUNTERCLOCKWISE_STEP;
			else encEvents.state = ENC_CLOCKWISE_STEP;

			encEvents.updated = 1;

			if(UITask) UITask->encoderEvents(encEvents);
		}
	}

	if(EXTI_GetITStatus (EXTI_Line15))
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
		if(drebezg(EXTI_Line15)==1)
		{
			TEncoderEvents encEvents = {};
			encEvents.pressed = 1;
			if(UITask) UITask->encoderEvents(encEvents);
		}
	}
}

volatile TFSWNum holdedFsw = TFSWNum::FSW_NONE;
volatile uint16_t prevKeyReg = KEY_NO_PRESS_MASK;
volatile uint8_t debounce_counter = 0;  // ДОБАВЛЕНО: Счетчик для дебаунса

void ISR_buttons_read()
{
	uint16_t key_reg = ~((HW_KeyInValue() & 0x7fff) ^ KEY_NO_PRESS_MASK);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);

	IOTask->ledTask();

	if(key_reg != prevKeyReg)
	{
		debounce_counter = 0;
		prevKeyReg = key_reg;
	}
	else
	{
		if(debounce_counter < 5) debounce_counter++;

		if(debounce_counter == 5)
		{
			debounce_counter++;  // Увеличиваем счетчик, чтобы не обрабатывать события повторно
			
			TKeysEvents keyEvents = {};

			keyEvents.keyUp = (key_reg >> KEY_UP_POS) & 0x1;
			keyEvents.keyDown = (key_reg >> KEY_DOWN_POS) & 0x1;
			keyEvents.key1 = (key_reg >> KEY1_POS) & 0x1;
			keyEvents.key2 = (key_reg >> KEY2_POS) & 0x1;
			keyEvents.key3 = (key_reg >> KEY3_POS) & 0x1;
			keyEvents.key4 = (key_reg >> KEY4_POS) & 0x1;
			keyEvents.key5 = (key_reg >> KEY5_POS) & 0x1;

			if(UITask) UITask->keysEvents(keyEvents);

			TFswEvents fswEvents = {};
			fswEvents.fsw = TFSWNum::FSW_NONE;
			if((key_reg >> FSW_DOWN_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_DOWN;
			if((key_reg >> FSW_CONFIRM_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_CONFIRM;
			if((key_reg >> FSW_UP_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_UP;


			if(holdedFsw != fswEvents.fsw)
			{
				if(fswEvents.fsw != TFSWNum::FSW_NONE)	// press in
				{
					if(UITask) UITask->fswSinglePressed(fswEvents);

					TIM_ClearFlag(TIM3, TIM_FLAG_Update);
					TIM_SetCounter(TIM3, sys_para[System::FSW_SPEED] * (8000.0f / 127.0f) + 55000);
					TIM_Cmd(TIM3, ENABLE);
				}
				else	// press out
				{
					if(TIM3->CR1 & TIM_CR1_CEN)
					{
						fswEvents.fsw = holdedFsw;
						if(UITask) UITask->fswDualPressed(fswEvents);

						TIM_Cmd(TIM3, DISABLE);
					}
				}
			}

			holdedFsw = fswEvents.fsw;
		}
	}
}

void ISR_fsw_hold_timer()
{
	if(holdedFsw != TFSWNum::FSW_NONE)
	{
		TFswEvents fswEvents;
		fswEvents.fsw = holdedFsw;
		if(UITask) UITask->fswDualHolded(fswEvents);
	}
	TIM_SetCounter(TIM3, 0);
	TIM_Cmd(TIM3, DISABLE);
}
