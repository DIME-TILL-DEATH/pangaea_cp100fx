#ifndef __UI_TASK_H__
#define __UI_TASK_H__

#include "appdefs.h"

#include "fs_browser.h"
#include "module_parameter.h"

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
		char name[CAB_NAME_STRING_SIZE];
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
		UI_SHOW_MENU,
		UI_RETURN_FROM_MENU,
		UI_TASK,
		UI_RUNNING_STRING,
		UI_KEYS_EVENTS,
		UI_ENCODER_EVENTS,
		UI_FSW_SINGLE_MODE_PRESS,
		UI_FSW_DUAL_MODE_PRESS,
		UI_FSW_DUAL_MODE_HOLD,
		UI_SET_PARAMETER
	}TCSCmdType;

	typedef struct{
		AbstractMenu* menu;
	}TShowMenuParams;

	typedef struct{
		setter_handler_t funcHandler;
		uint32_t value;
	}TSetParamParams;

	typedef struct{
		TCSCmdType type;

		union{
			TKeysEvents keysEvents;
			TEncoderEvents encoderEvents;
			TFswEvents fswEvents;
			TShowMenuParams showMenuParams;
			TSetParamParams setParamParams;
		};
	}TUICmd;

	void refreshMenu(){
		TUICmd cmd;
		cmd.type = UI_REFRESH_MENU;
		Command(&cmd);
	}

	void showMenu(AbstractMenu* menu){
		TUICmd cmd;
		cmd.type = UI_SHOW_MENU;
		cmd.showMenuParams.menu = menu;
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

	void fswSinglePressed(const TFswEvents& fswEvents){
		TUICmd cmd;
		cmd.type = UI_FSW_SINGLE_MODE_PRESS;
		cmd.fswEvents = fswEvents;
		Command(&cmd);
	};

	void fswDualPressed(const TFswEvents& fswEvents){
		TUICmd cmd;
		cmd.type = UI_FSW_DUAL_MODE_PRESS;
		cmd.fswEvents = fswEvents;
		Command(&cmd);
	};

	void fswDualHolded(const TFswEvents& fswEvents){
		TUICmd cmd;
		cmd.type = UI_FSW_DUAL_MODE_HOLD;
		cmd.fswEvents = fswEvents;
		Command(&cmd);
	};

	void setParam(setter_handler_t funcHandler, uint32_t value){
		TUICmd cmd;
		cmd.type = UI_SET_PARAMETER;
		cmd.setParamParams.funcHandler = funcHandler;
		cmd.setParamParams.value = value;
		Command(&cmd);
	}

private:
	void Code() override;

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

#endif /*__UI_TASK_H__*/
