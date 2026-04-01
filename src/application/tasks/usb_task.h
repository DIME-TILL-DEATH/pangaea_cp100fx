#ifndef __USB_TASK_H__
#define __USB_TASK_H__

#include "appdefs.h"
#include "console_task.h"

#include "class/msc/usbd_msc_core.h"
#include "class/cdc/usbd_cdc_core.h"
#include "class/cdc/usbd_cdc_if.h"

#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_dcd_int.h"

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

//	size_t RxQueueWaiting();
//	char RxQueueRecv();
//	TQueue::TQueueSendResult RxSend(uint8_t *buf, uint32_t len);
	void* UsbOtgDevHandle();
private:
	void Code() override;
	USB_OTG_CORE_HANDLE* USB_OTG_dev;

	TMode m_mode;
};

extern TUsbTask *UsbTask;

#endif /*__USB_TASK_H__*/
