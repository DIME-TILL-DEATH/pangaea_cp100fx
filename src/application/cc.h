#ifndef CC_H_

#include "appdefs.h"

extern uint32_t tap_temp;
extern uint32_t tap_temp1;
extern uint32_t tap_temp2;
extern uint32_t tap_global;

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

extern const uint8_t midi_dest_list[][14];
extern const uint8_t dest_tabl[];
extern const uint8_t dest_tabl_start[];
extern volatile uint8_t midi_b[3];
extern uint8_t contrs;
extern uint8_t mid_fl;
extern uint8_t ext_fl;
extern uint8_t ext_f_fl;
extern uint8_t ext_data;
extern uint8_t ext_sourc;
extern volatile uint8_t midi_in_buf[];
extern volatile uint8_t midi_in_po;
extern volatile uint16_t adc_bu;
extern volatile uint8_t tap_del_fl;
extern volatile uint8_t tap_trem_fl;
extern volatile uint8_t tap_moog_fl;
extern TCCTask *CCTask;

#endif /* CC_H_ */
