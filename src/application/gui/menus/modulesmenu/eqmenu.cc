#include "eqmenu.h"

#include "eepr.h"
#include "filter.h"
#include "preset.h"

#include "display_task.h"
#include "io_task.h"
#include "sharc_task.h"

#include "eqbandmenu.h"

const uint8_t lpf_hpf[][9] =
{"HPF", "LPF", "Presence", "Position"};
const uint8_t eq_pre_post[][5] =
{"Post", "Pre "};
const uint8_t gerz[] = "Hz";


EqMenu::EqMenu(AbstractMenu *parent)
{
	m_menuType = MENU_EQ;
	topLevelMenu = parent;
}

void EqMenu::show(TShowMode showMode)
{
	currentMenu = this;

	encoderKnobSelected = false;

	DisplayTask->EqInit();
}

void EqMenu::task()
{
	if(bandNum<5)
	{
		if(!encoderKnobSelected)
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], blinkFlag);
		else
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
	}
	else
	{
		if(!encoderKnobSelected)
		{
			if(blinkFlag==0)
				DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsNormal, (uint8_t*)lpf_hpf+(bandNum-5)*9);
			else
				DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, (uint8_t*)lpf_hpf+(bandNum-5)*9);
		}
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
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, (uint8_t*)lpf_hpf+(bandNum-5)*9);
	}
	else
	{
		encoderKnobSelected = 0;
		if(bandNum<5)
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
		else
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, (uint8_t*)lpf_hpf+(bandNum-5)*9);
	}

	restartBlinking(1);
}

void EqMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(bandNum==4)
		{
			DisplayTask->Clear();
			for(uint8_t i = 0; i<2; i++)
			{
				DisplayTask->StringOut(93, i, Font::fntSystem, Font::fnsNormal, (uint8_t*)gerz);
				DisplayTask->StringOut(6, 1+i, Font::fntSystem, Font::fnsNormal, (uint8_t*)lpf_hpf+(1+i)*9);
			}

			bandNum++;

			DisplayTask->StringOut(6, 0, Font::fntSystem, Font::fnsHighlight, (uint8_t*)lpf_hpf);
			DisplayTask->EqFilter(currentPreset.paramData.hpf*(980.0/127.0)+20.0, 0);
			DisplayTask->StringOut(28, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"(");
			DisplayTask->StringOut(50, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)")");
			DisplayTask->ParamIndNum(33, 0, currentPreset.paramData.hpf);

			DisplayTask->EqFilter(powf(127-currentPreset.paramData.lpf, 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
			DisplayTask->StringOut(28, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"(");
			DisplayTask->StringOut(50, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)")");
			DisplayTask->ParamIndNum(33, 1, currentPreset.paramData.lpf);

			DisplayTask->ParamInd(56, 2, currentPreset.modulesBuf[EQ_PRESENCE]);
			DisplayTask->IconAndArrows(ICON_EQ, STRELKA_UP);
			DisplayTask->StringOut(6, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)&lpf_hpf[3]);
			DisplayTask->StringOut(65, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)&eq_pre_post[currentPreset.modulesBuf[EQ_PREPOST]]);
			restartBlinking(0);
		}
		else
		{
			if((bandNum<8)&&(bandNum>4))
			{
				DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsNormal, (uint8_t*)&lpf_hpf[bandNum++-5]);
				DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, (uint8_t*)&lpf_hpf[bandNum-5]);
				restartBlinking(0);
			}
		}
		if(bandNum<4)
		{
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum++], 0);
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
			restartBlinking(0);
		}
	}
	else
	{
		if(bandNum<5)
		{
			if(currentPreset.modulesBuf[EQ_G0+bandNum]<30)
				DisplayTask->EqInd(27+14*bandNum, 0, ++currentPreset.modulesBuf[EQ_G0+bandNum], 1);
		}
		else
		{
			switch(bandNum)
			{
				case 5:
					if(currentPreset.modulesBuf[EQ_HPF]<127)
					{
						currentPreset.modulesBuf[EQ_HPF] = BaseParam::encSpeedInc(currentPreset.modulesBuf[EQ_HPF], 127);
						DisplayTask->ParamIndNum(33, 0, currentPreset.modulesBuf[EQ_HPF]);
						DisplayTask->EqFilter(currentPreset.modulesBuf[EQ_HPF]*(980.0/127.0)+20.0, 0);
					}
				break;
				case 6:
					if(currentPreset.modulesBuf[EQ_LPF]>0)
					{
						currentPreset.modulesBuf[EQ_LPF] = BaseParam::encSpeedDec(currentPreset.modulesBuf[EQ_LPF], 0);
						DisplayTask->ParamIndNum(33, 1, currentPreset.modulesBuf[EQ_LPF]);
						DisplayTask->EqFilter(powf(127-currentPreset.modulesBuf[EQ_LPF], 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
					}
				break;
				case 7:
					if(currentPreset.modulesBuf[EQ_PRESENCE]<127)
					{
						currentPreset.modulesBuf[EQ_PRESENCE] = BaseParam::encSpeedInc(currentPreset.modulesBuf[EQ_PRESENCE], 127);
						DisplayTask->ParamInd(56, bandNum-5, currentPreset.modulesBuf[EQ_PRESENCE]);
					}
				break;
				case 8:
					if(!currentPreset.modulesBuf[EQ_PREPOST])
					{
						DisplayTask->StringOut(65, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)&eq_pre_post[++currentPreset.modulesBuf[EQ_PREPOST]]);
					}
				break;
			}
		}
	}

	if(bandNum!=8)
		SharcTask->setParameter(DSP_ADDRESS_EQ, bandNum, currentPreset.modulesBuf[EQ_G0 + bandNum]);
	else
		SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_PREPOST_POS, currentPreset.modulesBuf[EQ_PREPOST]);
}

void EqMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(bandNum==5)
		{
			DisplayTask->EqInit();
			DisplayTask->IconAndArrows(ICON_EQ, STRELKA_DOWN);
			bandNum--;
			DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
			restartBlinking(0);
		}
		else
		{
			if((bandNum>0)&&(bandNum<5))
			{
				DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum--], 0);
				DisplayTask->EqInd(27+bandNum*14, 0, currentPreset.modulesBuf[EQ_G0+bandNum], 1);
				restartBlinking(0);
			}
		}
		if(bandNum>5)
		{
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsNormal, (uint8_t*)&lpf_hpf[bandNum-- - 5]);
			DisplayTask->StringOut(6, bandNum-5, Font::fntSystem, Font::fnsHighlight, (uint8_t*)&lpf_hpf[bandNum-5]);
			restartBlinking(0);
		}
	}
	else
	{
		if(bandNum<5)
		{
			if(currentPreset.modulesBuf[EQ_G0+bandNum]>0)
				DisplayTask->EqInd(27+14*bandNum, 0, --currentPreset.modulesBuf[EQ_G0+bandNum], 1);
		}
		else
		{
			switch(bandNum)
			{
				case 5:
					if(currentPreset.modulesBuf[EQ_HPF]>0)
					{
						currentPreset.modulesBuf[EQ_HPF] = BaseParam::encSpeedDec(currentPreset.modulesBuf[EQ_HPF], 0);
						DisplayTask->ParamIndNum(33, 0, currentPreset.modulesBuf[EQ_HPF]);
						DisplayTask->EqFilter(currentPreset.modulesBuf[EQ_HPF]*(980.0/127.0)+20.0, 0);
					}
				break;
				case 6:
					if(currentPreset.modulesBuf[EQ_LPF]<127)
					{
						currentPreset.modulesBuf[EQ_LPF] = BaseParam::encSpeedInc(currentPreset.modulesBuf[EQ_LPF], 127);
						DisplayTask->ParamIndNum(33, 1, currentPreset.modulesBuf[EQ_LPF]);
						DisplayTask->EqFilter(powf(127-currentPreset.modulesBuf[EQ_LPF], 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
					}
				break;
				case 7:
					if(currentPreset.modulesBuf[EQ_PRESENCE]>0)
					{
						currentPreset.modulesBuf[EQ_PRESENCE] = BaseParam::encSpeedDec(currentPreset.modulesBuf[EQ_PRESENCE], 0);
						DisplayTask->ParamInd(56, 2, currentPreset.modulesBuf[EQ_PRESENCE]);
					}
				break;
				case 8:
					if(currentPreset.modulesBuf[EQ_PREPOST])
					{
						DisplayTask->StringOut(65, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)&eq_pre_post[--currentPreset.modulesBuf[EQ_PREPOST]]);
					}
				break;
			}
		}

		if(bandNum!=8)
			SharcTask->setParameter(DSP_ADDRESS_EQ, bandNum, currentPreset.modulesBuf[EQ_G0 + bandNum]);
		else
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_PREPOST_POS, currentPreset.modulesBuf[EQ_PREPOST]);
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
