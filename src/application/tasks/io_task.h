#ifndef __IO_TASK_H__
#define __IO_TASK_H__

#include "appdefs.h"

typedef enum
{
	ENC_COUNTERCLOCKWISE_STEP = 1,
	ENC_CLOCKWISE_STEP
}TEncStep;

typedef enum
{
	FSW_NONE = -1,
	FSW_DOWN = 0,
	FSW_CONFIRM,
	FSW_UP
}TFSWNum;

typedef struct{
	uint8_t keyUp; //k_up;
	uint8_t keyDown; //k_down;

	uint8_t key1; //k_att;
	uint8_t key2; //k_master;
	uint8_t key3; //k_master_eq;
	uint8_t key4; //k_sys;
	uint8_t key5; //k_tuner;

//	uint8_t hold;
}TKeysEvents;

typedef struct{
	TFSWNum fsw;
}TFswEvents;


typedef struct{
	uint8_t pressed;
	uint8_t	updated;
	TEncStep state;
}TEncoderEvents;

class TIOTask: public TTask
{
public:
	TIOTask();
	~TIOTask();

	typedef enum{
		IO_LED_TASK,
	}TIOCmdType;

	typedef struct{
		TIOCmdType type;

		union{
			TFswEvents fswEvents;
		};
	}TIOCmd;

	void ledTask(){
		TIOCmd cmd;
		cmd.type = IO_LED_TASK;
		Command(&cmd);
	};


private:
	void Code() override;

	TQueue::TQueueSendResult Command(TIOCmd *cmd)
	{
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			TQueue::TQueueSendResult result;
			result = queue->SendToBackFromISR(cmd, &HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
			return result;
		}
		else
		{
			return queue->SendToBack(cmd, 0);
		}
	}

	TQueue *queue;
};

void ISR_encoder_read();
void ISR_buttons_read();
void ISR_fsw_hold_timer();

extern TIOTask *IOTask;

#endif /*__IO_TASK_H__*/
