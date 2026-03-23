#ifndef SHARC_TASK_H_
#define SHARC_TASK_H_

#include "appdefs.h"
#include "sharc.h"

class TSharcTask: public TTask
{
public:
	TSharcTask();

	void setParameter(dsp_module_address_t moduleAddress, uint8_t parameterAddress_data, uint8_t data = 0);
private:
	void Code();

	typedef enum{
		SHARC_SET_PARAMETER
	}TCmd;

	typedef struct{
		dsp_module_address_t moduleAddress;
		uint8_t parameterAddress_data;  // address OR data
		uint8_t data;
	}TParameterCmd;

	typedef struct{
		TCmd type;

		union
		{
			TParameterCmd parameterCmd;

		};
	}TSharcCmd;

	TQueue *queue;

	TQueue::TQueueSendResult Command(TSharcCmd *cmd)
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
};

extern TSharcTask *SharcTask;


#endif /* SHARC_TASK_H_ */
