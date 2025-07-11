#ifndef __CS_H__

#include "appdefs.h"
#include "gui/gui_task.h"

#include "mainmenu.h"
#include "usbmenu.h"

class TCSTask: public TTask
{
public:
	TCSTask();
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

	void SetKUp(uint8_t val)
	{
		k_up = val;
	}

	void SetKDown(uint8_t val)
	{
		k_down = val;
	}

	inline void DisplayAccess(bool val)
	{
		DispalyAccess = val;
		if(val)
			Give();
	}

	inline bool DisplayAccess()
	{
		return DispalyAccess;
	}

	void Restart(void);

	inline void CS_del(uint32_t del)
	{
		Delay(del);
	}

private:
	void Code();

	TSemaphore *sem;
	bool DispalyAccess;
};

float compr_out(float efxout);
void Gate_Change_Preset(void);
void Compressor_init(void);
void Compressor_Change_Preset(int dgui, int npreset);
float gate_out(float efxout);

extern float m_vol;
extern float p_vol;
extern uint8_t midi_clk_flag;
extern TCSTask *CSTask;

extern MainMenu* mainMenu;
extern UsbMenu* usbMenu;

#endif /*__CS_H__*/
