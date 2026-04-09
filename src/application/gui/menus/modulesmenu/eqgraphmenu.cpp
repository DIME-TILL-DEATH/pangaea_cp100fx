#include "eqgraphmenu.h"

#include "filter.h"
#include "preset.h"

#include "modules.h"

#include "display_task.h"

const uint8_t EqGraphMenu::paramNames[][3];
const uint8_t EqGraphMenu::bandNames[][6];

EqGraphMenu::EqGraphMenu(AbstractMenu *parent)
{
	m_menuType = MENU_EQ_GRAPH;
	topLevelMenu = parent;

	for(uint8_t i=0; i<5; i++)
	{
		eqFreq_ptr[i] = (int8_t*)&currentPreset.paramData.eq_freq[i];
		minParamVal[i] = -100;
		maxParamVal[i] = 100;
	}

	eqFreq_ptr[5] = (int8_t*)&currentPreset.paramData.hpf;
	eqFreq_ptr[6] = (int8_t*)&currentPreset.paramData.lpf;
	eqFreq_ptr[7] = (int8_t*)&currentPreset.paramData.eq_pre_post;

	minParamVal[5]=minParamVal[6] = 0;
	maxParamVal[5]=maxParamVal[6] = 127;

	minParamVal[7] = 0;
	maxParamVal[7] = 1;
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
		DisplayTask->StringOut(0, paramNum, Font::fntSystem, FONT_BLINKING, (uint8_t*)&paramNames[paramNum]);
	}

	if(updateResponse)
	{
		if(TIM_GetCounter(TIM6) != 0) return;

		for(uint8_t i = 0; i<4; i++)
		{
			DisplayTask->ClearString(9*6, i, Font::fntSystem, 13);
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
		DisplayTask->StringOut(0, paramNum, Font::fntSystem, Font::fnsHighlight, (uint8_t*)&paramNames[paramNum]);
	}
	else
	{
		encoderKnobSelected = 0;
	}

//	restartBlinking(1);
}

void EqGraphMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		uint8_t avalaibleParams = bandNum < 5 ? 4 : 2;

		if(paramNum < avalaibleParams-1) paramNum++;
		restartBlinking(0);
		printPage();
	}
	else
	{
		switch(paramNum)
		{
			case 0: if(bandNum < 7) bandNum++; break;
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

				switch(bandNum)
				{
					default: eq_freq_setter((bandNum << 16) | (a & 0xFF)); break;
					case 5: hpf_freq_setter(*eqFreq_ptr[bandNum]); break;
					case 6: lpf_freq_setter(*eqFreq_ptr[bandNum]); break;
					case 7: eq_position_setter(*eqFreq_ptr[bandNum]); break;
				}
				break;
			}
			case 2:
			{
				if(bandNum >= 5) break;

				if(currentPreset.paramData.eq_gain[bandNum] < 30)
					currentPreset.paramData.eq_gain[bandNum]++;

				eq_gain_setter((bandNum << 16) | currentPreset.paramData.eq_gain[bandNum]);

				break;
			}
			case 3:
			{
				if(bandNum >= 5) break;

				int8_t a = currentPreset.paramData.eq_q[bandNum];
				if(a < 120) a = BaseParam::encSpeedInc(a, 60);

				eq_q_setter((bandNum << 16) | (uint8_t)a);
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
		restartBlinking(0);
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
				switch(bandNum)
				{
					default: eq_freq_setter((bandNum << 16) | (a & 0xFF)); break;
					case 5: hpf_freq_setter(*eqFreq_ptr[bandNum]); break;
					case 6: lpf_freq_setter(*eqFreq_ptr[bandNum]); break;
					case 7: eq_position_setter(*eqFreq_ptr[bandNum]); break;
				}
				break;
				break;
			}
			case 2:
			{
				if(bandNum >= 5) break;

				if(currentPreset.paramData.eq_gain[bandNum] > 0)
					currentPreset.paramData.eq_gain[bandNum]--;

				eq_gain_setter((bandNum << 16) | currentPreset.paramData.eq_gain[bandNum]);
				break;
			}
			case 3:
			{
				if(bandNum >= 5) break;

				int8_t a = currentPreset.paramData.eq_q[bandNum];
				if(a > -60) a = BaseParam::encSpeedDec(a, -60);

				eq_q_setter((bandNum << 16) | (uint8_t)a);
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
	switch(bandNum)
	{
		case 5: hpf_freq_setter(0); break;
		case 6: lpf_freq_setter(0); break;
		case 7: break;
		default:
		{
			eq_gain_setter(15);
			eq_freq_setter(0);
			eq_q_setter(0);
		}
	}
	printPage();
}

void EqGraphMenu::printPage()
{
	uint8_t paramXPos = 6*2;

	DisplayTask->ClearString(0, 0, Font::fntSystem, 5);
	DisplayTask->ClearString(0, 1, Font::fntSystem, 9);
	DisplayTask->ClearString(0, 2, Font::fntSystem, 8);
	DisplayTask->ClearString(0, 3, Font::fntSystem, 8);

	DisplayTask->StringOut(0, 0, Font::fntSystem, (Font::TFontState)((paramNum==0) * Font::fnsHighlight), (uint8_t*)&paramNames[0]);
	DisplayTask->StringOut(paramXPos, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)&bandNames[bandNum]);

	if(bandNum == 7)
	{
		DisplayTask->StringOut(0, 1, Font::fntSystem, (Font::TFontState)((paramNum==1) * Font::fnsHighlight), (uint8_t*)&"P:");

		if(*eqFreq_ptr[bandNum]) DisplayTask->StringOut(paramXPos, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)&" PRE");
		else DisplayTask->StringOut(paramXPos, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)&" POST");

	}
	else
	{
		DisplayTask->StringOut(0, 1, Font::fntSystem, (Font::TFontState)((paramNum==1) * Font::fnsHighlight), (uint8_t*)&paramNames[1]);
		DisplayTask->EqFreq(paramXPos, 1, *eqFreq_ptr[bandNum], bandNum);
	}

	if(bandNum<5)
	{
		DisplayTask->StringOut(0, 2, Font::fntSystem, (Font::TFontState)((paramNum==2) * Font::fnsHighlight), (uint8_t*)&paramNames[2]);
		char numBuf[15];
		ksprintf(numBuf, "%ddB", currentPreset.paramData.eq_gain[bandNum]-15);
		DisplayTask->StringOut(paramXPos, 2, Font::fntSystem, Font::fnsNormal, (uint8_t*)numBuf);

		DisplayTask->StringOut(0, 3, Font::fntSystem, (Font::TFontState)((paramNum==3) * Font::fnsHighlight), (uint8_t*)&paramNames[3]);
		DisplayTask->EqQ(paramXPos, 3, (int8_t)currentPreset.paramData.eq_q[bandNum], bandNum);
	}
	updateResponse = true;
}

void EqGraphMenu::refresh()
{
	printPage();
}
