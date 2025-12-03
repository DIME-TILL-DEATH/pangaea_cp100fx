#ifndef CC_H_

#include "appdefs.h"

class TCCTask: public TTask
{
public:
	TCCTask();

	inline void Give()
	{
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			sem->GiveFromISR(&HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
		}
		else
			sem->Give();
	}
private:
	void Code();
	TSemaphore *sem;
};

void controllerSetData(uint8_t adr, uint8_t data);

extern volatile uint8_t midi_b[3];
extern uint8_t mid_fl;
extern uint8_t ext_fl;
extern uint8_t ext_f_fl;
extern uint8_t ext_data;
extern uint8_t ext_sourc;
extern volatile uint8_t midi_in_buf[];
extern volatile uint8_t midi_buf_wr_pos;
extern volatile uint16_t adc_bu;
extern TCCTask *CCTask;

#endif /* CC_H_ */
