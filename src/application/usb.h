#ifndef __USB_H__
#define __USB_H__

#include "appdefs.h"
#include "console.h"

extern TConsoleTask::readline_io_t cdc_io ;

class TUsbTask : public TTask
{
  public:

	 typedef enum { mCDC, mMSC } mode_t;

     TUsbTask () ;
     virtual ~TUsbTask() {} ;
     TUsbTask (mode_t val);

     void SendDataCDC(uint8_t* data , size_t len);

     inline size_t RxQueueWaiting () { return rx_queue->MessagesWaiting(); }
     inline char RxQueueRecv() { char c ; rx_queue->Receive( &c , portMAX_DELAY ); return c;  }
     TQueue::TQueueSendResult RxSend(uint8_t* buf, uint32_t len ) ;

     inline void* UsbOtgDevHandle() { return USB_OTG_dev ; }
  private:
     void Code() ;
     void*  USB_OTG_dev ;

     TQueue* rx_queue ;

};

extern TUsbTask* UsbTask ;

#endif /*__USB_H__*/
