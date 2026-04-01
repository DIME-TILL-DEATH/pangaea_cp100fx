#include "usb_task.h"

#include "display_task.h"
#include "ui_task.h"
#include "console_task.h"

TUsbTask *UsbTask = nullptr;

extern "C" uint16_t CDC_Recv(uint8_t *buf, uint32_t len)
{
	if(!UsbTask) return USBD_FAIL;

	UsbTask->RxSend(buf, len);
	return USBD_OK;
}

int cdc_send_char(const int c)
{
	char tmp = c;
	VCP_fops.pIf_DataTx((uint8_t*)&tmp, 1);	// CDC_Send
	return c;
}

char* cdc_send_string(const char *s)
{
	VCP_fops.pIf_DataTx((uint8_t*)s, kgp_sdk_libc::strlen(s));
	return (char*)s;
}

size_t cdc_send_buf(const char *buf, size_t size)
{
	VCP_fops.pIf_DataTx((uint8_t*)buf, size);
	return size;
}

//---------------------------------------------------------------------------
int cdc_recv_char(int &c)
{
	if(!UsbTask) return 0;

	c = UsbTask->RxQueueRecv();
	return c;
}

size_t cdc_recv_string(emb_string &dest)
{
	if(!UsbTask) return 0;

	dest.clear();
	int c;
	do
	{
		c = UsbTask->RxQueueRecv();
		if(!c)
			return dest.length();
		else
			dest.push_back(c);
	} while(1);
}

size_t cdc_recv_line(emb_string &dest)
{
	if(!UsbTask) return 0;

	dest.clear();
	int c;
	do
	{
		c = UsbTask->RxQueueRecv();
		if(c=='\n')
			return dest.length();
		else
			dest.push_back(c);
	} while(1);
}

size_t cdc_recv_buf(char *buf, size_t size)
{
	if(!UsbTask) return 0;

	size_t c = 0;
	while(c<size)
	{
		*(buf++) = UsbTask->RxQueueRecv();
		c++;
	}
	return c;
}

void console_task_hw_init()
{

}

void cdc_enable(int &ret)
{

}

void cdc_disable(int &ret)
{

}
//------------------------------------------------------------------------------
TUsbTask::TUsbTask(TMode val) :
		TTask()
{
	m_mode = val;
	USBD_Class_cb_TypeDef* usbClassCb;

	switch(m_mode)
	{
		case mCDC:
		{
			usbClassCb = &USBD_CDC_cb;

			rx_queue = new TQueue(1024, 1);
			vQueueAddToRegistry(rx_queue, "USB tx queue");


			USBD_DeviceDesc[4] = 2;	/*bDeviceClass*/
			USBD_DeviceDesc[5] = 2; /*bDeviceSubClass*/
			USBD_DeviceDesc[8] = 0x83; /*idVendor*/
			USBD_DeviceDesc[9] = 0x04; /*idVendor*/
			USBD_DeviceDesc[10] = 0x40; /*idVendor*/
			USBD_DeviceDesc[11] = 0x57; /*idVendor*/

			TConsoleTask::readline_io_t cdc_io = {
				.console_task_hw_init = console_task_hw_init,

				.enable = cdc_enable,
				.disable = cdc_disable,

				.send_char = cdc_send_char,
				.recv_char = cdc_recv_char,

				.send_string = cdc_send_string,
				.recv_string = cdc_recv_string,

				.send_buf = cdc_send_buf,
				.recv_buf = cdc_recv_buf,
			};

			ConsoleTask->SetIo(&cdc_io);
			break;
		}

		case mMSC:
		{
			usbClassCb = &USBD_MSC_cb;
			break;
		}
	}

//	kgp_sdk_libc::memset(USB_OTG_dev, 0, sizeof(USB_OTG_CORE_HANDLE));
	USB_OTG_dev = new USB_OTG_CORE_HANDLE;
	USBD_Init(USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, usbClassCb, &USR_cb, false);
}

TUsbTask::~TUsbTask()
{
	if(rx_queue) delete rx_queue;
	if(USB_OTG_dev) delete USB_OTG_dev;

	TConsoleTask::readline_io_t cdc_io = {
		.console_task_hw_init = nullptr,

		.enable = nullptr,
		.disable = nullptr,

		.send_char = nullptr,
		.recv_char = nullptr,

		.send_string = nullptr,
		.recv_string = nullptr,

		.send_buf = nullptr,
		.recv_buf = nullptr,
	};

	ConsoleTask->SetIo(&cdc_io);

//	ConsoleTask->Give();

	vTaskDelete(0);
}


void TUsbTask::Code()
{
	bool configured = false;
//	ConsoleTask->Resume();
	while(1)
	{

		if(rx_queue) USBD_OTG_ISR_Handler((USB_OTG_CORE_HANDLE*)USB_OTG_dev);

		if(((USB_OTG_CORE_HANDLE*)USB_OTG_dev)->dev.device_status==USB_OTG_CONFIGURED && configured==false)
		{
			configured = true;
		}

		if(((USB_OTG_CORE_HANDLE*)USB_OTG_dev)->dev.device_status!=USB_OTG_CONFIGURED && configured==true)
		{
			if(m_mode == mMSC)
			{
			   NVIC_SystemReset();
			   configured = false;
			}
			else
			{
				usbMenu->stopUsb();
			}
		}

	}
}
//------------------------------------------------------------------------------
TQueue::TQueueSendResult TUsbTask::RxSend(uint8_t *buf, uint32_t len)
{
	for(uint32_t i = 0; i<len; i++)
	{
		if(rx_queue->SendToBack(buf+i, 0)==TQueue::qsrQueueFull)
			return TQueue::qsrQueueFull;
	}
	return TQueue::qsrPass;
}


size_t TUsbTask::RxQueueWaiting()
{
	return rx_queue->MessagesWaiting();
}

char TUsbTask::RxQueueRecv()
{
	char c;
	rx_queue->Receive(&c, portMAX_DELAY);
	return c;
}

void* TUsbTask::UsbOtgDevHandle()
{
	return USB_OTG_dev;
}
