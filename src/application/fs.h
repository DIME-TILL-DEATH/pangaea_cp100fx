#ifndef __FS_H__
#define __FS_H__

#include "appdefs.h"
#include "fs_browser.h"

class TFSTask: public TTask
{
public:
	TFSTask();
	virtual ~TFSTask();
	inline void SendCommand(TFsBrowser::browse_command_t browse_command)
	{
		TFsBrowser::browse_command_t tmp = browse_command;
		queue->SendToBack((void*)&tmp, portMAX_DELAY);
	}

	inline void SendCommandFromISR(TFsBrowser::browse_command_t browse_command, BaseType_t *HigherPriorityTaskWoken)
	{
		TFsBrowser::browse_command_t tmp = browse_command;
		queue->SendToBackFromISR((void*)&tmp, HigherPriorityTaskWoken);
	}

	fs_object_t& Object()
	{
		return object;
	}
	;

private:
	void Code();
	TQueue *queue;
	fs_object_t object;
	fs_object_list_t object_list;
};

extern uint8_t cab1_data[];
extern uint8_t cab2_data[];
extern uint8_t print_flag;
extern TFSTask *FSTask;

#endif /*__FS_H__*/
