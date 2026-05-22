#include "sharc_task.h"

#include "preset.h"
#include "system.h"

TSharcTask *SharcTask = nullptr;

TSharcTask::TSharcTask() :
		TTask()
{
}

void TSharcTask::Code()
{
	queue = new TQueue(32, sizeof(TSharcCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();


//	extern EventGroupHandle_t startEventGroup;
//	xEventGroupSync(startEventGroup, EVENT_BIT_CCTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);
	System::setStartupValues();

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
			case SHARC_SEND_PRIMARY_DATA:
			{
				DSP_SendPrimaryData(cmd.dataCmd.cabMainData, cmd.dataCmd.cabAuxData,
										cmd.dataCmd.modulesData, cmd.dataCmd.presetNum);
				break;
			}
			case SHARC_SEND_CAB1_DATA:
			{
				DSP_SendPrimaryData(cmd.dataCmd.cabMainData, cmd.dataCmd.cabAuxData,
						&currentPreset.paramData, cmd.dataCmd.presetNum);
				break;
			}
			case SHARC_SEND_CAB2_DATA:
			{
				DSP_SendCab2Data(cmd.dataCmd.cabMainData, cmd.dataCmd.presetNum);
				break;
			}
			case SHARC_ERASE_CAB1:
			{
				DSP_SendPrimaryData(nullptr, nullptr, &currentPreset.paramData, cmd.dataCmd.presetNum);
				break;
			}
			case SHARC_ERASE_CAB2:
			{
				DSP_SendCab2Data(nullptr, cmd.dataCmd.presetNum);
				break;
			}
		}
	}
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

void TSharcTask::sendPrimaryData(uint8_t* cabMainData, uint8_t* cabAuxData,
			Preset::TModulesData* modulesData, uint8_t presetNum)
{
	TSharcCmd cmd;
	cmd.type = SHARC_SEND_PRIMARY_DATA;
	cmd.dataCmd.cabMainData = cabMainData;
	cmd.dataCmd.cabAuxData = cabAuxData;
	cmd.dataCmd.modulesData = modulesData;
	cmd.dataCmd.presetNum = presetNum;
	Command(&cmd);
}

void TSharcTask::sendCab1Data(uint8_t* cabMainData, uint8_t* cabAuxData, uint8_t presetNum)
{
	TSharcCmd cmd;
	cmd.type = SHARC_SEND_CAB1_DATA;
	cmd.dataCmd.cabMainData = cabMainData;
	cmd.dataCmd.cabAuxData = cabAuxData;
	cmd.dataCmd.modulesData = nullptr;
	cmd.dataCmd.presetNum = presetNum;
	Command(&cmd);
}

void TSharcTask::sendCab2Data(uint8_t* cabData, uint8_t presetNum) // 0 - currentPreset
{
	TSharcCmd cmd;
	cmd.type = SHARC_SEND_CAB2_DATA;
	cmd.dataCmd.cabMainData = cabData;
	cmd.dataCmd.cabAuxData = nullptr;
	cmd.dataCmd.modulesData = nullptr;
	cmd.dataCmd.presetNum = presetNum;
	Command(&cmd);
}

void TSharcTask::eraseCab1(uint8_t presetNum)
{
	TSharcCmd cmd;
	cmd.type = SHARC_ERASE_CAB1;
	cmd.dataCmd.cabMainData = nullptr;
	cmd.dataCmd.cabAuxData = nullptr;
	cmd.dataCmd.modulesData = nullptr;
	cmd.dataCmd.presetNum = presetNum;
	Command(&cmd);
}

void TSharcTask::eraseCab2(uint8_t presetNum)
{
	TSharcCmd cmd;
	cmd.type = SHARC_ERASE_CAB2;
	cmd.dataCmd.cabMainData = nullptr;
	cmd.dataCmd.cabAuxData = nullptr;
	cmd.dataCmd.modulesData = nullptr;
	cmd.dataCmd.presetNum = presetNum;
	Command(&cmd);
}

