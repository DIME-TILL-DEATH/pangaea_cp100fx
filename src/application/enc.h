#ifndef __ENC_H__

#include "appdefs.h"

enum
{
	ENC_COUNTERCLOCKWISE_STEP = 1,
	ENC_CLOCKWISE_STEP
};

volatile extern uint8_t enc_run;
class TENCTask : public TTask
{
  public:
    TENCTask () ;
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
    inline void SetEnc ( uint8_t val )
    {
      enc_run = val ;
    }
  private:
     void Code() ;
     TSemaphore* sem ;


};

extern volatile uint8_t encoder_state;
extern volatile uint8_t encoder_state_updated;
extern volatile uint8_t encoder_knob_pressed;
extern volatile uint8_t encoder_flag;
extern volatile uint8_t k_tuner;
extern volatile uint8_t contr_kn[];
extern volatile uint8_t contr_kn1[];

extern TENCTask* ENCTask ;

#endif /*__ENC_H__*/
