#include "appdefs.h"
#include "sd_test.h"
#include "sdio_sd.h"
#include "fs.h"

void SD_DeInit(void);
SD_Error SD_Init(void);
uint8_t sd_init_fl = 0;

TSD_TESTTask* SD_TESTTask ;

//------------------------------------------------------------------------------
TSD_TESTTask::TSD_TESTTask () : TTask()
{
}
void TSD_TESTTask::Code()
{
  sem = new TSemaphore (TSemaphore::fstCounting, 4 , 0 ) ;
  Delay(10);
  SD_TESTTask->Give();

  while(1)
    {
	     sem->Take(portMAX_DELAY);
	  	 if(!(GPIOA->IDR & GPIO_Pin_8))
	  		 {
	  		 	 if(sd_init_fl != 1)
	  		 	 {
	  		 		 sd_init_fl = 2;
	  		 		 if(SD_Init() != SD_OK)
	  		 		 {
	  		 			SD_DeInit();
	  		 			sd_init_fl = 0;
	  		 			SD_TESTTask->Give();
	  		 		 }
	  		 		 else
	  		 			 {
	  		 			 	 sd_init_fl = 1;
	  		 			 	 FSTask->SendCommand( TFsBrowser::bcFsMount );
	  		 			 }
	  		 	 }
	  		 }
	  	 else {
	  		 if(sd_init_fl)
	  			 {
	  			 	FSTask->SendCommand( TFsBrowser::bcFsUmount );
	  			 	SD_DeInit();
	  			 	sd_init_fl = 0;
	  			 }
	  	 }
    }
}
