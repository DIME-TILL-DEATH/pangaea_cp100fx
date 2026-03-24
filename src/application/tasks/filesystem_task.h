#ifndef __FILESYSTEM_TASK_H__
#define __FILESYSTEM_TASK_H__

#include "appdefs.h"
#include "fs_browser.h"

class TFileSystemTask: public TTask
{
public:
	TFileSystemTask();
	virtual ~TFileSystemTask();

	void SendCommand(TFsBrowser::browse_command_t browse_command)
	{
		TFsBrowser::browse_command_t tmp = browse_command;
		queue->SendToBack((void*)&tmp, portMAX_DELAY);
	}

	void SendCommandFromISR(TFsBrowser::browse_command_t browse_command, BaseType_t *HigherPriorityTaskWoken)
	{
		TFsBrowser::browse_command_t tmp = browse_command;
		queue->SendToBackFromISR((void*)&tmp, HigherPriorityTaskWoken);
	}

	fs_object_t& Object() { return object; }

private:
	void Code() override;
	TQueue *queue;
	fs_object_t object;

	bool impulseDirectoryExist;
};

extern TFileSystemTask *FileSystemTask;
extern TFsBrowser *fileBrowser;

#endif /*__FILESYSTEM_TASK_H__*/
