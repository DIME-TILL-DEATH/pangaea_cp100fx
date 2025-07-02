/*
 * display.h
 *
 *  Created on: Aug 30, 2014
 *      Author: s
 */

#ifndef __MIDI_SEND_H__
#define __MIDI_SEND_H__

#include "appdefs.h"

extern volatile uint8_t program_change_midi;
extern volatile uint8_t key_midi[];
extern volatile uint8_t key_midi1[];

extern volatile uint16_t adc_bu;

class TMidiSendTask: public TTask
{
public:

	TMidiSendTask();

	inline void Give()
	{
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			sem->GiveFromISR(&HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
		}
		else
			sem->Give();
	}

	inline void prog_ch_midi_start()
	{
		program_change_midi = 1;
	}
	inline void key_midi_start(uint8_t num, uint8_t val)
	{
		key_midi[num] = val;
	}
	inline void key_midi_start1(uint8_t num, uint8_t val)
	{
		key_midi1[num] = val;
	}
	//----------------------------------------------------------------------------------------
private:
	void Code();

	TSemaphore *sem;
};

extern volatile uint8_t pc_mute_fl;

extern TMidiSendTask *MidiSendTask;

#endif /* __MIDI_SEND_H__ */
