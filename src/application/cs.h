#ifndef __CS_H__

#include "appdefs.h"
#include "gui_task.h"

#include "fs_browser.h"

#include "mainmenu.h"
#include "usbmenu.h"

class TCSTask: public TTask
{
public:
	TCSTask();
	~TCSTask();

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

	//-----------------Responses----------------------
	typedef enum
	{
		rpFileLoaded,
		rpFileInvalid,
		rpFileSelected,
		rpDirSelected
	} TResponseType;

	typedef struct
	{
		fs_object_type_t type;
		char name[64];
		uint8_t *buffer;
	} TResponseFile;

	typedef struct
	{
		TResponseType responseType;
		union
		{
			TResponseFile file;
		};
	} TResponse;

	void SendResponse(const TResponse &response)
	{
		queue->SendToBack((void*)&response, portMAX_DELAY);
	}

	TResponse GetResponseBlocking()
	{
		TResponse response;
		queue->Receive((void*)&response, portMAX_DELAY);
		return response;
	}

private:
	void Code();

	TQueue *queue;

	TSemaphore *sem;
	bool DispalyAccess;
};

extern TCSTask *CSTask;

extern MainMenu *mainMenu;
extern UsbMenu *usbMenu;

#endif /*__CS_H__*/
