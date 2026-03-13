#ifndef __CS_H__
#define __CS_H__

#include "appdefs.h"

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
		responseQueue->SendToBack((void*)&response, portMAX_DELAY);
	}

	TResponse GetResponseBlocking()
	{
		TResponse response;
		responseQueue->Receive((void*)&response, portMAX_DELAY);
		return response;
	}

	typedef enum{
		CS_REFRESH_MENU,
		CS_RUNNING_STRING,
		CS_TASK,
		CS_KEYS_EVENTS,
		CS_ENCODER_EVENTS
	}TCSCmdType;

	typedef struct{
		TCSCmdType type;

		union{
			TKeysEvents keysEvents;
			TEncoderEvents encoderEvents;
		};
	}TCSCmd;

	void refreshMenu(){
		TCSCmd cmd;
		cmd.type = CS_REFRESH_MENU;
		Command(&cmd);
	}

	void runningString(){
		TCSCmd cmd;
		cmd.type = CS_RUNNING_STRING;
		Command(&cmd);
	}

	void task(){
		TCSCmd cmd;
		cmd.type = CS_TASK;
		Command(&cmd);
	}

	void keysEvents(const TKeysEvents& events){
		TCSCmd cmd;
		cmd.type = CS_KEYS_EVENTS;
		cmd.keysEvents = events;
		Command(&cmd);
	}

	void encoderEvents(const TEncoderEvents& events){
		TCSCmd cmd;
		cmd.type = CS_ENCODER_EVENTS;
		cmd.encoderEvents = events;
		Command(&cmd);
	}

private:
	void Code();

	TQueue::TQueueSendResult Command(TCSCmd *cmd)
		{
			if(cortex_isr_num())
			{
				// send comand from ISR
				BaseType_t HigherPriorityTaskWoken;
				TQueue::TQueueSendResult result;
				result = cmdQueue->SendToBackFromISR(cmd, &HigherPriorityTaskWoken);
				if(HigherPriorityTaskWoken)
					TScheduler::Yeld();
				return result;
			}
			else
			{
				// thread mode
				return cmdQueue->SendToBack(cmd, 0);
			}
		}

	TQueue *responseQueue;
	TQueue *cmdQueue;

	TSemaphore *sem;
	bool DispalyAccess;


};
extern uint8_t tun_del_val;

extern TCSTask *CSTask;

extern MainMenu *mainMenu;
extern UsbMenu *usbMenu;

#endif /*__CS_H__*/
