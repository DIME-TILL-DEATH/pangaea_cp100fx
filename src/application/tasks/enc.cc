#include "enc.h"

#include "cs.h"
#include "sd_test.h"
#include "gui_task.h"

#include "eepr.h"
#include "footswitch.h"
#include "system.h"
#include "init.h"

#define KEY_NO_PRESS_MASK 0x8613

#define KEY_UP_POS		14
#define KEY_DOWN_POS	3
#define KEY1_POS	13
#define KEY2_POS	12
#define KEY3_POS	11
#define KEY4_POS	6
#define KEY5_POS	5

#define FSW_DOWN_POS	3
#define FSW_CONFIRM_POS	7
#define FSW_UP_POS	8

volatile uint8_t encoder_state;
volatile uint8_t encoder_state_updated;
volatile uint8_t encoder_knob_pressed;

volatile uint16_t key_reg;


TENCTask *ENCTask;
//------------------------------------------------------------------------------
TENCTask::TENCTask() :
		TTask()
{

}

void tim_start(uint16_t del)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_SetCounter(TIM3, del);
	TIM_Cmd(TIM3, ENABLE);
}

uint8_t fsw1_in_press_fl = 0;
uint8_t fsw2_in_press_fl = 0;
uint8_t fsw3_in_press_fl = 0;
uint8_t fsw1_in_hold_fl = 0;
uint8_t fsw2_in_hold_fl = 0;
uint8_t fsw3_in_hold_fl = 0;

uint8_t tim3_end_fl = 0;

void TENCTask::Code()
{
	while(!enc_run);

	TIM_Cmd(TIM3, ENABLE);

	while(1)
	{
		//---------------------------------------------Run string-------------------------------------
		if(currentMenu)
		{

			StringOutParam *runningString = currentMenu->getRunningString();
			if(runningString)
				runningString->task();
		}
		//------------------------------------------Keys------------------------------------------------

		if((key_reg != 31212) && (!fsw1_in_press_fl) && (!fsw2_in_press_fl) && (!fsw3_in_press_fl))
		{
			tim_start(0xf700);
			switch(key_reg)
			{

				case 30956:
					fsw1_in_press_fl = 1;
					if(sys_para[System::FSW1_MODE] == Footswitch::FswMode::Single)
					{
						Footswitch::press_execute(0);
					}
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[System::FSW_SPEED] * (8000.0f / 127.0f) + 55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //--------------------DOWN-----------------
				case 31084:
					fsw2_in_press_fl = 1;
					if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
					{
						Footswitch::press_execute(1);
					}
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[System::FSW_SPEED] * (8000.0f / 127.0f) + 55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //----------------CONFIRM----------
				case 31208:
					fsw3_in_press_fl = 1;
					if(sys_para[System::FSW3_MODE] == Footswitch::Single)
					{
						Footswitch::press_execute(2);
					}
					else
					{
						if(!tim3_end_fl)
						{
							tim_start(sys_para[System::FSW_SPEED] * (8000.0f / 127.0f) + 55000);
							TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
						}
					}
				break;    //--------------------UP-----------------
			}
		}

		if(key_reg == 31212)
		{
			if(sys_para[System::FSW1_MODE] == Footswitch::Double)
			{
				if(fsw1_in_press_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						Footswitch::press_execute(0);
					tim_start(0xf700);
					fsw1_in_press_fl = 0;
				}
			}

			if(sys_para[System::FSW2_MODE] == Footswitch::Double)
			{
				if(fsw2_in_press_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						Footswitch::press_execute(1);
					tim_start(0xf700);
					fsw2_in_press_fl = 0;
				}
			}

			if(sys_para[System::FSW3_MODE] == Footswitch::Double)
			{
				if(fsw3_in_press_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						Footswitch::press_execute(2);
					tim_start(0xf700);
					fsw3_in_press_fl = 0;
				}
			}
		}

		if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
		{
			if(key_reg == 31212)
				fsw1_in_press_fl = fsw2_in_press_fl = fsw3_in_press_fl = tim3_end_fl = 0;
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

//----------------------------------------------------Test SD------------------------------------------------
		if(EXTI_GetITStatus (EXTI_Line8))
		{
			EXTI_ClearITPendingBit(EXTI_Line8);
			SD_TESTTask->Give();
		}
	}
}

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

	if(CSTask) CSTask->keysEvents(keyEvents);

	if(key_reg) keysHold = true;
	else keysHold = false;

	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);
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

			CSTask->encoderEvents(encEvents);
		}
	}

	if(EXTI_GetITStatus (EXTI_Line15))
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
		if(drebezg(EXTI_Line15)==1)
		{
			encEvents.pressed = 1;
			CSTask->encoderEvents(encEvents);
		}
	}
}


void ISR_fsw_hold_timer()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);

	tim3_end_fl = 1;
	if(fsw1_in_press_fl || fsw2_in_press_fl || fsw3_in_press_fl)
	{
		if(fsw1_in_press_fl)
		{
			fsw1_in_hold_fl = 1;
			fsw1_in_press_fl = 0;
			Footswitch::hold_execute(0);
			fsw1_in_hold_fl = 0;
		}
		if(fsw2_in_press_fl)
		{
			fsw2_in_hold_fl = 1;
			fsw2_in_press_fl = 0;
			Footswitch::hold_execute(1);
			fsw2_in_hold_fl = 0;
		}
		if(fsw3_in_press_fl)
		{
			fsw3_in_hold_fl = 1;
			fsw3_in_press_fl = 0;
			Footswitch::hold_execute(2);
			fsw3_in_hold_fl = 0;
		}
	}
}


