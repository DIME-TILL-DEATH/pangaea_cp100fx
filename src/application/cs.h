#ifndef __CS_H__

#include "appdefs.h"
#include "gui/gui_task.h"

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
	void Setcurrent_menu(gui_menu_type val)
	{
		current_menu_type = val;
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

	inline void Restart(void)
	{
		current_menu_type = MENU_MAIN;
		clean_flag();
		//DisplayTask->Menu_init(1,0,prog_data,prog1);
	}

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

#endif /*__CS_H__*/
