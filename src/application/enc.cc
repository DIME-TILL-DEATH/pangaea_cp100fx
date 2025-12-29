#include "appdefs.h"
#include "enc.h"
#include "eepr.h"
#include "BF706_send.h"
#include "cs.h"
#include "cc.h"
#include "fs.h"
#include "display.h"
#include "sd_test.h"
#include "midi_send.h"
#include "allFonts.h"

#include "system.h"

#include "usb.h"

#include "gui_task.h"

#include "preset.h"
#include "footswitch.h"

#include "tunermenu.h"

#include "system.h"

volatile uint8_t contr_kn[3];
volatile uint8_t contr_kn1[3];
volatile uint8_t contr_pr[3];
//volatile uint8_t usb_flag = 0;

uint8_t k_up = 0;
uint8_t k_down = 0;

uint8_t k_att = 0;
uint8_t k_sys = 0;
uint8_t k_master = 0;
uint8_t k_master_eq = 0;
uint8_t k_tuner = 0;

volatile uint16_t key_reg_in;
volatile uint8_t key_reg_out[2] = {0, 0};
volatile uint16_t key_reg;
volatile uint8_t num_key_prog;

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

void fsw_press_execute(uint8_t num)
{
	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->keyUp();
		CSTask->Give();
	}
	else
	{
		switch(sys_para[System::FSW1_PRESS_TYPE + num])
		{
			case Footswitch::Default:
				if(currentMenu->menuType() == MENU_MAIN)
				{
					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);

					switch(num)
					{
						case 0:
						{
							mainMenu->presetDown();

							if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
									|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							}
							CSTask->Give();
							break;
						}
						case 1:
						{
							if(!sys_para[System::SWAP_SWITCH])
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								CSTask->Give();
							}
							else
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								}
								CSTask->Give();
							}
						}
						break;
						case 2:
							if(!sys_para[System::SWAP_SWITCH])
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_PRESS_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_PRESS_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								}
								CSTask->Give();
							}
							else
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							}
						break;
					}
				}
			break;
			case Footswitch::Controller:
				if(!contr_kn[num])
				{
					contr_kn[num] = currentPreset.modules.rawData[fo1 + num] = 1;
					ext_data = 127;
				}
				else
				{
					contr_kn[num] = currentPreset.modules.rawData[fo1 + num] = 0;
					ext_data = 0;
				}

				if(currentMenu->menuType() == MENU_MAIN)
					DisplayTask->IndFoot(num, contr_kn[num]); // refresh в конце

				if(sys_para[System::FSW1_CTRL_PRESS_CC + num])
				{
					MidiSendTask->key_midi_start(num, contr_kn[num] + 1);
					MidiSendTask->Give();
				}

				ext_sourc = num + 2;
				ext_fl = 1;
				CCTask->Give();
			break;

			case Footswitch::Tuner:
				currentMenu->showChild(new TunerMenu(currentMenu));
				CSTask->Give();
			break;

			default: // Preset maps
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(num_key_prog == num)
						contr_pr[num]++;
					if(contr_pr[num] > (sys_para[System::FSW1_PRESS_TYPE + num] - 3))
						contr_pr[num] = 0;

					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					mainMenu->presetChoose(sys_para[System::FSW1_PRESS_PR1 + num * 4 + contr_pr[num]]);
					num_key_prog = num;

					if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
						{
							encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
						}
						else
						{
							if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default) encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							else mainMenu->refresh();
						}
					}
					else
					{
						mainMenu->refresh();
					}
					CSTask->Give();
				}
			break;
		}
	}
}
void fsw_hold_execute(uint8_t num)
{
	if(currentMenu->menuType() == MENU_TUNER)
	{
		currentMenu->keyUp();
		CSTask->Give();
	}
	else
	{
		switch(sys_para[System::FSW1_HOLD_TYPE + num])
		{
			case Footswitch::Default:
			{
				if(currentMenu->menuType() == MENU_MAIN)
				{
					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);

					switch(num)
					{
						case 0:
						{
							mainMenu->presetDown();

							if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
									|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							}
							CSTask->Give();
							break;
						}
						case 1:
						{
							if(!sys_para[System::SWAP_SWITCH])
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								CSTask->Give();
							}
							else
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								}
								CSTask->Give();
							}
						}
						break;
						case 2:
							if(!sys_para[System::SWAP_SWITCH])
							{
								mainMenu->presetUp();

								if((sys_para[System::FSW2_HOLD_TYPE] && !sys_para[System::SWAP_SWITCH])
										|| (sys_para[System::FSW3_HOLD_TYPE] && sys_para[System::SWAP_SWITCH]))
								{
									encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
								}
								CSTask->Give();
							}
							else
							{
								encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							}
						break;
					}
				}
				break;
			}
			case Footswitch:: Controller:
			{
				if(!contr_kn1[num])
				{
					contr_kn1[num] = currentPreset.modules.rawData[fo11 + num] = 1;
					ext_data = 127;
				}
				else
				{
					ext_data = contr_kn1[num] = currentPreset.modules.rawData[fo11 + num] = 0;
				}

				if(currentMenu->menuType() == MENU_MAIN)
					DisplayTask->IndFoot(num, contr_kn1[num]);

				if(sys_para[System::FSW1_CTRL_HOLD_CC + num])
				{
					MidiSendTask->key_midi_start1(num, contr_kn1[num] + 1);
					MidiSendTask->Give();
				}
				ext_sourc = sys_para[System::FSW1_CTRL_HOLD_CC + num] + 4;
				ext_fl = 1;
				CCTask->Give();
			break;
			}

			case Footswitch::Tuner:
				currentMenu->showChild(new TunerMenu(currentMenu));
				CSTask->Give();
			break;

			default:
				if(currentMenu->menuType() == MENU_MAIN)
				{
					if(contr_pr[num] > (sys_para[System::FSW1_HOLD_TYPE + num] - 3))
						contr_pr[num] = 0;

					MainMenu* mainMenu = static_cast<MainMenu*>(currentMenu);
					mainMenu->presetChoose(sys_para[System::FSW1_HOLD_PR1 + num * 4 + contr_pr[num]++]);

					if(sys_para[System::FSW2_HOLD_TYPE] != Footswitch::FswType::Default)
					{
						if(sys_para[System::FSW2_MODE] == Footswitch::Single)
						{
							encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
						}
						else
						{
							if(sys_para[System::FSW2_PRESS_TYPE] != Footswitch::FswType::Default) encoder_knob_pressed = 1; // Thread sync mainMenu->presetConfirm();
							else mainMenu->refresh();
						}
					}
					else
					{
						mainMenu->refresh();
					}
					CSTask->Give();
				}
			break;
		}
	}
}

uint8_t kn2_in_fl = 0;

uint8_t fsw1_in_fl = 0;
uint8_t fsw2_in_fl = 0;
uint8_t fsw3_in_fl = 0;
uint8_t fsw1_in_fl1 = 0;
uint8_t fsw2_in_fl1 = 0;
uint8_t fsw3_in_fl1 = 0;

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

		if((key_reg != 31212) && (!kn2_in_fl) && (!fsw1_in_fl) && (!fsw2_in_fl) && (!fsw3_in_fl))
		{
			tim_start(0xf700);
			switch(key_reg)
			{
				case 14828:
					k_up = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31204:
					k_down = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 23020:
					k_att = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31148:
					k_sys = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 31180:
					k_tuner = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 27116:
					k_master = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 29164:
					k_master_eq = 1;
					CSTask->Give();
					kn2_in_fl = 1;
				break;
				case 30956:
					fsw1_in_fl = 1;
					if(sys_para[System::FSW1_MODE] == Footswitch::FswMode::Single)
					{
						fsw_press_execute(0);
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
					fsw2_in_fl = 1;
					if(sys_para[System::FSW2_MODE] == Footswitch::FswMode::Single)
					{
						fsw_press_execute(1);
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
					fsw3_in_fl = 1;
					if(!sys_para[System::FSW3_MODE])
					{
						fsw_press_execute(2);
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
			blinkFlag_fl = 1;
		}

		if(key_reg == 31212)
		{
			if(sys_para[System::FSW1_MODE] == Footswitch::Double)
			{
				if(fsw1_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						fsw_press_execute(0);
					tim_start(0xf700);
					fsw1_in_fl = 0;
				}
			}

			if(sys_para[System::FSW2_MODE] == Footswitch::Double)
			{
				if(fsw2_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						fsw_press_execute(1);
					tim_start(0xf700);
					fsw2_in_fl = 0;
				}
			}

			if(sys_para[System::FSW3_MODE] == Footswitch::Double)
			{
				if(fsw3_in_fl)
				{
					TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
					if(!tim3_end_fl)
						fsw_press_execute(2);
					tim_start(0xf700);
					fsw3_in_fl = 0;
				}
			}
		}

		if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
		{
			//TIM_ClearFlag(TIM3,TIM_FLAG_Update);
			if(key_reg == 31212)
				kn2_in_fl = fsw1_in_fl = fsw2_in_fl = fsw3_in_fl = tim3_end_fl = 0;
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

extern "C" void DMA1_Stream5_IRQHandler()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
	DMA_ClearITPendingBit(DMA1_Stream6, DMA_IT_TCIF6);
	key_reg = key_reg_in & 0x7fff;
	DMA_Cmd(DMA1_Stream6, DISABLE);
	DMA1_Stream6->NDTR = 2;
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

extern "C" void TIM3_IRQHandler()
{
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	tim3_end_fl = 1;
	if(fsw1_in_fl || fsw2_in_fl || fsw3_in_fl)
	{
		if(fsw1_in_fl)
		{
			fsw1_in_fl1 = 1;
			fsw1_in_fl = 0;
			fsw_hold_execute(0);
			fsw1_in_fl1 = 0;
		}
		if(fsw2_in_fl)
		{
			fsw2_in_fl1 = 1;
			fsw2_in_fl = 0;
			fsw_hold_execute(1);
			fsw2_in_fl1 = 0;
		}
		if(fsw3_in_fl)
		{
			fsw3_in_fl1 = 1;
			fsw3_in_fl = 0;
			fsw_hold_execute(2);
			fsw3_in_fl1 = 0;
		}
	}
}

uint16_t mediann_tap(uint16_t *array, int length)  // массив и его длина
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
