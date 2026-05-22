#include "eqbandmenu.h"

#include "preset.h"
#include "modules.h"

#include "display_task.h"

const uint8_t EqBandMenu::paramNameStrings[][2];
const uint8_t EqBandMenu::eqFreqStrings[][10];

EqBandMenu::EqBandMenu(AbstractMenu *parent, uint8_t bandNum)
{
	m_menuType = MENU_EQ;
	topLevelMenu = parent;

	m_bandNum = bandNum;
}

void EqBandMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();

	refresh();

	DisplayTask->StringOut(0, 2, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Press EQ for Default");
	DisplayTask->StringOut(0, 3, Font::fntSystem, Font::fnsNormal, &eqFreqStrings[m_bandNum][0]);
	DisplayTask->StringOut(78, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Q 0.70");
}

void EqBandMenu::task()
{
	if(!encoderKnobSelected)
		DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, FONT_BLINKING, &paramNameStrings[m_paramNum][0]);
}

void EqBandMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;
		DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, Font::fnsHighlight, &paramNameStrings[m_paramNum][0]);
	}
	else
	{
		encoderKnobSelected = 0;
	}

	restartBlinking(0);
}

void EqBandMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(m_paramNum < 1)
		{
			m_paramNum++;
			restartBlinking(0);
			refresh();
		}
	}
	else
	{
		if(m_paramNum == 1)
		{
			int8_t a = currentPreset.paramData.eq_q[m_bandNum];
			if(a<120)
			{
				a = BaseParam::encSpeedInc(a, 120);
				currentPreset.modulesBuf[EQ_Q0 + m_bandNum] = a;
				DisplayTask->EqQ(40, 1, a, m_bandNum);

				eq_q_setter(m_bandNum << 16 | (uint8_t)a);
			}
		}
		else
		{
			int8_t a = currentPreset.paramData.eq_freq[m_bandNum];
			if(a<100)
			{
				int8_t a = currentPreset.paramData.eq_freq[m_bandNum];
				a = BaseParam::encSpeedInc(a, 100);
				currentPreset.modulesBuf[EQ_F0 + m_bandNum] = a;
				DisplayTask->EqFreq(40, 0, a, m_bandNum);

				eq_freq_setter(m_bandNum << 16 | (uint8_t)a);
			}
		}
	}
}

void EqBandMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(m_paramNum>0)
		{
			m_paramNum--;
			restartBlinking(0);
			refresh();
		}
	}
	else
	{
		int8_t a = currentPreset.modulesBuf[EQ_F0+m_bandNum+m_paramNum*5];
		if(m_paramNum == 1)
		{
			int8_t a = currentPreset.paramData.eq_q[m_bandNum];
			if(a>-60)
			{
				a = BaseParam::encSpeedDec(a, -60);
				currentPreset.modulesBuf[EQ_Q0 + m_bandNum] = a;
				DisplayTask->EqQ(40, 1, a, m_bandNum);

				eq_q_setter(m_bandNum << 16 | (uint8_t)a);
			}
		}
		else
		{
			int8_t a = currentPreset.paramData.eq_freq[m_bandNum];
			if(a>-100)
			{
				a = BaseParam::encSpeedDec(a, -100);
				currentPreset.modulesBuf[EQ_F0 + m_bandNum] = a;
				DisplayTask->EqFreq(40, 0, a, m_bandNum);

				eq_freq_setter(m_bandNum << 16 | (uint8_t)a);
			}
		}
	}
}

void EqBandMenu::keyDown()
{

}

void EqBandMenu::key3()
{
	eq_freq_setter(0);
	eq_q_setter(0);

	int8_t a = currentPreset.paramData.eq_freq[m_bandNum];
	DisplayTask->EqFreq(40, 0, a, m_bandNum);
	a = currentPreset.paramData.eq_q[m_bandNum];
	DisplayTask->EqQ(40, 1, a, m_bandNum);
}

void EqBandMenu::refresh()
{
	DisplayTask->StringOut(6, 0, Font::fntSystem,
			(Font::TFontState)(Font::fnsHighlight * (m_paramNum == 0) * blinkFlag), &paramNameStrings[0][0]);
	int8_t a = currentPreset.paramData.eq_freq[m_bandNum];
	DisplayTask->EqFreq(40, 0, a, m_bandNum);

	DisplayTask->StringOut(6, 1, Font::fntSystem,
			(Font::TFontState)(Font::fnsHighlight * (m_paramNum == 1) * blinkFlag), &paramNameStrings[1][0]);
	a = currentPreset.paramData.eq_q[m_bandNum];
	DisplayTask->EqQ(40, 1, a, m_bandNum);
}
