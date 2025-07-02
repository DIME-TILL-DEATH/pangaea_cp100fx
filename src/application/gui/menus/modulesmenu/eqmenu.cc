#include "eqmenu.h"
#include "eqbandmenu.h"

#include "appdefs.h"
#include "eepr.h"
#include "gui/elements/allFonts.h"
#include "display.h"
#include "enc.h"
#include "BF706_send.h"
#include "filter.h"

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
	current_menu_type = m_menuType;
	currentMenu = this;

	encoderKnobSelected = false;

	DisplayTask->EqInit();
}

void EqMenu::task()
{
	if(bandNum<5)
	{
		if(!encoderKnobSelected)
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], blinkFlag_fl);
		else
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], 1);
	}
	else
	{
		if(blinkFlag_fl==0)
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 0, (uint8_t*)lpf_hpf+(bandNum-5)*9);
		else
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 2, (uint8_t*)lpf_hpf+(bandNum-5)*9);
	}
}

void EqMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;
		if(bandNum<5)
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], 1);
		else
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 2, (uint8_t*)lpf_hpf+(bandNum-5)*9);
	}
	else
	{
		encoderKnobSelected = 0;
		if(bandNum<5)
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], 1);
		else
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 2, (uint8_t*)lpf_hpf+(bandNum-5)*9);
	}

	tim5_start(1);
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
				DisplayTask->StringOut(93, i, TDisplayTask::fntSystem, 0, (uint8_t*)gerz);
				DisplayTask->StringOut(6, 1+i, TDisplayTask::fntSystem, 0, (uint8_t*)lpf_hpf+(1+i)*9);
			}

			bandNum++;

			DisplayTask->StringOut(6, 0, TDisplayTask::fntSystem, 2, (uint8_t*)lpf_hpf);
			DisplayTask->EqLH(presetData[hpf_v]*(980.0/127.0)+20.0, 0);
			DisplayTask->StringOut(28, 0, TDisplayTask::fntSystem, 0, (uint8_t*)"(");
			DisplayTask->StringOut(50, 0, TDisplayTask::fntSystem, 0, (uint8_t*)")");

			DisplayTask->ParamIndicNum(33, 0, presetData[hpf_v]);
			DisplayTask->EqLH(powf(127-presetData[lpf_v], 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
			DisplayTask->StringOut(28, 1, TDisplayTask::fntSystem, 0, (uint8_t*)"(");
			DisplayTask->StringOut(50, 1, TDisplayTask::fntSystem, 0, (uint8_t*)")");

			DisplayTask->ParamIndicNum(33, 1, presetData[lpf_v]);
			DisplayTask->ParamIndic(56, 2, presetData[pre_v]);
			DisplayTask->Icon_Strel(ICON_EQ, STRELKA_UP);
			DisplayTask->StringOut(6, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&lpf_hpf[3]);
			DisplayTask->StringOut(65, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_pre_post[presetData[eq_pr_po]]);
			tim5_start(0);
		}
		else
		{
			if((bandNum<8)&&(bandNum>4))
			{
				DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 0, (uint8_t*)&lpf_hpf[bandNum++-5]);
				DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 2, (uint8_t*)&lpf_hpf[bandNum-5]);
				tim5_start(0);
			}
		}
		if(bandNum<4)
		{
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum++], 0);
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], 1);
			tim5_start(0);
		}
	}
	else
	{
		if(bandNum<5)
		{
			if(presetData[EQ_G0+bandNum]<30)
				DisplayTask->EqIndic(27+14*bandNum, 0, ++presetData[EQ_G0+bandNum], 1);
		}
		else
		{
			switch(bandNum)
			{
				case 5:
					if(presetData[EQ_HPF]<127)
					{
						presetData[EQ_HPF] = enc_speed_inc(presetData[EQ_HPF], 127);
						DisplayTask->ParamIndicNum(33, 0, presetData[EQ_HPF]);
						DisplayTask->EqLH(presetData[EQ_HPF]*(980.0/127.0)+20.0, 0);
					}
				break;
				case 6:
					if(presetData[EQ_LPF]>0)
					{
						presetData[EQ_LPF] = enc_speed_dec(presetData[EQ_LPF], 0);
						DisplayTask->ParamIndicNum(33, 1, presetData[EQ_LPF]);
						DisplayTask->EqLH(powf(127-presetData[EQ_LPF], 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
					}
				break;
				case 7:
					if(presetData[pre_v]<127)
					{
						presetData[pre_v] = enc_speed_inc(presetData[pre_v], 127);
						DisplayTask->ParamIndic(56, bandNum-5, presetData[pre_v]);
					}
				break;
				case 8:
					if(!presetData[EQ_PREPOST])
					{
						DisplayTask->StringOut(65, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_pre_post[++presetData[EQ_PREPOST]]);
					}
				break;
			}
		}
	}

	if(bandNum!=8)
		gui_send(4, bandNum);
	else
		gui_send(4, 12);
}

void EqMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(bandNum==5)
		{
			DisplayTask->EqInit();
			DisplayTask->Icon_Strel(ICON_EQ, STRELKA_DOWN);
			bandNum--;
			DisplayTask->EqIndic(27+bandNum*14, 0, presetData[eq1+bandNum], 1);
			tim5_start(0);
		}
		else
		{
			if((bandNum>0)&&(bandNum<5))
			{
				DisplayTask->EqIndic(27+bandNum*14, 0, presetData[EQ_G0+bandNum--], 0);
				DisplayTask->EqIndic(27+bandNum*14, 0, presetData[EQ_G0+bandNum], 1);
				tim5_start(0);
			}
		}
		if(bandNum>5)
		{
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 0, (uint8_t*)&lpf_hpf[bandNum-- - 5]);
			DisplayTask->StringOut(6, bandNum-5, TDisplayTask::fntSystem, 2, (uint8_t*)&lpf_hpf[bandNum-5]);
			tim5_start(0);
		}
	}
	else
	{
		if(bandNum<5)
		{
			if(presetData[EQ_G0+bandNum]>0)
				DisplayTask->EqIndic(27+14*bandNum, 0, --presetData[EQ_G0+bandNum], 1);
		}
		else
		{
			switch(bandNum)
			{
				case 5:
					if(presetData[hpf_v]>0)
					{
						presetData[hpf_v] = enc_speed_dec(presetData[hpf_v], 0);
						DisplayTask->ParamIndicNum(33, 0, presetData[hpf_v]);
						DisplayTask->EqLH(presetData[hpf_v]*(980.0/127.0)+20.0, 0);
					}
				break;
				case 6:
					if(presetData[lpf_v]<127)
					{
						presetData[lpf_v] = enc_speed_inc(presetData[lpf_v], 127);
						DisplayTask->ParamIndicNum(33, 1, presetData[lpf_v]);
						DisplayTask->EqLH(powf(127-presetData[lpf_v], 2.0)*(19000.0/powf(127.0, 2.0))+1000.0, 1);
					}
				break;
				case 7:
					if(presetData[pre_v]>0)
					{
						presetData[pre_v] = enc_speed_dec(presetData[pre_v], 0);
						DisplayTask->ParamIndic(56, 2, presetData[pre_v]);
					}
				break;
				case 8:
					if(presetData[EQ_PREPOST])
					{
						DisplayTask->StringOut(65, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_pre_post[--presetData[EQ_PREPOST]]);
					}
				break;
			}
		}
		if(bandNum!=8)
			gui_send(4, bandNum);
		else
			gui_send(4, 12);
	}
}

void EqMenu::keyDown()
{
	if((!encoderKnobSelected)&&(bandNum<5))
	{
		shownChildMenu = new EqBandMenu(this, bandNum);
		shownChildMenu->show();
	}

	tim5_start(0);
}

void EqMenu::key3()
{
	Filter::calcEqResponse();

	DisplayTask->Clear();
	DisplayTask->EqResponse();
}
