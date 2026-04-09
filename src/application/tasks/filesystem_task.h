#ifndef __FILESYSTEM_TASK_H__
#define __FILESYSTEM_TASK_H__

#include "appdefs.h"
#include "fs_browser.h"
#include "preset.h"

class TFileSystemTask: public TTask
{
public:
	TFileSystemTask();
	virtual ~TFileSystemTask();

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

	void SendCommand(TFsBrowser::browse_command_t browse_command);
	void SendCommandFromISR(TFsBrowser::browse_command_t browse_command, BaseType_t *HigherPriorityTaskWoken);
	void SendResponse(const TResponse &response);
	TFileSystemTask::TResponse GetResponseBlocking();

	fs_object_t& Object() { return object; }

private:
	void Code() override;
	TQueue *queue;
	TQueue *responseQueue;
	fs_object_t object;

	bool impulseDirectoryExist;
};

extern TFileSystemTask *FileSystemTask;
extern TFsBrowser *fileBrowser;

#endif /*__FILESYSTEM_TASK_H__*/
