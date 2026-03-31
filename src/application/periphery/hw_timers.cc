#include "hw_timers.h"

#include "ui_task.h"
#include "io_task.h"

void HW_TimersInit()
{
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
	TIM_TimeBaseStructure.TIM_Prescaler = 0x700;
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

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_Init(&NVIC_InitStructure);
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
