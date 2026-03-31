#ifndef SHARC_TASK_H_
#define SHARC_TASK_H_

#include "appdefs.h"
#include "sharc.h"

#include "preset.h"

class TSharcTask: public TTask
{
public:
	TSharcTask();

	void setParameter(dsp_module_address_t moduleAddress, uint8_t parameterAddress_data, uint8_t data = 0);
	void sendPrimaryData(uint8_t* cabMainData, uint8_t* cabAuxData,
			Preset::TModulesData* modulesData, uint8_t presetNum = 0);

	void sendCab1Data(uint8_t* cabMainData, uint8_t* cabAuxData, uint8_t presetNum = 0);
	void sendCab2Data(uint8_t* data, uint8_t presetNum = 0); // 0 - currentPreset

	void eraseCab1(uint8_t presetNum);
	void eraseCab2(uint8_t presetNum);
private:
	void Code() override;

	typedef enum{
		SHARC_SET_PARAMETER,
		SHARC_SEND_PRIMARY_DATA,
		SHARC_SEND_CAB1_DATA,
		SHARC_SEND_CAB2_DATA,
		SHARC_ERASE_CAB1,
		SHARC_ERASE_CAB2
	}TCmd;

	typedef struct{
		dsp_module_address_t moduleAddress;
		uint8_t parameterAddress_data;  // address OR data
		uint8_t data;
	}TParameterCmd;

	typedef struct{
		uint8_t* cabMainData;
		uint8_t* cabAuxData;
		Preset::TModulesData* modulesData;
		uint8_t presetNum;
	}TDataCmd;

	typedef struct{
		TCmd type;

		union
		{
			TParameterCmd parameterCmd;
			TDataCmd dataCmd;

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
