#include "usbmenu.h"

#include "display.h"
#include "usb.h"
#include "init.h"

#include "cs.h"

#include "BF706_send.h"

const uint8_t UsbMenu::strUsbMenu[][19];
const uint8_t UsbMenu::strPositions[3];

UsbMenu::UsbMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_USB_SELECT;
}

void UsbMenu::show(TShowMode showMode)
{
	currentMenu = this;

	m_countOff = 20;
	autoconnectOn = true;

	DisplayTask->Clear();
	DisplayTask->StringOut(strPositions[0], 0, Font::fntSystem, 0, &strUsbMenu[0][0]);
	DisplayTask->StringOut(strPositions[1], 1, Font::fntSystem, 0, &strUsbMenu[1][0]);
	DisplayTask->StringOut(strPositions[2], 3, Font::fntSystem, 0, &strUsbMenu[2][0]);
	DisplayTask->ParamIndicNum(strPositions[2] + 18*6, 3, m_countOff);
}

void UsbMenu::task()
{
	if(!(GPIOA->IDR & GPIO_Pin_9))
	{
		switch(usb_connect_type)
		{
			case TUsbTask::mMSC:
			{
				NVIC_SystemReset();
				break;
			}

			case TUsbTask::mCDC:
			{
				usbMenu->stopUsb();
				return;
			}
		}
	}

	if(usbConnected)
	{
		return;
	}

	DisplayTask->StringOut(strPositions[m_parNum], m_parNum, Font::fntSystem, 2 * blinkFlag_fl, &strUsbMenu[m_parNum][0]);

	if(autoconnectOn)
	{
		if(blinkFlag_fl)
		{
			if(m_countOff > 0)
			{
				m_countOff--;
				DisplayTask->ParamIndicNum(strPositions[2] + 18*6, 3, m_countOff/2);
			}
			else
			{
				encoderPressed();
			}
		}
	}
	else
	{
		DisplayTask->Clear_str(0, 3, Font::fntSystem, 22*6);
	}
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
			DSP_GuiSendParameter(DSP_ADDRESS_TUN_PROC, 0, 0);

			DisplayTask->Clear();
			DisplayTask->StringOut(6, 1, Font::fntSystem, 0, (uint8_t*)"cp100fx connected as");
			DisplayTask->StringOut(30, 2, Font::fntSystem, 0, (uint8_t*)"mass storage");
			break;
		}

		case TUsbTask::mCDC:
		{
			DisplayTask->Clear();
			DisplayTask->StringOut(6, 1, Font::fntSystem, 0, (uint8_t*)"cp100fx connected as");
			DisplayTask->StringOut(34, 2, Font::fntSystem, 0, (uint8_t*)"serial port");

			// Memory leak
//			currentMenu = mainMenu;
//			mainMenu->show();
			break;
		}
	}

	startUsb();
}

void UsbMenu::encoderClockwise()
{
	if(usbConnected) return;

	if(m_parNum < 1) m_parNum++;

	DisplayTask->StringOut(strPositions[0], 0, Font::fntSystem, 0, &strUsbMenu[0][0]);
	tim5_start(0);

	autoconnectOn = false;
}

void UsbMenu::encoderCounterClockwise()
{
	if(usbConnected) return;

	if(m_parNum > 0) m_parNum--;

	DisplayTask->StringOut(strPositions[1], 1, Font::fntSystem, 0, &strUsbMenu[1][0]);
	tim5_start(0);

	autoconnectOn = false;
}

void UsbMenu::startUsb()
{
	if(usb_connect_type == TUsbTask::mCDC)
	{
		UsbTask = new TUsbTask(TUsbTask::mCDC);

		ConsoleTask = new TConsoleTask(256);
		ConsoleTask->Create("CONS", 20*configMINIMAL_STACK_SIZE, 0);
		ConsoleTask->SetIo(&cdc_io);
		ConsoleTask->Echo(false);

//		ConsoleTask->Clear();
	}
	else
	{
		UsbTask = new TUsbTask(TUsbTask::mMSC);
	}

	UsbTask->Create("USB", 10*configMINIMAL_STACK_SIZE, 0);
}

void UsbMenu::stopUsb()
{
	delete ConsoleTask;
	delete UsbTask;

	usbConnected = false;

	currentMenu = topLevelMenu;
	topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
}
