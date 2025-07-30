#include "eqbandmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

#include "preset.h"

const uint8_t EqBandMenu::gerz[];
const uint8_t EqBandMenu::eq_p_l[][2];
const uint8_t EqBandMenu::def_eq_band[][10];

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
	DisplayTask->StringOut(6, 0, Font::fntSystem, 0, (uint8_t*)eq_p_l);
	int8_t a = currentPreset.modules.rawData[f1+m_bandNum];
	DisplayTask->EqPar(40, 0, a, 0, m_bandNum);
	DisplayTask->StringOut(6, 1, Font::fntSystem, 0, (uint8_t*)eq_p_l+2);
	a = currentPreset.modules.rawData[q1+m_bandNum];
	DisplayTask->EqPar(40, 1, a, 1, m_bandNum);
	DisplayTask->StringOut(80, 0, Font::fntSystem, 0, (uint8_t*)gerz);
	DisplayTask->StringOut(0, 2, Font::fntSystem, 0, (uint8_t*)"Press EQ for Default");
	DisplayTask->StringOut(0, 3, Font::fntSystem, 0, (uint8_t*)&def_eq_band[m_bandNum]);
	DisplayTask->StringOut(78, 3, Font::fntSystem, 0, (uint8_t*)"Q 0.70");
}

void EqBandMenu::task()
{
	if(!encoderKnobSelected)
		DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 2*blinkFlag_fl, (uint8_t*)&eq_p_l[m_paramNum]);
}

void EqBandMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;
		DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 2, (uint8_t*)&eq_p_l[m_bandNum]);
	}
	else
	{
		DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 0, (uint8_t*)&eq_p_l[m_bandNum]);
		encoderKnobSelected = 0;
	}

	tim5_start(0);
}

void EqBandMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(m_paramNum<1)
		{
			DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 0, (uint8_t*)&eq_p_l[m_paramNum++]);
			DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 2, (uint8_t*)&eq_p_l[m_paramNum]);
			tim5_start(0);
		}
	}
	else
	{
		int8_t a = currentPreset.modules.rawData[EQ_F0+m_bandNum+m_paramNum*5];
		if(m_paramNum)
		{
			if(a<120)
			{
				a = BaseParam::encSpeedInc(a, 120);
				currentPreset.modules.rawData[EQ_Q0 + m_bandNum] = a;
				DisplayTask->EqPar(40, 1, a, m_paramNum, m_bandNum);
				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + m_bandNum, currentPreset.modules.rawData[EQ_Q0 + m_bandNum]);
			}
		}
		else
		{
			if(a<100)
			{
				a = BaseParam::encSpeedInc(a, 100);
				currentPreset.modules.rawData[EQ_F0 + m_bandNum] = a;
				DisplayTask->EqPar(40, 0, a, m_paramNum, m_bandNum);
				DisplayTask->StringOut(80, m_paramNum, Font::fntSystem, 0, (uint8_t*)gerz);
				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + m_bandNum, currentPreset.modules.rawData[EQ_F0 + m_bandNum]);
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
			DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 0, (uint8_t*)&eq_p_l[m_paramNum--]);
			DisplayTask->StringOut(6, m_paramNum, Font::fntSystem, 2, (uint8_t*)&eq_p_l[m_paramNum]);
			tim5_start(0);
		}
	}
	else
	{
		int8_t a = currentPreset.modules.rawData[f1+m_bandNum+m_paramNum*5];
		if(m_paramNum)
		{
			if(a>-60)
			{
				a = BaseParam::encSpeedDec(a, -60);
				currentPreset.modules.rawData[EQ_Q0 + m_bandNum] = a;
				DisplayTask->EqPar(40, 1, a, m_paramNum, m_bandNum);
				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + m_bandNum, currentPreset.modules.rawData[EQ_Q0 + m_bandNum]);
			}
		}
		else
		{
			if(a>-100)
			{
				a = BaseParam::encSpeedDec(a, -100);
				currentPreset.modules.rawData[EQ_F0 + m_bandNum] = a;
				DisplayTask->EqPar(40, 0, a, m_paramNum, m_bandNum);
				DisplayTask->StringOut(80, m_paramNum, Font::fntSystem, 0, (uint8_t*)gerz);
				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + m_bandNum, currentPreset.modules.rawData[EQ_F0 + m_bandNum]);
			}
		}
	}
}

void EqBandMenu::keyDown()
{

}

void EqBandMenu::key3()
{
	currentPreset.modules.rawData[EQ_F0+m_bandNum] = 0;
	currentPreset.modules.rawData[EQ_Q0+m_bandNum] = 0;
	DisplayTask->EqPar(40, 0, currentPreset.modules.rawData[f1+m_bandNum], 0, m_bandNum);
	DisplayTask->StringOut(80, 0, Font::fntSystem, 0, (uint8_t*)gerz);
	DisplayTask->EqPar(40, 1, currentPreset.modules.rawData[f1+m_bandNum+5], 1, m_bandNum);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + m_bandNum, currentPreset.modules.rawData[EQ_Q0 + m_bandNum]);
	DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + m_bandNum, currentPreset.modules.rawData[EQ_F0 + m_bandNum]);
}
