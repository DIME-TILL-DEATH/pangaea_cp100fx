#include "fs.h"
#include "cs.h"
#include "format.h"
#include "allFonts.h"
#include "display.h"
uint8_t print_flag;

TFSTask *FSTask;

//------------------------------------------------------------------------------
TFSTask::TFSTask() :
		TTask()
{
	queue = new TQueue(4, sizeof(TFsBrowser::browse_command_t));
}

TFSTask::~TFSTask()
{
	delete queue;
}

bool cab_data_ready;
//-----------------------------------------------------------------------------------------------------
void TFSTask::Code()
{
	TFsBrowser::browse_command_t browse_command;
	TFsBrowser *fb = new TFsBrowser();

	while(1)
	{
		queue->Receive((void*)&browse_command, portMAX_DELAY);

		fb->Browse(browse_command, object, object_list);
	}
}
//------------------------------------------------------------------------------

