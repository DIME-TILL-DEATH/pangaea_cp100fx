#include "eqbandmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

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
	current_menu = MENU_EQ_BAND;
	currentMenu = this;

	DisplayTask->Clear();
	DisplayTask->StringOut(6, 0, TDisplayTask::fntSystem, 0, (uint8_t*)eq_p_l);
	int8_t a = presetData[f1+m_bandNum];
	DisplayTask->EqPar(40, 0, a, 0, m_bandNum);
	DisplayTask->StringOut(6, 1, TDisplayTask::fntSystem, 0, (uint8_t*)eq_p_l+2);
	a = presetData[q1+m_bandNum];
	DisplayTask->EqPar(40, 1, a, 1, m_bandNum);
	DisplayTask->StringOut(80, 0, TDisplayTask::fntSystem, 0, (uint8_t*)gerz);
	DisplayTask->StringOut(0, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Press EQ for Default");
	DisplayTask->StringOut(0, 3, TDisplayTask::fntSystem, 0, (uint8_t*)&def_eq_band[m_bandNum]);
	DisplayTask->StringOut(78, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Q 0.70");
}

void EqBandMenu::task()
{
	if(!encoderKnobSelected)
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2*tim5_fl, (uint8_t*)&eq_p_l[m_paramNum]);
}

void EqBandMenu::encoderPressed()
{
	if(encoderKnobSelected==0)
	{
		encoderKnobSelected = 1;
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2, (uint8_t*)&eq_p_l[m_bandNum]);
	}
	else
	{
		DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_p_l[m_bandNum]);
		encoderKnobSelected = 0;
	}

	tim5_start(0);
	clean_flag();
}

void EqBandMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(m_paramNum<1)
		{
			DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_p_l[m_paramNum++]);
			DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2, (uint8_t*)&eq_p_l[m_paramNum]);
			tim5_start(0);
		}
	}
	else
	{
		int8_t a = presetData[f1+m_bandNum+m_paramNum*5];
		if(m_paramNum)
		{
			if(a<120)
			{
				a = enc_speed_inc(a, 120);
				presetData[f1+m_bandNum+m_paramNum*5] = a;
				DisplayTask->EqPar(40, 1, a, m_paramNum, m_bandNum);
				gui_send(22, m_bandNum+m_paramNum*5);
			}
		}
		else
		{
			if(a<100)
			{
				a = enc_speed_inc(a, 100);
				presetData[f1+m_bandNum+m_paramNum*5] = a;
				DisplayTask->EqPar(40, 0, a, m_paramNum, m_bandNum);
				DisplayTask->StringOut(80, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)gerz);
				gui_send(22, m_bandNum+m_paramNum*5);
			}
		}
	}

	clean_flag();
}

void EqBandMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(m_paramNum>0)
		{
			DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)&eq_p_l[m_paramNum--]);
			DisplayTask->StringOut(6, m_paramNum, TDisplayTask::fntSystem, 2, (uint8_t*)&eq_p_l[m_paramNum]);
			tim5_start(0);
		}
	}
	else
	{
		int8_t a = presetData[f1+m_bandNum+m_paramNum*5];
		if(m_paramNum)
		{
			if(a>-60)
			{
				a = enc_speed_dec(a, -60);
				presetData[f1+m_bandNum+m_paramNum*5] = a;
				DisplayTask->EqPar(40, 1, a, m_paramNum, m_bandNum);
				gui_send(22, m_bandNum+m_paramNum*5);
			}
		}
		else
		{
			if(a>-100)
			{
				a = enc_speed_dec(a, -100);
				presetData[f1+m_bandNum+m_paramNum*5] = a;
				DisplayTask->EqPar(40, 0, a, m_paramNum, m_bandNum);
				DisplayTask->StringOut(80, m_paramNum, TDisplayTask::fntSystem, 0, (uint8_t*)gerz);
				gui_send(22, m_bandNum+m_paramNum*5);
			}
		}
	}

	clean_flag();
}

void EqBandMenu::keyDown()
{

}

void EqBandMenu::key3()
{
	presetData[f1+m_bandNum] = 0;
	presetData[f1+m_bandNum+5] = 0;
	DisplayTask->EqPar(40, 0, presetData[f1+m_bandNum], 0, m_bandNum);
	DisplayTask->StringOut(80, 0, TDisplayTask::fntSystem, 0, (uint8_t*)gerz);
	DisplayTask->EqPar(40, 1, presetData[f1+m_bandNum+5], 1, m_bandNum);
	gui_send(22, m_bandNum);
	gui_send(22, m_bandNum+5);

	clean_flag();
}
