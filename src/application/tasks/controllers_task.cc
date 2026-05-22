#include "controllers_task.h"

#include "system.h"
#include "controller.h"

#include "ui_task.h"

TControllersTask *ControllersTask = nullptr;
//------------------------------------------------------------------------------
TControllersTask::TControllersTask() :
		TTask()
{
}

void TControllersTask::midiCommand(uint8_t source, uint8_t data)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_MIDI;
	cmd.midiCmd.source = source;
	cmd.midiCmd.data = data;
	Command(&cmd);
}

void TControllersTask::extCommand(uint8_t source, uint8_t data)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_EXTERNAL;
	cmd.extCmd.source = source;
	cmd.extCmd.data = data;
	Command(&cmd);
}

void TControllersTask::midiCalcTempo(uint16_t* data, uint8_t size)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_TEMPO;
	cmd.tempoCmd.data = data;
	cmd.tempoCmd.size = size;
	Command(&cmd);
}

void TControllersTask::Code()
{
	queue = new TQueue(64, sizeof(TControllerCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();

	TControllerCmd cmd;

	for(int i =0; i < maBufSize; i++) maMidiClockBuf[i] = 500;

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_CCTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.type)
		{
			case CONTROLLER_MIDI:
			{
				for(uint8_t i=0; i < Controller::controllersCount; i++)
				{
					if(currentPreset.controller[i].src == cmd.midiCmd.source)
						Controller::SetData(currentPreset.controller[i], cmd.midiCmd.data);
				}
				UITask->refreshMenu();
				break;
			}

			case CONTROLLER_EXTERNAL:
			{
				for(uint8_t i=0; i < Controller::controllersCount; i++)
				{
					if(currentPreset.controller[i].src == cmd.extCmd.source)
						Controller::SetData(currentPreset.controller[i], cmd.extCmd.data);
				}
				UITask->refreshMenu();
				break;
			}

			case CONTROLLER_TEMPO:
			{
				uint16_t buf[32];
				kgp_sdk_libc::memcpy(buf, cmd.tempoCmd.data, cmd.tempoCmd.size * sizeof(uint16_t));

				float roundedInterval = 0;
				for(int i=0; i<cmd.tempoCmd.size; i++)
				{
					roundedInterval += buf[i];
				}
				roundedInterval = roundedInterval/cmd.tempoCmd.size;

				if(maMidiClockBufPos == maBufSize) maMidiClockBufPos = 0;

				float result = 0;
				for(int i =0; i < maBufSize; i++) result += maMidiClockBuf[i];
				result = result / maBufSize;

				System::setMoogTime(result);
				System::setDelayTime(result);
				System::setTremoloTime(result);
				UITask->refreshMenu();
				break;
			}
		}


	}
}

