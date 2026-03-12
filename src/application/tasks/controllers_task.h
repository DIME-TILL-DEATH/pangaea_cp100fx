#ifndef CC_H_

#include "appdefs.h"

class TControllersTask: public TTask
{
public:
	TControllersTask();

	void midiCommand(uint8_t source, uint8_t data);
	void extCommand(uint8_t source, uint8_t data);
	void midiCalcTempo(uint16_t* data, uint8_t size);

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
		uint16_t* data;
		uint8_t size;
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

	static constexpr uint8_t maBufSize = 4;
	float maMidiClockBuf[maBufSize] = {500};
	uint8_t maMidiClockBufPos = 0;
};

void controllerSetData(uint8_t adr, uint8_t data);

extern volatile uint8_t midi_b[3];

extern volatile uint16_t adc_bu;
extern TControllersTask *ControllersTask;

#endif /* CC_H_ */
