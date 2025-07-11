#include "usbmenu.h"

#include "display.h"
#include "usb.h"
#include "init.h"

#include "cs.h"

#include "BF706_send.h"

const uint8_t UsbMenu::strUsbMenu[][12];
const uint8_t UsbMenu::strPositions[2];

UsbMenu::UsbMenu()
{
	m_menuType = MENU_USB_SELECT;
}

void UsbMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	DisplayTask->StringOut(strPositions[0], 1, TDisplayTask::fntSystem, 0, &strUsbMenu[0][0]);
	DisplayTask->StringOut(strPositions[1], 2, TDisplayTask::fntSystem, 0, &strUsbMenu[1][0]);
}

void UsbMenu::task()
{
	if(usbConnected) return;

	DisplayTask->StringOut(strPositions[m_parNum], m_parNum + 1, TDisplayTask::fntSystem, 2 * blinkFlag_fl, &strUsbMenu[m_parNum][0]);
}

void UsbMenu::encoderPressed()
{
	usb_connect_type = static_cast<TUsbTask::TMode>(m_parNum);
	usbConnected = true;

	switch(usb_connect_type)
	{
		case TUsbTask::mMSC:
		{
			send_codec(0xa102);
			DSP_gui_set_parameter(DSP_ADDRESS_TUN_PROC, 0, 0);

			DisplayTask->Clear();
			DisplayTask->StringOut(6, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"cp100fx connected as");
			DisplayTask->StringOut(30, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"mass storage");
			break;
		}

		case TUsbTask::mCDC:
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(6, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"cp100fx connected as");
			DisplayTask->StringOut(34, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"serial port");

//			currentMenu = mainMenu;
//			mainMenu->show();
			break;
		}
	}

	start_usb();
}

void UsbMenu::encoderClockwise()
{
	if(usbConnected) return;

	if(m_parNum < 1) m_parNum++;

	DisplayTask->StringOut(strPositions[0], 1, TDisplayTask::fntSystem, 0, &strUsbMenu[0][0]);
	tim5_start(0);
}

void UsbMenu::encoderCounterClockwise()
{
	if(usbConnected) return;

	if(m_parNum > 0) m_parNum--;

	DisplayTask->StringOut(strPositions[1], 2, TDisplayTask::fntSystem, 0, &strUsbMenu[1][0]);
	tim5_start(0);
}

void UsbMenu::start_usb()
{
	if(usb_connect_type == TUsbTask::mCDC)
	{
		UsbTask = new TUsbTask(TUsbTask::mCDC);

		ConsoleTask = new TConsoleTask(256);
		ConsoleTask->Echo(false);
		ConsoleTask->SetIo(&cdc_io);
		ConsoleTask->Create("CONS", 20*configMINIMAL_STACK_SIZE, 0);

		//ConsoleTask->Clear();
	}
	else
	{
		UsbTask = new TUsbTask(TUsbTask::mMSC);
	}

	UsbTask->Create("USB", 10*configMINIMAL_STACK_SIZE, 0);
}
