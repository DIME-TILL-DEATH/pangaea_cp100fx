#ifndef SD_TEST_H_

#include "appdefs.h"

class TSDTestTask: public TTask
{
public:
	TSDTestTask();

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

	static uint8_t sdInitState;

private:

	void Code();
	TSemaphore *sem;
};

extern TSDTestTask *SDTestTask;

#endif /* SD_TEST_H_ */
