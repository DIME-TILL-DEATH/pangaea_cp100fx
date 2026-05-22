#include "filesystem_task.h"

#include "format.h"

#include "display_task.h"
#include "ui_task.h"


TFileSystemTask *FileSystemTask = nullptr;
TFsBrowser *fileBrowser = nullptr;
//------------------------------------------------------------------------------
TFileSystemTask::TFileSystemTask() :
		TTask()
{
	queue = new TQueue(4, sizeof(TFsBrowser::browse_command_t));
	responseQueue = new TQueue(4, sizeof(TResponse));
}

TFileSystemTask::~TFileSystemTask()
{
	delete queue;
	delete responseQueue;
}

//-----------------------------------------------------------------------------------------------------
void TFileSystemTask::Code()
{
	TFsBrowser::browse_command_t browse_command;
	fileBrowser = new TFsBrowser();

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_FSTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	while(1)
	{
		queue->Receive((void*)&browse_command, portMAX_DELAY);

		fileBrowser->Browse(browse_command, object);
	}
}
//------------------------------------------------------------------------------
void TFileSystemTask::SendCommand(TFsBrowser::browse_command_t browse_command)
{
	TFsBrowser::browse_command_t tmp = browse_command;
	queue->SendToBack((void*)&tmp, portMAX_DELAY);
}

void TFileSystemTask::SendCommandFromISR(TFsBrowser::browse_command_t browse_command, BaseType_t *HigherPriorityTaskWoken)
{
	TFsBrowser::browse_command_t tmp = browse_command;
	queue->SendToBackFromISR((void*)&tmp, HigherPriorityTaskWoken);
}

void TFileSystemTask::SendResponse(const TResponse &response)
{
	responseQueue->SendToBack((void*)&response, portMAX_DELAY);
}

TFileSystemTask::TResponse TFileSystemTask::GetResponseBlocking()
{
	TResponse response;
	responseQueue->Receive((void*)&response, portMAX_DELAY);
	return response;
}
