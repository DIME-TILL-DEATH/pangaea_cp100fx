#ifndef __ENC_H__
#define __ENC_H__

#include "appdefs.h"

typedef enum
{
	ENC_COUNTERCLOCKWISE_STEP = 1,
	ENC_CLOCKWISE_STEP
}TEncStep;

typedef struct{
	uint8_t keyUp; //k_up;
	uint8_t keyDown; //k_down;

	uint8_t key1; //k_att;
	uint8_t key2; //k_master;
	uint8_t key3; //k_master_eq;
	uint8_t key4; //k_sys;
	uint8_t key5; //k_tuner;

	uint8_t encPressed;
	uint8_t	encUpdated;
	TEncStep encState;

	uint8_t hold;
}TKeysEvents;

typedef struct{
	uint8_t pressed;
	uint8_t	updated;
	TEncStep state;
}TEncoderEvents;

class TIOTask: public TTask
{
public:
	TIOTask();

	void Give()
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

	void SetEnc(uint8_t val)
	{
		enc_run = val;
	}
private:
	void Code();
	TSemaphore *sem;

	uint8_t enc_run = 0;
};

void ISR_encoder_read();
void ISR_buttons_read();
void ISR_fsw_hold_timer();

extern TIOTask *IOTask;

#endif /*__ENC_H__*/
