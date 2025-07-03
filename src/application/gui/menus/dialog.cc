#include "dialog.h"

#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"
#include "BF706_send.h"

#include "presetactionsmenu.h"

#include "preset.h"

Dialog::Dialog(AbstractMenu *parent, TDialogType dialogType)
{
	topLevelMenu = parent;
	m_dialogType = dialogType;

	switch(m_dialogType)
	{
		case TDialogType::SaveChanges:
		{
			m_menuType = MENU_PRESET_SAVE;

			m_btnCount = 3;
			m_nameLenght = 8;

			m_btnNames = new char*[m_btnCount];
			for(int i = 0; i<m_btnCount; i++)
				m_btnNames[i] = new char[m_nameLenght];

			kgp_sdk_libc::strcpy(m_btnNames[0], "No");
			kgp_sdk_libc::strcpy(m_btnNames[1], "Return");
			kgp_sdk_libc::strcpy(m_btnNames[2], "Yes");

			kgp_sdk_libc::strcpy(m_questionString1, "   Save changes? ");

			m_btnPositions[0] = 10;
			m_btnPositions[1] = 40;
			m_btnPositions[2] = 90;
			break;
		}
		case TDialogType::ErasePreset:
		{
			m_menuType = MENU_ERASE;

			m_yesMenu = m_noMenu = parent;

			m_btnCount = 2;
			m_nameLenght = 4;

			m_btnNames = new char*[m_btnCount];
			for(int i = 0; i<m_btnCount; i++)
				m_btnNames[i] = new char[m_nameLenght];

			kgp_sdk_libc::strcpy(m_btnNames[0], "No");
			kgp_sdk_libc::strcpy(m_btnNames[1], "Yes");

			kgp_sdk_libc::strcpy(m_questionString1, "   Set preset   ");
			kgp_sdk_libc::strcpy(m_questionString2, "default settings?");

			m_btnPositions[0] = 30;
			m_btnPositions[1] = 78;
			break;
		}
	}
}

Dialog::~Dialog()
{
	for(int i = 0; i<m_btnCount; i++)
		delete[] m_btnNames[i];
}

void Dialog::setYesMenu(AbstractMenu* yesMenu)
{
	m_yesMenu = yesMenu;
}

void Dialog::setNoMenu(AbstractMenu* noMenu)
{
	m_noMenu = noMenu;
}

void Dialog::show(TShowMode showMode)
{
	currentMenu = this;

	m_paramNum = m_btnCount - 1;

	DisplayTask->Clear();
	DisplayTask->StringOut(10, 2, TDisplayTask::fntSystem, 0, (uint8_t*)m_questionString1);
	DisplayTask->StringOut(10, 3, TDisplayTask::fntSystem, 0, (uint8_t*)m_questionString2);

	for(uint8_t i = 0; i<m_btnCount; i++)
		DisplayTask->StringOut(m_btnPositions[i], 0, TDisplayTask::fntSystem, 0, (uint8_t*)m_btnNames[i]);

	tim5_start(1);
}

void Dialog::task()
{
	DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, TDisplayTask::fntSystem, 3*blinkFlag_fl, (uint8_t*)m_btnNames[m_paramNum]);
}

void Dialog::encoderPressed()
{
	switch(m_paramNum)
	{
		case 2:
			eepr_read_imya(preselectedPresetNumber);

			currentMenu = m_yesMenu;
			m_yesMenu->show();

		break;
		case 0:
			prog_ch();
			eepr_read_imya(preselectedPresetNumber);

			currentMenu = m_noMenu;
			m_noMenu->show();

		break;
		case 1:
			switch(m_dialogType)
			{
				case TDialogType::ErasePreset:
				{
					preset_erase(currentPresetNumber);
					kgp_sdk_libc::memcpy(currentPreset.modules.rawData, prog_data_init, 512);
					currentPreset.modules.rawData[delay_tim_lo] = 0xf4;
					currentPreset.modules.rawData[delay_tim_hi] = 1;

					cab_data_ready = false;
					send_cab_data(1, currentPresetNumber+1, 0);
					if(cab_num==2)
						send_cab_data1(1, currentPresetNumber+1);

					prog_ch();
					break;
				}
				default: break;
			}

			currentMenu = topLevelMenu;
			topLevelMenu->show();
		break;
	}
}

void Dialog::encoderClockwise()
{
	if(m_paramNum < m_btnCount-1)
		DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, TDisplayTask::fntSystem, 0, (uint8_t*)m_btnNames[m_paramNum++]);

	tim5_start(1);
}

void Dialog::encoderCounterClockwise()
{
	if(m_paramNum > 0)
		DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, TDisplayTask::fntSystem, 0, (uint8_t*)m_btnNames[m_paramNum--]);

	tim5_start(0);
}
