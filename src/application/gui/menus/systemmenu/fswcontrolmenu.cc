#include "fswcontrolmenu.h"

#include "footswitch.h"
#include "eeprom.h"
#include "system.h"

#include "display_task.h"
#include "io_task.h"
#include "ui_task.h"

const uint8_t FswControlMenu::presetNumPos[4];
const uint8_t FswControlMenu::strFswType[][12];

FswControlMenu::FswControlMenu(AbstractMenu* parent, Footswitch::FswButton button, Footswitch::FswModeName mode)
{
	topLevelMenu = parent;
	m_menuType = MENU_FSW_CONTROL;
	m_button = button;
	m_mode = mode;

	if(mode == Footswitch::onPress)
	{
		fsType_ptr = &sys_para[System::FSW1_PRESS_TYPE + m_button];
		mapPreset = &sys_para[System::FSW1_PRESS_PR1 + m_button*4];
		ctrlCC_ptr = &sys_para[System::FSW1_CTRL_PRESS_CC + m_button];
	}
	else
	{
		fsType_ptr = &sys_para[System::FSW1_HOLD_TYPE + m_button];
		mapPreset = &sys_para[System::FSW1_HOLD_PR1 + m_button*4];
		ctrlCC_ptr = &sys_para[System::FSW1_CTRL_HOLD_CC + m_button];
	}
}

void FswControlMenu::show(TShowMode showMode)
{
	currentMenu = this;
	DisplayTask->Clear();
	printPage();
}

void FswControlMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum == 0)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, FONT_BLINKING, "Type");
		}
		else
		{
			if((*(fsType_ptr) == Footswitch::FswType::Controller) && (m_parNum==1))
			{
				DisplayTask->StringOut(52, 2, Font::fntSystem, FONT_BLINKING, "CC#");
			}

			if(*(fsType_ptr) >= Footswitch::FswType::PresetMap1)
			{

				if(blinkFlag)
				{
					presetMapString[(m_parNum-1)*4] |= 0x80;
					presetMapString[(m_parNum-1)*4+1] |= 0x80;
				}
				else
				{
					presetMapString[(m_parNum-1)*4] &= ~0x80;
					presetMapString[(m_parNum-1)*4+1] &= ~0x80;
				}
				DisplayTask->StringOut(presetNumPos[*(fsType_ptr)-3], 2, Font::fntSystem, Font::fnsNormal, &presetMapString[0]);
			}
		}
	}
}

void FswControlMenu::refresh()
{
	printPage();
}

void FswControlMenu::encoderPressed()
{
	if(m_encoderKnobSelected) m_encoderKnobSelected = 0;
	else m_encoderKnobSelected = 1;

	printPage();
}

void FswControlMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(*(fsType_ptr) > 2)
		{
			if(m_parNum < (*(fsType_ptr)-2))
			{
				m_parNum++;
			}
		}

		if(*(fsType_ptr) == Footswitch::FswType::Controller)
		{
			if(m_parNum == 0) m_parNum++;
		}
	}
	else
	{
		if(m_parNum == 0)
		{
			if(*(fsType_ptr) < 6) *(fsType_ptr) += 1;

			if(m_mode == Footswitch::onPress)
				Footswitch::setPressType(m_button, static_cast<Footswitch::FswType>(*(fsType_ptr)));
			else
				Footswitch::setHoldType(m_button, static_cast<Footswitch::FswType>(*(fsType_ptr)));
		}
		else
		{
			if((*(fsType_ptr) == Footswitch::FswType::Controller))
			{
				uint8_t a = *(ctrlCC_ptr);
				if(a<128)
					a = BaseParam::encSpeedInc(a, 128);
				*(ctrlCC_ptr) = a;

				if(m_mode == Footswitch::onPress)
					Footswitch::setPressCc(m_button, a);
				else
					Footswitch::setHoldCc(m_button, a);
			}

			if(*(fsType_ptr) >= Footswitch::FswType::PresetMap1)
			{
				uint8_t a = mapPreset[m_parNum - 1];

				a = BaseParam::encSpeedInc(a, 98);
				if(a>98) a = 0;

				if(m_mode == Footswitch::onPress)
					Footswitch::setPressMap(m_button, m_parNum - 1, a);
				else
					Footswitch::setHoldMap(m_button, m_parNum - 1, a);
			}
		}
	}

	printPage();
}

void FswControlMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum>0) m_parNum--;
	}
	else
	{
		if(m_parNum == 0)
		{
			if(*(fsType_ptr)>0) *(fsType_ptr) -= 1;

			if(m_mode == Footswitch::onPress)
				Footswitch::setPressType(m_button, static_cast<Footswitch::FswType>(*(fsType_ptr)));
			else
				Footswitch::setHoldType(m_button, static_cast<Footswitch::FswType>(*(fsType_ptr)));
		}
		else
		{
			if((*(fsType_ptr) == Footswitch::FswType::Controller))
			{
				uint8_t a = *(ctrlCC_ptr);
				if(a>0)
					a = BaseParam::encSpeedDec(a, 0);
				*(ctrlCC_ptr) = a;

				if(m_mode == Footswitch::onPress)
					Footswitch::setPressCc(m_button, a);
				else
					Footswitch::setHoldCc(m_button, a);
			}

			if(*(fsType_ptr) >= Footswitch::FswType::PresetMap1)
			{

				int8_t a = mapPreset[m_parNum - 1];
				a = BaseParam::encSpeedDec(a, -1);
				if(a<0) a = 98;

				if(m_mode == Footswitch::onPress)
					Footswitch::setPressMap(m_button, m_parNum - 1, a);
				else
					Footswitch::setHoldMap(m_button, m_parNum - 1, a);
			}
		}
	}
	printPage();
}

void FswControlMenu::presetNumToString(char *adr, uint16_t val)
{
	if(val<10)
		ksprintf(adr, "0%d", val);
	else
		ksprintf(adr, "%d", val);
}

void FswControlMenu::printPage()
{
	kgp_sdk_libc::memset(presetMapString, 0, 22);

	DisplayTask->StringOut(3, 0, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 0)), "Type");

	DisplayTask->StringOut(40, 0, Font::fntSystem, Font::fnsNormal, &strFswType[*(fsType_ptr)][0]);
	DisplayTask->ClearString(0, 2, Font::fntSystem, 21);

	if(*(fsType_ptr) >= Footswitch::FswType::PresetMap1)
	{
		uint8_t a = *(fsType_ptr)-3;

		presetNumToString(presetMapString, mapPreset[0] + 1);

		if(a)
		{
			kgp_sdk_libc::memcpy(presetMapString + 2, "->", 2);
			presetNumToString(presetMapString+4, mapPreset[1] + 1);
		}
		if(a>1)
		{
			kgp_sdk_libc::memcpy(presetMapString + 6, "->", 2);
			presetNumToString(presetMapString+8, mapPreset[2] + 1);
		}
		if(a>2)
		{
			kgp_sdk_libc::memcpy(presetMapString + 10, "->", 2);
			presetNumToString(presetMapString+12, mapPreset[3] + 1);
		}

		if(m_parNum > 0)
		{
			presetMapString[(m_parNum-1)*4] |= 0x80;
			presetMapString[(m_parNum-1)*4+1] |= 0x80;
		}

		DisplayTask->StringOut(presetNumPos[a], 2, Font::fntSystem, Font::fnsNormal, presetMapString);
	}
	else if(*(fsType_ptr) == Footswitch::FswType::Controller)
	{
		DisplayTask->StringOut(52, 2, Font::fntSystem, (Font::TFontState)(Font::fnsHighlight * (m_parNum == 1)), "CC#");

		if(*(ctrlCC_ptr) == 0)
			DisplayTask->StringOut(76, 2, Font::fntSystem, Font::fnsNormal, "Off");
		else
			DisplayTask->ParamIndNum(76, 2, *(ctrlCC_ptr)-1);
	}

	restartBlinking(0);
}
