#ifndef SD_TEST_H_

#include "appdefs.h"

class TSD_TESTTask : public TTask
{
  public:
    TSD_TESTTask () ;

    inline void Give()
     {
   	 if ( cortex_isr_num())
   	 {
   		 BaseType_t HigherPriorityTaskWoken ;
   		 sem-> GiveFromISR ( &HigherPriorityTaskWoken) ;
            if ( HigherPriorityTaskWoken )
         	   TScheduler::Yeld();
   	 }
   	 else sem-> Give () ;
     }

  private:

     void Code() ;
     TSemaphore* sem ;
};

extern TSD_TESTTask* SD_TESTTask ;

#endif /* SD_TEST_H_ */
