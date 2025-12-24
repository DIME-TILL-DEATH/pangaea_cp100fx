#include "dialog.h"

#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"
#include "init.h"
#include "BF706_send.h"

#include "presetactionsmenu.h"

#include "preset.h"

// TODO: common Dialog to subtypes

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
		case TDialogType::RestartDevice:
		{
			m_menuType = MENU_RESTART;

			m_btnCount = 2;
			m_nameLenght = 7;

			m_btnNames = new char*[m_btnCount];
			for(int i = 0; i<m_btnCount; i++)
				m_btnNames[i] = new char[m_nameLenght];

			kgp_sdk_libc::strcpy(m_btnNames[0], "Cancel ");
			kgp_sdk_libc::strcpy(m_btnNames[1], "Restart");

			kgp_sdk_libc::strcpy(m_questionString1, "Changing cab config");
			kgp_sdk_libc::strcpy(m_questionString2, " needs device reset ");

			strPos = 6;
			m_btnPositions[0] = 6;
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

void Dialog::show(TShowMode showMode)
{
	currentMenu = this;

	m_paramNum = m_btnCount - 1;

	DisplayTask->Clear();
	DisplayTask->StringOut(strPos, 2, Font::fntSystem, 0, (uint8_t*)m_questionString1);
	DisplayTask->StringOut(strPos, 3, Font::fntSystem, 0, (uint8_t*)m_questionString2);

	for(uint8_t i = 0; i<m_btnCount; i++)
		DisplayTask->StringOut(m_btnPositions[i], 0, Font::fntSystem, 0, (uint8_t*)m_btnNames[i]);

	tim5_start(1);
}

void Dialog::keyUp()
{
	topLevelMenu->returnFromChildMenu();
}

void Dialog::task()
{
	DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, Font::fntSystem, 3*blinkFlag_fl, (uint8_t*)m_btnNames[m_paramNum]);
}

void Dialog::encoderPressed()
{
	switch(m_paramNum)
	{
		case 0:
			Preset::Change();


			switch(m_dialogType)
			{
				case TDialogType::SaveChanges:
				{
					topLevelMenu->returnFromChildMenu(TReturnMode::ReturnToRoot);
					break;
				}
				case TDialogType::ErasePreset:
				{
					topLevelMenu->returnFromChildMenu();
					break;
				}
				case TDialogType::RestartDevice:
				{
					write_sys();
					topLevelMenu->returnFromChildMenu();
					break;
				}
			}
		break;

		case 1:
			switch(m_dialogType)
			{
				case TDialogType::ErasePreset:
				{
					Preset::Erase();
					Preset::Change();
					topLevelMenu->returnFromChildMenu();
					break;
				}
				case TDialogType::RestartDevice:
				{
					write_sys();
					NVIC_SystemReset();
					topLevelMenu->returnFromChildMenu(); //dummy
					break;
				}
				case TDialogType::SaveChanges:
				{
					topLevelMenu->returnFromChildMenu();
					break;
				}
			}


		break;

		case 2:
			shownChildMenu = new PresetActionsMenu(this, PresetActionsMenu::Save);
			shownChildMenu->show();
		break;
	}
}

void Dialog::encoderClockwise()
{
	if(m_paramNum < m_btnCount-1)
		DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, Font::fntSystem, 0, (uint8_t*)m_btnNames[m_paramNum++]);

	tim5_start(1);
}

void Dialog::encoderCounterClockwise()
{
	if(m_paramNum > 0)
		DisplayTask->StringOut(m_btnPositions[m_paramNum], 0, Font::fntSystem, 0, (uint8_t*)m_btnNames[m_paramNum--]);

	tim5_start(0);
}
