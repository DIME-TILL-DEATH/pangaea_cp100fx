#include "eepr.h"
#include "footswitch.h"
#include "system.h"
#include "init.h"
#include "io_task.h"

#include "sdtest_task.h"
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

volatile uint16_t key_reg;


TIOTask *IOTask;
//------------------------------------------------------------------------------
TIOTask::TIOTask() :
		TTask()
{
	queue = new TQueue(8, sizeof(TIOCmd));
}

TIOTask::~TIOTask()
{
	delete queue;
}



void TIOTask::Code()
{
	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_IOTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIOCmd cmd;
	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);
		//------------------------------------------Keys------------------------------------------------
		switch(cmd.type)
		{
			case IO_FSW_SINGLE_MODE_PRESS:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Single)
					Footswitch::press_execute(cmd.fswEvents.fsw);
				break;
			}

			case IO_FSW_DUAL_MODE_PRESS:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Double)
					Footswitch::press_execute(cmd.fswEvents.fsw);
				break;
			}

			case IO_FSW_DUAL_MODE_HOLD:
			{
				if(sys_para[System::FSW1_MODE + cmd.fswEvents.fsw] == Footswitch::FswMode::Double)
					Footswitch::hold_execute(cmd.fswEvents.fsw);
				break;
			}
		}

//----------------------------------------------------LED FX-----------------------------------------------
		uint8_t isLedOn = 0;
		uint16_t enabledMask = 0;
		enabledMask = (1 << ENABLE_AMP) | (1 << ENABLE_PREAMP) | (1 << ENABLE_CAB);
		for(uint8_t i = 0; i < 14; i++)
		{
			if(!((enabledMask >> i) & 0x1))
				isLedOn += currentPreset.modules.rawData[i];
		}

		if(isLedOn)
			GPIO_SetBits(GPIOB, GPIO_Pin_14);
		else
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	}
}

void ISR_encoder_read()
{
	TEncoderEvents encEvents;
	encEvents.pressed = 0;
	encEvents.updated = 0;

	if(EXTI_GetITStatus (EXTI_Line14))
	{
		uint16_t a = GPIOC->IDR&GPIO_Pin_13;
		EXTI_ClearITPendingBit(EXTI_Line14);

		if(drebezg(EXTI_Line14)==1)
		{
			if(a) encEvents.state = ENC_COUNTERCLOCKWISE_STEP;
			else encEvents.state = ENC_CLOCKWISE_STEP;

			encEvents.updated = 1;

			UITask->encoderEvents(encEvents);
		}
	}

	if(EXTI_GetITStatus (EXTI_Line15))
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
		if(drebezg(EXTI_Line15)==1)
		{
			encEvents.pressed = 1;
			UITask->encoderEvents(encEvents);
		}
	}
}

volatile TFSWNum holdedFsw = TFSWNum::FSW_NONE;
volatile bool keysHold = false;
void ISR_buttons_read()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);

	key_reg = ~((key_reg_in & 0x7fff) ^ KEY_NO_PRESS_MASK);

	TKeysEvents keyEvents;
	keyEvents.hold = keysHold;

	keyEvents.keyUp = (key_reg >> KEY_UP_POS) & 0x1;
	keyEvents.keyDown = (key_reg >> KEY_DOWN_POS) & 0x1;
	keyEvents.key1 = (key_reg >> KEY1_POS) & 0x1;
	keyEvents.key2 = (key_reg >> KEY2_POS) & 0x1;
	keyEvents.key3 = (key_reg >> KEY3_POS) & 0x1;
	keyEvents.key4 = (key_reg >> KEY4_POS) & 0x1;
	keyEvents.key5 = (key_reg >> KEY5_POS) & 0x1;

	if(UITask) UITask->keysEvents(keyEvents);

	TFswEvents fswEvents;

	fswEvents.fsw = TFSWNum::FSW_NONE;
	if((key_reg >> FSW_DOWN_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_DOWN;
	if((key_reg >> FSW_CONFIRM_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_CONFIRM;
	if((key_reg >> FSW_UP_POS) & 0x1) fswEvents.fsw = TFSWNum::FSW_UP;

	if(IOTask)
	{
		if(holdedFsw != fswEvents.fsw)
		{
			if(fswEvents.fsw != TFSWNum::FSW_NONE)	// press in
			{
				IOTask->fswSinglePressed(fswEvents);

				TIM_ClearFlag(TIM3, TIM_FLAG_Update);
				TIM_SetCounter(TIM3, sys_para[System::FSW_SPEED] * (8000.0f / 127.0f) + 55000);
				TIM_Cmd(TIM3, ENABLE);
			}
			else	// press out
			{
				if(TIM3->CR1 & TIM_CR1_CEN)
				{
					fswEvents.fsw = holdedFsw;
					IOTask->fswDualPressed(fswEvents);

					TIM_Cmd(TIM3, DISABLE);
				}
			}
		}
	}
	holdedFsw = fswEvents.fsw;

	if(key_reg) keysHold = true;
	else keysHold = false;

	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

void ISR_fsw_hold_timer()
{
	if(holdedFsw != TFSWNum::FSW_NONE)
	{
		TFswEvents fswEvents;
		fswEvents.fsw = holdedFsw;
		IOTask->fswDualHolded(fswEvents);
	}
//	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	TIM_SetCounter(TIM3, 0);
	TIM_Cmd(TIM3, DISABLE);
}


