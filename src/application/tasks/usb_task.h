#ifndef __USB_TASK_H__
#define __USB_TASK_H__

#include "appdefs.h"
#include "console.h"

class TUsbTask: public TTask
{
public:

	enum TMode
	{
		mCDC,
		mMSC
	};

	TUsbTask();
	~TUsbTask();

	TUsbTask(TMode val);

	void SendDataCDC(uint8_t *data, size_t len);

	size_t RxQueueWaiting();
	char RxQueueRecv();
	TQueue::TQueueSendResult RxSend(uint8_t *buf, uint32_t len);
	void* UsbOtgDevHandle();
private:
	void Code() override;
	void *USB_OTG_dev;

	TQueue *rx_queue{nullptr};
	TConsoleTask::readline_io_t* cdc_io{nullptr};
};

extern TUsbTask *UsbTask;

#endif /*__USB_TASK_H__*/
