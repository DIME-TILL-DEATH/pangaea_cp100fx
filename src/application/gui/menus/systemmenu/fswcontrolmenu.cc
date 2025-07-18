#include "fswcontrolmenu.h"

#include "cs.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

#include "gui_task.h"

#include "footswitch.h"

const uint8_t FswControlMenu::presetNumPos[4];
const uint8_t FswControlMenu::strFswType[][12];

FswControlMenu::FswControlMenu(AbstractMenu* parent, TFswControls& fswControls)
{

	topLevelMenu = parent;
	m_menuType = MENU_FSW_CONTROL;

	m_fswControls = fswControls;
}

void FswControlMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strFswType[*(m_fswControls.fs)][0]);

	printPage();
}

void FswControlMenu::task()
{
	if(!m_encoderKnobSelected)
	{
		if(!m_parNum)
		{
			DisplayTask->StringOut(3, 0, Font::fntSystem, 2 * blinkFlag_fl, &expr_menu[0][0]);
		}
		else
		{
			if((*(m_fswControls.fs)==1) && (m_parNum==1))
			{
				DisplayTask->StringOut(52, 2, Font::fntSystem, 2 * blinkFlag_fl, "CC#");
			}
			if(*(m_fswControls.fs) > 2)
			{

				if(blinkFlag_fl)
				{
					str_temp[(m_parNum-1)*4] |= 0x80;
					str_temp[(m_parNum-1)*4+1] |= 0x80;
				}
				else
				{
					str_temp[(m_parNum-1)*4] &= ~0x80;
					str_temp[(m_parNum-1)*4+1] &= ~0x80;
				}
				DisplayTask->StringOut(presetNumPos[*(m_fswControls.fs)-3], 2, Font::fntSystem, 0, &str_temp[0]);
			}
		}
	}
}

void FswControlMenu::encoderPressed()
{
	if(m_encoderKnobSelected) m_encoderKnobSelected = 0;
	else m_encoderKnobSelected = 1;

	tim5_start(0);
}

void FswControlMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(*(m_fswControls.fs) > 2)
		{
			if(!m_parNum)
			{
				DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &expr_menu[0][0]);
			}
			if(m_parNum<(*(m_fswControls.fs)-2))
			{
				str_temp[(m_parNum-1)*4] &= ~0x80;
				str_temp[(m_parNum++ - 1)*4+1] &= ~0x80;
				DisplayTask->StringOut(presetNumPos[*(m_fswControls.fs)-3], 2, Font::fntSystem, 0, &str_temp[0]);

				str_temp[(m_parNum-1)*4] |= 0x80;
				str_temp[(m_parNum-1)*4+1] |= 0x80;
				DisplayTask->StringOut(presetNumPos[*(m_fswControls.fs)-3], 2, Font::fntSystem, 0, &str_temp[0]);
			}
			tim5_start(0);
		}

		if(*(m_fswControls.fs)==1)
		{
			if(!m_parNum)
			{
				m_parNum++;
				DisplayTask->StringOut(3, 0, Font::fntSystem, 0, &expr_menu[0][0]);
				DisplayTask->StringOut(52, 2, Font::fntSystem, 2, "CC#");
				tim5_start(0);
			}
		}
	}
	else
	{
		if(m_parNum == 0)
		{
			if(*(m_fswControls.fs) < 6) *(m_fswControls.fs) += 1;

			DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strFswType[*(m_fswControls.fs)][0]);
			DisplayTask->Clear_str(0, 2, Font::fntSystem, 21);

			printPage();
		}
		else
		{
			if((*(m_fswControls.fs) == Footswitch::FswType::Controller))
			{
				uint8_t a = *(m_fswControls.k1_cc);
				if(a<128)
					a = BaseParam::encSpeedInc(a, 128);
				*(m_fswControls.k1_cc) = a;

				if(a == 0)
					DisplayTask->StringOut(76, 2, Font::fntSystem, 0, "Off");
				else
					DisplayTask->ParamIndicNum(76, 2, a-1);
			}

			if(*(m_fswControls.fs) >= Footswitch::FswType::PresetMap1)
			{
				uint8_t a = *(m_fswControls.pr_start + m_parNum - 1);
				a = BaseParam::encSpeedInc(a, 98);
				if(a>98) a = 0;
				*(m_fswControls.pr_start + m_parNum - 1) = a++;

				str_temp[(m_parNum-1)*4] = ('0'+a/10)|0x80;
				str_temp[(m_parNum-1)*4+1] = ('0'+a%10)|0x80;
				DisplayTask->SymbolOut(presetNumPos[*(m_fswControls.fs)-3]+(m_parNum-1)*24, 2,
						Font::fntSystem, 2, '0'+a/10);
				DisplayTask->SymbolOut(presetNumPos[*(m_fswControls.fs)-3]+(m_parNum-1)*24+6, 2,
						Font::fntSystem, 2, '0'+a%10);
			}
		}
	}
}

void FswControlMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum>0)
		{
			if((*(m_fswControls.fs)==1)&&(m_parNum==1)) DisplayTask->StringOut(52, 2, Font::fntSystem, 0, "CC#");

			// Вывод на одну и ту же позицию, но в первом есть --
			str_temp[(m_parNum - 1)*4] &= ~0x80;
			str_temp[(m_parNum-- - 1)*4 + 1] &= ~0x80;
			DisplayTask->StringOut(presetNumPos[*(m_fswControls.fs)-3], 2, Font::fntSystem, 0, &str_temp[0]);

			str_temp[(m_parNum-1)*4] |= 0x80;
			str_temp[(m_parNum-1)*4+1] |= 0x80;
			DisplayTask->StringOut(presetNumPos[*(m_fswControls.fs)-3], 2, Font::fntSystem, 0, &str_temp[0]);

			if(!m_parNum) DisplayTask->StringOut(3, 0, Font::fntSystem, 2, &expr_menu[0][0]);

			tim5_start(0);
		}
	}
	else if(!m_parNum)
	{
		if(*(m_fswControls.fs)>0) *(m_fswControls.fs) -= 1;

		DisplayTask->StringOut(40, 0, Font::fntSystem, 0, &strFswType[*(m_fswControls.fs)][0]);
		DisplayTask->Clear_str(0, 2, Font::fntSystem, 21);

		printPage();
	}
	else
	{
		if((*(m_fswControls.fs) == Footswitch::FswType::Controller))
		{
			uint8_t a = *(m_fswControls.k1_cc);
			if(a>0)
				a = BaseParam::encSpeedDec(a, 0);
			*(m_fswControls.k1_cc) = a;

			if(!a)
				DisplayTask->StringOut(76, 2, Font::fntSystem, 0, "Off");
			else
				DisplayTask->ParamIndicNum(76, 2, a-1);
		}
		if(*(m_fswControls.fs) >= Footswitch::FswType::PresetMap1)
		{

			int8_t a = *(m_fswControls.pr_start + m_parNum - 1);
			a = BaseParam::encSpeedDec(a, -1);
			if(a<0) a = 98;

			*(m_fswControls.pr_start + m_parNum - 1) = a++;
			str_temp[(m_parNum-1)*4] = ('0'+a/10)|0x80;
			str_temp[(m_parNum-1)*4+1] = ('0'+a%10)|0x80;
			DisplayTask->SymbolOut(presetNumPos[*(m_fswControls.fs)-3]+(m_parNum-1)*24, 2,
					Font::fntSystem, 2, '0'+a/10);
			DisplayTask->SymbolOut(presetNumPos[*(m_fswControls.fs)-3]+(m_parNum-1)*24+6, 2,
					Font::fntSystem, 2, '0'+a%10);
		}
	}
}

void FswControlMenu::strPresetInit(char *adr, uint16_t val)
{
	if(val<10)
		ksprintf(adr, "0%d", val);
	else
		ksprintf(adr, "%d", val);
}

void FswControlMenu::printPage()
{
	kgp_sdk_libc::memset(str_temp, 0, 22);

	if(*(m_fswControls.fs) >= Footswitch::FswType::PresetMap1)
	{
		uint8_t a = *(m_fswControls.fs)-3;

		strPresetInit(str_temp, *(m_fswControls.pr_start) + 1);

		if(a)
		{
			kgp_sdk_libc::memcpy(str_temp + 2, "->", 2);
			strPresetInit(str_temp+4, *(m_fswControls.pr_start + 1) + 1);
		}
		if(a>1)
		{
			kgp_sdk_libc::memcpy(str_temp + 6, "->", 2);
			strPresetInit(str_temp+8, *(m_fswControls.pr_start + 2) + 1);
		}
		if(a>2)
		{
			kgp_sdk_libc::memcpy(str_temp + 10, "->", 2);
			strPresetInit(str_temp+12, *(m_fswControls.pr_start + 3) + 1);
		}

		DisplayTask->StringOut(presetNumPos[a], 2, Font::fntSystem, 0, &str_temp[0]);
	}
	else if(*(m_fswControls.fs) == Footswitch::FswType::Controller)
	{
		DisplayTask->StringOut(52, 2, Font::fntSystem, 0, "CC#");

		if(!*(m_fswControls.k1_cc))
			DisplayTask->StringOut(76, 2, Font::fntSystem, 0, "Off");
		else
			DisplayTask->ParamIndicNum(76, 2, *(m_fswControls.k1_cc)-1);
	}
}
