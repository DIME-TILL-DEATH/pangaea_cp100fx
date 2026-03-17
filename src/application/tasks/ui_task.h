#ifndef __CS_H__
#define __CS_H__

#include "appdefs.h"

#include "fs_browser.h"

#include "mainmenu.h"
#include "usbmenu.h"

class TUITask: public TTask
{
public:
	TUITask();
	~TUITask();

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
		UI_REFRESH_MENU,
		UI_RETURN_FROM_MENU,
		UI_RUNNING_STRING,
		UI_TASK,
		UI_KEYS_EVENTS,
		UI_ENCODER_EVENTS
	}TCSCmdType;

	typedef struct{
		TCSCmdType type;

		union{
			TKeysEvents keysEvents;
			TEncoderEvents encoderEvents;
		};
	}TUICmd;

	void refreshMenu(){
		TUICmd cmd;
		cmd.type = UI_REFRESH_MENU;
		Command(&cmd);
	}

	void returnFromMenu(){
		TUICmd cmd;
		cmd.type = UI_RETURN_FROM_MENU;
		Command(&cmd);
	}

	void runningString(){
		TUICmd cmd;
		cmd.type = UI_RUNNING_STRING;
		Command(&cmd);
	}

	void task(){
		TUICmd cmd;
		cmd.type = UI_TASK;
		Command(&cmd);
	}

	void keysEvents(const TKeysEvents& events){
		TUICmd cmd;
		cmd.type = UI_KEYS_EVENTS;
		cmd.keysEvents = events;
		Command(&cmd);
	}

	void encoderEvents(const TEncoderEvents& events){
		TUICmd cmd;
		cmd.type = UI_ENCODER_EVENTS;
		cmd.encoderEvents = events;
		Command(&cmd);
	}

private:
	void Code();

	TQueue::TQueueSendResult Command(TUICmd *cmd)
	{
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			TQueue::TQueueSendResult result;
			result = cmdQueue->SendToBackFromISR(cmd, &HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
			return result;
		}
		else
		{
			return cmdQueue->SendToBack(cmd, 0);
		}
	}

	TQueue *responseQueue;
	TQueue *cmdQueue;
};

extern TUITask *UITask;

extern AbstractMenu *currentMenu;
extern MainMenu *mainMenu;
extern UsbMenu *usbMenu;

#endif /*__CS_H__*/
