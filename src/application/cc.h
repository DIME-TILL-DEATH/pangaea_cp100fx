#ifndef CC_H_

#include "appdefs.h"

class TCCTask: public TTask
{
public:
	TCCTask();

	void midiCommand(uint8_t source, uint8_t data);
	void extCommand(uint8_t source, uint8_t data);

private:
	void Code();

	typedef enum{
		CONTROLLER_MIDI,
		CONTROLLER_EXTERNAL,
		CONTROLLER_TEMPO
	}TCmd;

	typedef struct{
		uint8_t source;
		uint8_t data;
	}TMidiCmd;

	typedef struct{
		uint8_t source;
		uint8_t data;
	}TExternalCmd;

	typedef struct{

	}TTempoCmd;

	typedef struct{
		TCmd type;

		union
		{
			TMidiCmd midiCmd;
			TExternalCmd extCmd;
			TTempoCmd tempoCmd;
		};
	}TControllerCmd;

	TQueue::TQueueSendResult Command(TControllerCmd *cmd)
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
	TSemaphore *sem;
};

void controllerSetData(uint8_t adr, uint8_t data);

extern volatile uint8_t midi_b[3];
//extern uint8_t mid_fl;
//extern uint8_t ext_fl;
//extern uint8_t ext_f_fl;
//extern uint8_t ext_data;
//extern uint8_t ext_sourc;

extern volatile uint16_t adc_bu;
extern TCCTask *ControllersTask;

#endif /* CC_H_ */
