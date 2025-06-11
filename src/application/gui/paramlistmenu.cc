#include "paramlistmenu.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "fonts/icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

extern volatile int8_t temp;
extern const uint16_t bpm_time[];
extern uint16_t trem_time;

ParamListMenu::ParamListMenu(AbstractMenu* parent, gui_menu_type menuType)
{
	topLevelMenu = parent;
	m_menuType = menuType;
}

ParamListMenu::~ParamListMenu()
{
	for(int i=0; i<64; i++)
	{
		if(paramsList[i]) delete(paramsList[i]);
	}
}

void ParamListMenu::setParams(BaseParam** settlingParamList, uint8_t setlingParamCount, uint8_t pages)
{
	for(int i=0; i<64; i++)
	{
		paramsList[i] = nullptr; // clean up
	}

	paramsCount = setlingParamCount;

	for(int i=0; i<paramsCount; i++)
	{
		paramsList[i] = settlingParamList[i];
	}

	pagesCount = pages; // Библиотека математики ведёт себя странно, плюс требуется приведение к float
}

void ParamListMenu::show()
{
	currentPageNumber = -1;

	printPage();

	encoderKnobSelected = false;
	currentParamNum = 0;
}

void ParamListMenu::task()
{
	if(!encoderKnobSelected)
	{
		DisplayTask->StringOut(6, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								tim5_fl * 2, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
}

void ParamListMenu::encoderPressed()
{
	if(!encoderKnobSelected)
	{
		encoderKnobSelected = true;
		DisplayTask->StringOut(6, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								2, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
	else
	{
		encoderKnobSelected = false;
		DisplayTask->StringOut(6, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								0, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
	tim5_start(1);
	clean_flag();
}

void ParamListMenu::encoderClockwise()
{
	if(!encoderKnobSelected)
	{
		if(currentParamNum < paramsCount - 1)
		{
			currentParamNum++; // Порядок важен!
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(paramsList[currentParamNum]->value() - paramsList[currentParamNum]->offset() < paramsList[currentParamNum]->maxValue())
		{
			if(paramsList[currentParamNum]->stepSize() == 1)
				*(paramsList[currentParamNum]->valuePtr()) = enc_speed_inc(paramsList[currentParamNum]->value(), 0);
			else
				*(paramsList[currentParamNum]->valuePtr()) += paramsList[currentParamNum]->stepSize();

			printParam(paramsList[currentParamNum], currentParamNum % paramsOnPage);

			if(paramsList[currentParamNum]->bytePosition() != NOT_SEND_POS)
							DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(),
									paramsList[currentParamNum]->bytePosition(), paramsList[currentParamNum]->value());
		}
	}

	clean_flag();
}

void ParamListMenu::encoderCounterClockwise()
{
	if(!encoderKnobSelected)
	{
		if(currentParamNum > 0)
		{
			currentParamNum--; // Порядок важен!
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(paramsList[currentParamNum]->value() - paramsList[currentParamNum]->offset() > paramsList[currentParamNum]->minValue())
		{
			if(paramsList[currentParamNum]->stepSize() == 1)
				*(paramsList[currentParamNum]->valuePtr()) = enc_speed_dec(paramsList[currentParamNum]->value(), 0);
			else
				*(paramsList[currentParamNum]->valuePtr()) -= paramsList[currentParamNum]->stepSize();

			printParam(paramsList[currentParamNum], currentParamNum % paramsOnPage);

			if(paramsList[currentParamNum]->bytePosition() != NOT_SEND_POS)
				DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(),
						paramsList[currentParamNum]->bytePosition(), paramsList[currentParamNum]->value());
		}
	}

	clean_flag();
}

void ParamListMenu::keyDown()
{
	if(m_canTap)
	{
		// Tremolo tap
		if(tap_temp_global() && !sys_para[tap_typ])
		{
			trem_time = tap_global / 48.0f / tap_tim_v[prog_data[t_tap_t]];
			if(trem_time < 2731)
			{
				if(!sys_para[tim_type])
				{
					gui_send(10,5);
				}
				else
				{
					if(trem_time < 2728 && trem_time > 249)
					{
						while(trem_time < bpm_time[temp++]);
						trem_time = bpm_time[temp];
						gui_send(10,5);
					}
				}
			}

		}
		clean_flag();
	}
}

void ParamListMenu::printPage()
{
	int8_t newPageNumber;
	if(currentParamNum > 0) newPageNumber = currentParamNum / paramsOnPage;
	else newPageNumber = 0;

	if(newPageNumber != currentPageNumber)
	{
		strelka_t drawStrelka;

		if(newPageNumber < pagesCount - 1) drawStrelka = STRELKA_DOWN;
		if(newPageNumber > 0 && newPageNumber < pagesCount) drawStrelka = STRELKA_UPDOWN;
		if(newPageNumber == pagesCount - 1) drawStrelka = STRELKA_UP;
		if(pagesCount == 1) drawStrelka = STRELKA_NONE;

		DisplayTask->Clear();
		DisplayTask->Icon_Strel(iconFormMenuType(m_menuType), drawStrelka);
		currentPageNumber = newPageNumber;
	}

	uint8_t stringCount = paramsCount - currentPageNumber * paramsOnPage;

	for(uint8_t i = 0; i < stringCount; i++)
	{
		uint8_t displayParamNum = i + currentPageNumber * paramsOnPage;
		printParam(paramsList[displayParamNum], i);
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem , 0, (uint8_t*)(paramsList[displayParamNum]->name()));
	}
}

void ParamListMenu::printParam(BaseParam* param, uint8_t yPos)
{
	switch(param->type())
	{
	case BaseParam::GUI_PARAMETER_DUMMY: break;
	case BaseParam::GUI_PARAMETER_LEVEL:
		DisplayTask->ParamIndic(param->xDisplayPosition(), yPos, param->value());
		break;
	case BaseParam::GUI_PARAMETER_MIX:
		DisplayTask->ParamIndicMix(param->xDisplayPosition(), yPos, param->value());
		break;
	case BaseParam::GUI_PARAMETER_PAN:
		break;
	case BaseParam::GUI_PARAMETER_SUBMENU:
		break;
	case BaseParam::GUI_PARAMETER_LIST:
	{
		StringParam* stringParam = static_cast<StringParam*>(param);
		DisplayTask->StringOut(param->xDisplayPosition(), yPos, TDisplayTask::fntSystem , 0, stringParam->getString(param->value()));
		break;
	}
	case BaseParam::GUI_PARAMETER_NUM:
		DisplayTask->ParamIndicNum(param->xDisplayPosition(), yPos, param->value());
		break;
	}
}
