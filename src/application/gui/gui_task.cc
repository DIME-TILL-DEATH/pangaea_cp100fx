#include "gui_task.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"
#include "midi_send.h"
#include "init.h"

#include "system.h"

#include "modules.h"
#include "footswitch.h"
#include "controllers.h"
#include "preset.h"

#include "abstractmenu.h"

const uint8_t expr_menu[][10] = {"Type", "Calibrate", "CC#", "Store Lev"};
const float tap_time_coefs[6] = {1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 0.5f};

uint16_t midi_clk_send;

void tim5_start(uint8_t val)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_SetCounter(TIM4, 0xffff);
	blinkFlag_fl = val;
}

void clean_flag(void)
{
	k_up = k_down = k_sys = k_att = k_master = k_master_eq = encoder_knob_pressed = encoder_state_updated = k_tuner = 0;
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}

void gui(void)
{
	if(usb_flag && !usbMenu->isConnected() && currentMenu->menuType() != MENU_USB_SELECT)
	{
		currentMenu->showChild(usbMenu);
	}

	currentMenu->task();

	if(encoder_knob_pressed)
	{
		currentMenu->encoderPressed();
		clean_flag();
	}

	if(encoder_state_updated)
	{
		if(encoder_state==ENC_COUNTERCLOCKWISE_STEP)
		{
			currentMenu->encoderCounterClockwise();
		}

		if(encoder_state==ENC_CLOCKWISE_STEP)
		{
			currentMenu->encoderClockwise();
		}
		clean_flag();
	}

	if(k_up)
	{
		currentMenu->keyUp();
		clean_flag();
	}

	if(k_down)
	{
		currentMenu->keyDown();
		clean_flag();
	}

	if(k_att)
	{
		currentMenu->key1();
		clean_flag();
	}

	if(k_master)
	{
		currentMenu->key2();
		clean_flag();
	}

	if(k_master_eq)
	{
		currentMenu->key3();
		clean_flag();
	}

	if(k_sys)
	{
		currentMenu->key4();
		clean_flag();
	}

	if(k_tuner)
	{
		currentMenu->key5();
		clean_flag();
	}

	if(sys_para[System::TAP_TYPE] == System::TAP_TYPE_GLOBAL_MIDI)
	{
		extern volatile uint16_t midi_clk_buf[];
		uint16_t mediann_tap(uint16_t *array, int length);
		uint16_t c = mediann_tap((uint16_t*)midi_clk_buf, 64);
		c >>= 4;
		if((c<(midi_clk_send-7))||(c>(midi_clk_send+7)))
		{
			midi_clk_send = c;
			void send_midi_temp(uint16_t *c);
			send_midi_temp(&c);
		}
	}
}
//---------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler()
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	if(blinkFlag_fl)
		blinkFlag_fl = 0;
	else
		blinkFlag_fl = 1;

	if(!(currentMenu->menuType() == MENU_TUNER))
		CSTask->Give();
}
