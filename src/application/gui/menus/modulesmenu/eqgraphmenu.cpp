#include "eqgraphmenu.h"

#include "appdefs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"
#include "filter.h"

#include "preset.h"

const uint8_t EqGraphMenu::paramNames[][3];
const uint8_t EqGraphMenu::bandNames[][4];

EqGraphMenu::EqGraphMenu(AbstractMenu *parent)
{
	m_menuType = MENU_EQ_GRAPH;
	topLevelMenu = parent;

	for(uint8_t i=0; i<5; i++)
	{
		eqFreq_ptr[i] = (int8_t*)&currentPreset.modules.paramData.eq_freq[i];
		minParamVal[i] = -100;
		maxParamVal[i] = 100;
	}

	eqFreq_ptr[5] = (int8_t*)&currentPreset.modules.paramData.hpf;
	eqFreq_ptr[6] = (int8_t*)&currentPreset.modules.paramData.lpf;
	minParamVal[5]=minParamVal[6] = 0;
	maxParamVal[5]=maxParamVal[6] = 127;
}

void EqGraphMenu::show(TShowMode showMode)
{
	currentMenu = this;
	encoderKnobSelected = false;

	DisplayTask->Clear();

	Filter::calcEqPoints();
	printPage();
}

void EqGraphMenu::task()
{
	if(!encoderKnobSelected)
	{
		DisplayTask->StringOut(0, paramNum, Font::fntSystem, blinkFlag_fl * 2, (uint8_t*)&paramNames[paramNum]);
	}

	if(updateResponse)
	{
		if(TIM_GetCounter(TIM6) != 0) return;

		for(uint8_t i = 0; i<4; i++)
		{
			DisplayTask->Clear_str(9*6, i, Font::fntSystem, 13);
		}
		Filter::calcEqResponse();
		DisplayTask->EqResponse();

		updateResponse = false;
	}
}

void EqGraphMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;

	}
	else
	{
		encoderKnobSelected = 0;
	}

	tim5_start(1);
}

void EqGraphMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		uint8_t avalaibleParams = bandNum < 5 ? 4 : 2;

		if(paramNum < avalaibleParams-1) paramNum++;
		printPage();
	}
	else
	{
		switch(paramNum)
		{
			case 0: if(bandNum < 6) bandNum++; break;
			case 1:
			{
				int8_t a = *eqFreq_ptr[bandNum];

				if(bandNum == 6)
				{
					if(a > minParamVal[bandNum]) a = BaseParam::encSpeedDec(a, minParamVal[bandNum]);
				}
				else
				{
					if(a < maxParamVal[bandNum]) a = BaseParam::encSpeedInc(a, maxParamVal[bandNum]);
				}

				*eqFreq_ptr[bandNum] = a;

				if(bandNum < 5)
				{
					DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + bandNum, *eqFreq_ptr[bandNum]);
				}
				else
				{
					DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, *eqFreq_ptr[bandNum]);
				}
				break;
			}
			case 2:
			{
				if(bandNum >= 5) break;

				if(currentPreset.modules.rawData[EQ_G0+bandNum] < 30)
					currentPreset.modules.rawData[EQ_G0+bandNum]++;

				DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, currentPreset.modules.rawData[EQ_G0 + bandNum]);

				break;
			}
			case 3:
			{
				if(bandNum >= 5) break;

				int8_t a = currentPreset.modules.paramData.eq_q[bandNum];
				if(a < 120) a = BaseParam::encSpeedInc(a, 60);
				currentPreset.modules.paramData.eq_q[bandNum] = a;

				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + bandNum, currentPreset.modules.rawData[EQ_Q0 + bandNum]);
				break;
			}
		}
		printPage();
	}
}

void EqGraphMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(paramNum > 0) paramNum--;
		printPage();
	}
	else
	{
		switch(paramNum)
		{
			case 0: if(bandNum > 0) bandNum--; break;
			case 1:
			{
				int8_t a = *eqFreq_ptr[bandNum];

				if(bandNum == 6)
				{
					if(a < maxParamVal[bandNum]) a = BaseParam::encSpeedInc(a, maxParamVal[bandNum]);
				}
				else
				{
					if(a > minParamVal[bandNum]) a = BaseParam::encSpeedDec(a, minParamVal[bandNum]);
				}

				*eqFreq_ptr[bandNum] = a;
				if(bandNum < 5)
				{
					DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + bandNum, *eqFreq_ptr[bandNum]);
				}
				else
				{
					DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, *eqFreq_ptr[bandNum]);
				}
				break;
			}
			case 2:
			{
				if(bandNum >= 5) break;

				if(currentPreset.modules.rawData[EQ_G0+bandNum] > 0)
					currentPreset.modules.rawData[EQ_G0+bandNum]--;

				DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, currentPreset.modules.rawData[EQ_G0 + bandNum]);
				break;
			}
			case 3:
			{
				if(bandNum >= 5) break;

				int8_t a = currentPreset.modules.paramData.eq_q[bandNum];
				if(a > -60) a = BaseParam::encSpeedDec(a, -60);
				currentPreset.modules.paramData.eq_q[bandNum] = (uint8_t)a;

				DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + bandNum, currentPreset.modules.rawData[EQ_Q0 + bandNum]);
				break;
			}
		}
		printPage();
	}
}

void EqGraphMenu::keyDown()
{

}

void EqGraphMenu::key3()
{
	*eqFreq_ptr[bandNum]= 0;

	if(bandNum<5)
	{
		currentPreset.modules.rawData[EQ_G0 + bandNum] = 15;
		currentPreset.modules.rawData[EQ_F0 + bandNum] = 0;
		currentPreset.modules.rawData[EQ_Q0 + bandNum] = 0;

		DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, currentPreset.modules.rawData[EQ_G0 + bandNum]);
		DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_F0_POS + bandNum, currentPreset.modules.rawData[EQ_F0 + bandNum]);
		DSP_GuiSendParameter(DSP_ADDRESS_EQ_BAND, EQ_Q0_POS + bandNum, currentPreset.modules.rawData[EQ_Q0 + bandNum]);

	}
	else
	{
		DSP_GuiSendParameter(DSP_ADDRESS_EQ, bandNum, *eqFreq_ptr[bandNum]);
	}

	printPage();
}

void EqGraphMenu::printPage()
{
	uint8_t paramXPos = 6*2;

	DisplayTask->Clear_str(0, 0, Font::fntSystem, 5);
	DisplayTask->Clear_str(0, 1, Font::fntSystem, 9);
	DisplayTask->Clear_str(0, 2, Font::fntSystem, 8);
	DisplayTask->Clear_str(0, 3, Font::fntSystem, 8);

	DisplayTask->StringOut(0, 0, Font::fntSystem, (paramNum==0)*2, (uint8_t*)&paramNames[0]);
	DisplayTask->StringOut(paramXPos, 0, Font::fntSystem, 0, (uint8_t*)&bandNames[bandNum]);

	DisplayTask->StringOut(0, 1, Font::fntSystem, (paramNum==1)*2, (uint8_t*)&paramNames[1]);
	DisplayTask->EqFreq(paramXPos, 1, *eqFreq_ptr[bandNum], bandNum);

	if(bandNum<5)
	{
		DisplayTask->StringOut(0, 2, Font::fntSystem, (paramNum==2)*2, (uint8_t*)&paramNames[2]);
		char numBuf[15];
		ksprintf(numBuf, "%ddB", currentPreset.modules.paramData.eq_gain[bandNum]-15);
		DisplayTask->StringOut(paramXPos, 2, Font::fntSystem, 0, (uint8_t*)numBuf);

		DisplayTask->StringOut(0, 3, Font::fntSystem, (paramNum==3)*2, (uint8_t*)&paramNames[3]);
		DisplayTask->EqQ(paramXPos, 3, (int8_t)currentPreset.modules.paramData.eq_q[bandNum], bandNum);
	}
	updateResponse = true;
}
