#include "cabbrowsermenu.h"

#include "system.h"
#include "preset.h"

#include "fs_browser.h"

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

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);

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
			DisplayTask->StringOut(0, 1, Font::fntSystem, 0, (uint8_t*)"There is no directory"); //imp_dir_n
			DisplayTask->StringOut(42, 3, Font::fntSystem, 0, (uint8_t*)"IMPULSE");	//imp_dir_no
			UITask->Delay(1000);
			DisplayTask->Clear();

			topLevelMenu->returnFromChildMenu();
		}
	}
	else
	{
		DisplayTask->Clear();

		if(!TSDTestTask::sdInitState)
			DisplayTask->StringOut(6, 1, Font::fntSystem, 0, (uint8_t*)"MicroSD is not ready"); //sd_nr
		else
			DisplayTask->StringOut(0, 1, Font::fntSystem, 0, (uint8_t*)"MicroSD is loading..");  //sd_lo

		UITask->Delay(1000);
		DisplayTask->Clear();

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::keyUp()
{
	if(m_cabNumber == 0)
	{
		SharcTask->sendCab1Data(currentPreset.cab1Data, currentPreset.cabAuxData);
		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
	}
	else
	{
		SharcTask->sendCab2Data(currentPreset.cab2Data);
		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
	}
	topLevelMenu->returnFromChildMenu();
}

void CabBrowserMenu::encoderPressed()
{
	FileSystemTask->SendCommand(TFsBrowser::bcAction);

	TUITask::TResponse browserResponse;
	browserResponse = UITask->GetResponseBlocking();

	if(browserResponse.responseType == TUITask::TResponseType::rpFileSelected)
	{
		DisplayTask->Clear();

		if(m_cabNumber==0)
		{
			kgp_sdk_libc::memcpy(currentPreset.cab1Data, tempCabBuffer, CAB_DATA_SIZE);
			if(cab_type != CAB_CONFIG_STEREO)
				kgp_sdk_libc::memcpy(currentPreset.cabAuxData, tempCabBuffer + CAB_DATA_SIZE, CAB_DATA_SIZE);

			kgp_sdk_libc::memcpy(&currentPreset.cab1Name, selectedCabName, CAB_DATA_SIZE - 1);
			currentPreset.cab1NameSize = kgp_sdk_libc::strlen((const char*)currentPreset.cab1Name);

			SharcTask->sendCab1Data(currentPreset.cab1Data, currentPreset.cabAuxData);
			SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
		}
		else
		{
			kgp_sdk_libc::memcpy(currentPreset.cab2Data, tempCabBuffer, CAB_DATA_SIZE);
			kgp_sdk_libc::memcpy(&currentPreset.cab2Name, selectedCabName, CAB_DATA_SIZE - 1);
			currentPreset.cab2NameSize = kgp_sdk_libc::strlen((const char*)currentPreset.cab2Name);

			SharcTask->sendCab2Data(currentPreset.cab2Data);
			SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
		}

		topLevelMenu->returnFromChildMenu();
	}
	else
	{
		refresh();
	}
}

void CabBrowserMenu::encoderCounterClockwise()
{
	FileSystemTask->SendCommand(TFsBrowser::bcUp);

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::encoderClockwise()
{
	FileSystemTask->SendCommand(TFsBrowser::bcDown);

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::processBrowserResponse()
{
	TUITask::TResponse browserResponse;
	browserResponse = UITask->GetResponseBlocking();

	switch(browserResponse.responseType)
	{
		case TUITask::rpFileLoaded:
		{
			kgp_sdk_libc::memcpy(selectedCabName, browserResponse.file.name, 64);

			if(m_cabNumber == 0)
				SharcTask->sendCab1Data(&browserResponse.file.buffer[0], &browserResponse.file.buffer[CAB_DATA_SIZE]); //gui_send(16, 1);
			else
				SharcTask->sendCab2Data(browserResponse.file.buffer);

			if(m_cabNumber == 0) SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
			else SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);

			break;
		}

		case TUITask::rpFileInvalid:
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
	//	Delay(10);
		DisplayTask->Clear();
		DisplayTask->StringOut(4, 0, Font::fntSystem, Font::fnsBlack,
				(uint8_t*)FileSystemTask->Object().dir.c_str());
		DisplayTask->StringOut(4, 1, Font::fntSystem, Font::fnsBlack,
				(uint8_t*)FileSystemTask->Object().name.c_str());
}
