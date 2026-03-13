#include "sdtest_task.h"

#include "appdefs.h"
#include "filesystem_task.h"
#include "sdio_sd.h"


uint8_t TSDTestTask::sdInitState = 0;

TSDTestTask *SDTestTask;

//------------------------------------------------------------------------------
TSDTestTask::TSDTestTask() :
		TTask()
{
}

void TSDTestTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 4, 0);
	Delay(10);
	SDTestTask->Give();

	while(1)
	{
		sem->Take(portMAX_DELAY);
		if(!(GPIOA->IDR&GPIO_Pin_8))
		{
			if(sdInitState!=1)
			{
				sdInitState = 2;
				if(SD_Init()!=SD_OK)
				{
					SD_DeInit();
					sdInitState = 0;
					SDTestTask->Give();
				}
				else
				{
					sdInitState = 1;
					FileSystemTask->SendCommand(TFsBrowser::bcFsMount);
				}
			}
		}
		else
		{
			if(sdInitState)
			{
				FileSystemTask->SendCommand(TFsBrowser::bcFsUmount);
				SD_DeInit();
				sdInitState = 0;
			}
		}
	}
}
