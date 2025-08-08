#include "cabbrowsermenu.h"

#include "appdefs.h"
#include "init.h"

#include "cs.h"
#include "fs.h"

#include "allFonts.h"
#include "display.h"

#include "BF706_send.h"

#include "preset.h"

#include "fs_browser.h"
#include "sd_test.h"

CabBrowserMenu::CabBrowserMenu(AbstractMenu *parent, uint8_t cabNumber)
{
	topLevelMenu = parent;
	m_menuType = MENU_CABBROWSER;

	m_cabNumber = cabNumber;
}

void CabBrowserMenu::show(TShowMode showMode)
{
	currentMenu = this;

	if(TSD_TESTTask::sdInitState == 1)
	{

		DisplayTask->Clear();

		if(TFsBrowser::impulseDirExist)
		{
			FSTask->SendCommand(TFsBrowser::bcCurrent);
			FSTask->SendCommand(TFsBrowser::bcLoadImp);
			CSTask->Give();

			processBrowserResponse();
			refresh();
		}
		else
		{
			DisplayTask->StringOut(0, 1, Font::fntSystem, 0, (uint8_t*)"There is no directory"); //imp_dir_n
			DisplayTask->StringOut(42, 3, Font::fntSystem, 0, (uint8_t*)"IMPULSE");	//imp_dir_no
			CSTask->CS_del(1000);
			DisplayTask->Clear();

			topLevelMenu->returnFromChildMenu();
		}
	}
	else
	{
		DisplayTask->Clear();

		if(!TSD_TESTTask::sdInitState)
			DisplayTask->StringOut(6, 1, Font::fntSystem, 0, (uint8_t*)"MicroSD is not ready"); //sd_nr
		else
			DisplayTask->StringOut(0, 1, Font::fntSystem, 0, (uint8_t*)"MicroSD is loading..");  //sd_lo

		CSTask->CS_del(1000);
		DisplayTask->Clear();

		topLevelMenu->returnFromChildMenu();
	}
}

void CabBrowserMenu::keyUp()
{
//	DisplayTask->Clear();

	if(m_cabNumber == 0)
	{
		DSP_SendPrimaryCabData(cab1.data);
		DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]);
	}
	else
	{
		DSP_SendSecondaryCabData(cab2.data);
		DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]);
	}
	topLevelMenu->returnFromChildMenu();
}

void CabBrowserMenu::encoderPressed()
{
	FSTask->SendCommand(TFsBrowser::bcAction);
	CSTask->Give();

	TCSTask::TResponse browserResponse;
	browserResponse = CSTask->GetResponseBlocking();

	if(browserResponse.responseType == TCSTask::TResponseType::rpFileSelected)
	{
		DisplayTask->Clear();

		if(m_cabNumber==0)
		{
			kgp_sdk_libc::memcpy(cab1.data, presetBuffer, 4096 * 3);
			if(cab_type != CAB_CONFIG_STEREO) kgp_sdk_libc::memcpy(cab1.data + 4096 * 3, presetBuffer + 4096 * 3, 4096 * 3);

			kgp_sdk_libc::memcpy(cab1.name.string, selectedCabName, 64);

			DSP_SendPrimaryCabData(cab1.data);
			DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]);
		}
		else
		{
			kgp_sdk_libc::memcpy(cab2.data, presetBuffer, 4096 * 3);
			kgp_sdk_libc::memcpy(cab2.name.string, selectedCabName, 64);

			DSP_SendSecondaryCabData(cab2.data);
			DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]);
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
	FSTask->SendCommand(TFsBrowser::bcUp);
	CSTask->Give();

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::encoderClockwise()
{
	FSTask->SendCommand(TFsBrowser::bcDown);
	CSTask->Give();

	processBrowserResponse();
	refresh();
}

void CabBrowserMenu::processBrowserResponse()
{
	TCSTask::TResponse browserResponse;
	browserResponse = CSTask->GetResponseBlocking();

	switch(browserResponse.responseType)
	{
		case TCSTask::rpFileLoaded:
		{
			kgp_sdk_libc::memcpy(selectedCabName, browserResponse.file.name, 64);

			if(m_cabNumber == 0)
				DSP_SendPrimaryCabData(browserResponse.file.buffer); //gui_send(16, 1);
			else
				DSP_SendSecondaryCabData(browserResponse.file.buffer);

			if(m_cabNumber == 0) DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]);
			else DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]);

			break;
		}

		case TCSTask::rpFileInvalid:
		{
			if(m_cabNumber == 0) DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]/2);
			else DSP_GuiSendParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]/2);

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
				(uint8_t*)FSTask->Object().dir.c_str());
		DisplayTask->StringOut(4, 1, Font::fntSystem, Font::fnsBlack,
				(uint8_t*)FSTask->Object().name.c_str());
}
