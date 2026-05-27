#include "cabbrowsermenu.h"

#include "system.h"
#include "preset.h"
#include "fs_browser.h"

#include "bitmaps.h"

#include "display_task.h"
#include "filesystem_task.h"
#include "sdtest_task.h"
#include "ui_task.h"
#include "sharc_task.h"

CabBrowserMenu::CabBrowserMenu(AbstractMenu *parent, uint8_t cabNumber)
{
	topLevelMenu = parent;
	m_menuType = MENU_CABBROWSER;

	m_cabNumber = cabNumber;
}

void CabBrowserMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->SetIndicator(TIndicatorType::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

	if(TSDTestTask::sdInitState == 1)
	{

		DisplayTask->Clear();

		if(TFsBrowser::impulseDirExist)
		{
			FileSystemTask->SendCommand(TFsBrowser::bcCurrent);
			FileSystemTask->SendCommand(TFsBrowser::bcLoadImp);

			processBrowserResponse();
			refresh();
		}
		else
		{
			DisplayTask->StringOut(0, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"There is no directory"); //imp_dir_n
			DisplayTask->StringOut(42, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"IMPULSE");	//imp_dir_no
			UITask->Delay(1000);
			DisplayTask->Clear();

			topLevelMenu->returnFromChildMenu();
		}
	}
	else
	{
		DisplayTask->Clear();

		if(!TSDTestTask::sdInitState)
			DisplayTask->StringOut(6, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"MicroSD is not ready"); //sd_nr
		else
			DisplayTask->StringOut(0, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"MicroSD is loading..");  //sd_lo

		UITask->Delay(1000);
		DisplayTask->Clear();

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::keyUp()
{
	ir_cab_restore(m_cabNumber);
	topLevelMenu->returnFromChildMenu();
}

void CabBrowserMenu::encoderPressed()
{
	FileSystemTask->SendCommand(TFsBrowser::bcAction);

	TFileSystemTask::TResponse browserResponse;
	browserResponse = FileSystemTask->GetResponseBlocking();

	if(browserResponse.responseType == TFileSystemTask::TResponseType::rpFileSelected)
	{
		ir_cab_setter(m_cabNumber);
		topLevelMenu->returnFromChildMenu();
	}
	else
	{
		refresh();
	}
}

void CabBrowserMenu::encoderCounterClockwise()
{
	FileSystemTask->SendCommand(TFsBrowser::bcPrev);

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::encoderClockwise()
{
	FileSystemTask->SendCommand(TFsBrowser::bcNext);

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::processBrowserResponse()
{
	TFileSystemTask::TResponse browserResponse;
	browserResponse = FileSystemTask->GetResponseBlocking();

	switch(browserResponse.responseType)
	{
		case TFileSystemTask::rpFileLoaded:
		{
			if(m_cabNumber == 0)
				SharcTask->sendCab1Data(&browserResponse.file.buffer[0], &browserResponse.file.buffer[CAB_DATA_SIZE]); //gui_send(16, 1);
			else
				SharcTask->sendCab2Data(browserResponse.file.buffer);

			if(m_cabNumber == 0) SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
			else SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);

			switch(m_cabNumber)
			{
				case 0:
				{
					SharcTask->sendCab1Data(&browserResponse.file.buffer[0], &browserResponse.file.buffer[CAB_DATA_SIZE]);
					SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
					break;
				}
				case 1:
				{
					SharcTask->sendCab2Data(browserResponse.file.buffer);
					SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
					break;
				}
				case 2:
				{
					SharcTask->sendCab1Data(&browserResponse.file.buffer[0], &browserResponse.file.buffer[CAB_DATA_SIZE]);
					SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
					SharcTask->sendCab2Data(browserResponse.file.buffer);
					SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
					break;
				}
				default: break;
			}

			ir_on_setter(1);

			break;
		}

		case TFileSystemTask::rpFileInvalid:
		{
			if(m_cabNumber == 0) SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]/2);
			else SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]/2);

			break;
		}

		default: break;
	}
}

void CabBrowserMenu::refresh()
{
	DisplayTask->Clear();
	char dirNameBuf[32];
	kgp_sdk_libc::memset(dirNameBuf, 0, 32);
	kgp_sdk_libc::memcpy(dirNameBuf, FileSystemTask->Object().dir.c_str(), symbolsOnLine(Font::fntSystem));

	if(FileSystemTask->Object().dir.length() > symbolsOnLine(Font::fntSystem))
	{
		dirNameBuf[symbolsOnLine(Font::fntSystem) - 2] = '.';
		dirNameBuf[symbolsOnLine(Font::fntSystem) - 3] = '.';
		dirNameBuf[symbolsOnLine(Font::fntSystem) - 4] = '.';
	}

	DisplayTask->StringOut(4, 0, Font::fntSystem, Font::fnsNormal, dirNameBuf);
	DisplayTask->StringOut(4, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)FileSystemTask->Object().name.c_str());
}
