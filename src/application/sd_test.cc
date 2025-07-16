#include "appdefs.h"
#include "sd_test.h"
#include "sdio_sd.h"
#include "fs.h"

//void SD_DeInit(void);
//SD_Error SD_Init(void);


uint8_t TSD_TESTTask::sdInitState = 0;

TSD_TESTTask *SD_TESTTask;

//------------------------------------------------------------------------------
TSD_TESTTask::TSD_TESTTask() :
		TTask()
{
}
void TSD_TESTTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 4, 0);
	Delay(10);
	SD_TESTTask->Give();

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
					SD_TESTTask->Give();
				}
				else
				{
					sdInitState = 1;
					FSTask->SendCommand(TFsBrowser::bcFsMount);
				}
			}
		}
		else
		{
			if(sdInitState)
			{
				FSTask->SendCommand(TFsBrowser::bcFsUmount);
				SD_DeInit();
				sdInitState = 0;
			}
		}
	}
}
