#include "eqmenu.h"

#include "filter.h"
#include "preset.h"
#include "modules.h"

#include "display_task.h"
#include "sharc_task.h"

#include "eqbandmenu.h"

const uint8_t filterNamesStrings[][9] = {"HPF", "LPF", "Presence", "Position"};
const uint8_t eq_pre_post[][5] = {"Post", "Pre "};


EqMenu::EqMenu(AbstractMenu *parent)
{
	m_menuType = MENU_EQ;
	topLevelMenu = parent;
}

void EqMenu::show(TShowMode showMode)
{
	currentMenu = this;

	encoderKnobSelected = false;

	printPage();
}

void EqMenu::task()
{
	if(!encoderKnobSelected)
	{
		if(bandNum<5)
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], blinkFlag);
		else
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem,
					(Font::TFontState)(Font::fnsHighlight * blinkFlag), &filterNamesStrings[bandNum-5][0]);
	}
}

void EqMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;
		if(bandNum<5)
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
		else
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, &filterNamesStrings[bandNum-5][0]);
	}
	else
	{
		encoderKnobSelected = 0;
	}

	restartBlinking(0);
}

void EqMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(bandNum < 9) bandNum++;

		restartBlinking(0);

		if(bandNum == 5) printPage();
		else refresh();
	}
	else
	{
		switch(bandNum)
		{
			case 5:
			{
				if(currentPreset.paramData.hpf < 127)
					hpf_freq_setter((uint8_t)BaseParam::encSpeedInc(currentPreset.paramData.hpf, 127));
				break;
			}
			case 6:
			{
				if(currentPreset.paramData.lpf > 0)
					lpf_freq_setter((uint8_t)BaseParam::encSpeedDec(currentPreset.paramData.lpf, 0));
				break;
			}
			case 7:
			{
				if(currentPreset.paramData.presence < 127)
					amp_presence_setter((uint8_t)BaseParam::encSpeedInc(currentPreset.paramData.presence, 127));
				break;
			}
			case 8:
			{
				if(currentPreset.paramData.eq_pre_post == 0)
					eq_position_setter(++currentPreset.paramData.eq_pre_post);
				break;
			}
			default:
			{
				if(currentPreset.paramData.eq_gain[bandNum] < 30 && bandNum < 5)
					eq_gain_setter(bandNum << 16 | ++currentPreset.paramData.eq_gain[bandNum]);
				break;
			}
		}
		refresh();
	}
}

void EqMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(bandNum > 0) bandNum--;

		restartBlinking(0);

		if(bandNum == 4) printPage();
		else refresh();
	}
	else
	{
		switch(bandNum)
		{
			case 5:
			{
				if(currentPreset.paramData.hpf > 0)
					hpf_freq_setter((uint8_t)BaseParam::encSpeedDec(currentPreset.paramData.hpf, 0));
				break;
			}
			case 6:
			{
				if(currentPreset.paramData.lpf < 127)
					lpf_freq_setter((uint8_t)BaseParam::encSpeedInc(currentPreset.paramData.lpf, 127));
				break;
			}
			case 7:
			{
				if(currentPreset.paramData.presence > 0)
					amp_presence_setter((uint8_t)BaseParam::encSpeedDec(currentPreset.paramData.presence, 0));
				break;
			}
			case 8:
			{
				if(currentPreset.paramData.eq_pre_post > 0)
					eq_position_setter(--currentPreset.paramData.eq_pre_post);
				break;
			}
			default:
			{
				if(currentPreset.paramData.eq_gain[bandNum] > 0 && bandNum < 5)
					eq_gain_setter(bandNum << 16 | --currentPreset.paramData.eq_gain[bandNum]);
				break;
			}
		}
		refresh();
	}
}

void EqMenu::keyDown()
{
	if((!encoderKnobSelected)&&(bandNum<5))
	{
		shownChildMenu = new EqBandMenu(this, bandNum);
		shownChildMenu->show();
	}

	restartBlinking(0);
}

void EqMenu::key3()
{

}

void EqMenu::refresh()
{
	bool blink = encoderKnobSelected ? true : blinkFlag;

	if(bandNum > 4)
	{
		for(uint8_t i = 0; i<4; i++)
		{
			DisplayTask->StringOut(6, i, Font::fntSystem,
					(Font::TFontState)(Font::fnsHighlight * blink * (i == bandNum - 5)), &filterNamesStrings[i][0]);
		}

		DisplayTask->EqFilter(currentPreset.paramData.hpf*(980.0/127.0)+20.0, 0);
		DisplayTask->ParamIndNum(33, 0, currentPreset.paramData.hpf);

		DisplayTask->EqFilter(powf(127-currentPreset.paramData.lpf, 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
		DisplayTask->ParamIndNum(33, 1, currentPreset.paramData.lpf);

		DisplayTask->ParamInd(56, 2, currentPreset.paramData.presence);

		DisplayTask->StringOut(65, 3, Font::fntSystem, Font::fnsNormal, &eq_pre_post[currentPreset.paramData.eq_pre_post][0]);
	}
	else
	{
		for(uint8_t i =0; i < 5; i++)
			DisplayTask->EqInd(27+i*14, 0, currentPreset.paramData.eq_gain[i], (i== bandNum) * blink);
	}
}

void EqMenu::printPage()
{
	bool blink = encoderKnobSelected ? true : blinkFlag;

	if(bandNum > 4)
	{
		DisplayTask->Clear();
		for(uint8_t i = 0; i<4; i++)
		{
			if(i < 2) DisplayTask->StringOut(93, i, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Hz");
			DisplayTask->StringOut(6, i, Font::fntSystem,
					(Font::TFontState)(Font::fnsHighlight * blink * (i == bandNum - 5)), &filterNamesStrings[i][0]);
		}

		DisplayTask->EqFilter(currentPreset.paramData.hpf*(980.0/127.0)+20.0, 0);
		DisplayTask->StringOut(28, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"(");
		DisplayTask->StringOut(50, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)")");
		DisplayTask->ParamIndNum(33, 0, currentPreset.paramData.hpf);

		DisplayTask->EqFilter(powf(127-currentPreset.paramData.lpf, 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
		DisplayTask->StringOut(28, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"(");
		DisplayTask->StringOut(50, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)")");
		DisplayTask->ParamIndNum(33, 1, currentPreset.paramData.lpf);

		DisplayTask->ParamInd(56, 2, currentPreset.paramData.presence);

		DisplayTask->StringOut(65, 3, Font::fntSystem, Font::fnsNormal, &eq_pre_post[currentPreset.paramData.eq_pre_post][0]);

		DisplayTask->IconAndArrows(ICON_EQ, STRELKA_UP);
	}
	else
	{
		DisplayTask->Clear();
		DisplayTask->EqInit();
		DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.paramData.eq_gain[bandNum], blink);
	}
}
