#include "fs.h"
#include "cs.h"
#include "format.h"
#include "gui/allFonts.h"
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

uint8_t __CCM_BSS__ cab1_data[12288];
uint8_t __CCM_BSS__ cab2_data[12288];
uint8_t sd_buf[512];
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
		extern volatile uint8_t file_fl;
		if(browse_command==TFsBrowser::bcFsMount||browse_command==TFsBrowser::bcFsUmount||file_fl)
			continue;
		if(browse_command!=TFsBrowser::bcStartup)
		{
			Delay(10);
			DisplayTask->Clear();
			DisplayTask->StringOut(4, 0, TDisplayTask::fntSystem, TDisplayTask::fnsBlack,
					(uint8_t*)FSTask->Object().dir.c_str());
			DisplayTask->StringOut(4, 1, TDisplayTask::fntSystem, TDisplayTask::fnsBlack,
					(uint8_t*)FSTask->Object().name.c_str());
		}
	}
}
//------------------------------------------------------------------------------

