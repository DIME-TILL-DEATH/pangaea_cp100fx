#include "filesystem_task.h"

#include "format.h"
#include "allFonts.h"
#include "display_task.h"
#include "ui_task.h"


TFileSystemTask *FileSystemTask;
TFsBrowser *fileBrowser;
//------------------------------------------------------------------------------
TFileSystemTask::TFileSystemTask() :
		TTask()
{
	queue = new TQueue(4, sizeof(TFsBrowser::browse_command_t));
}

TFileSystemTask::~TFileSystemTask()
{
	delete queue;
}

//-----------------------------------------------------------------------------------------------------
void TFileSystemTask::Code()
{
	TFsBrowser::browse_command_t browse_command;
	fileBrowser = new TFsBrowser();

	while(1)
	{
		queue->Receive((void*)&browse_command, portMAX_DELAY);

		fileBrowser->Browse(browse_command, object);
	}
}
//------------------------------------------------------------------------------
