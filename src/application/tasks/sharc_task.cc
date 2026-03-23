#include "sharc_task.h"

TSharcTask *SharcTask = nullptr;

TSharcTask::TSharcTask() :
		TTask()
{
}

void TSharcTask::setParameter(dsp_module_address_t moduleAddress, uint8_t parameterAddress_data, uint8_t data)
{
	TSharcCmd cmd;
	cmd.type = SHARC_SET_PARAMETER;
	cmd.parameterCmd.moduleAddress = moduleAddress;
	cmd.parameterCmd.parameterAddress_data = parameterAddress_data;  // address OR data
	cmd.parameterCmd.data = data;
	Command(&cmd);
}


void TSharcTask::Code()
{
	queue = new TQueue(32, sizeof(TSharcCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();


//	extern EventGroupHandle_t startEventGroup;
//	xEventGroupSync(startEventGroup, EVENT_BIT_CCTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);
	TSharcCmd cmd;
	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.type)
		{
			case SHARC_SET_PARAMETER:
			{
				DSP_SendParameter(cmd.parameterCmd.moduleAddress,
						cmd.parameterCmd.parameterAddress_data, cmd.parameterCmd.data);
				break;
			}
		}
	}
}

