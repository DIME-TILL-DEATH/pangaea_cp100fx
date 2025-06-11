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
		DisplayTask->StringOut(6, currentParamNum, TDisplayTask::fntSystem, tim5_fl * 2, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
}

void ParamListMenu::encoderPressed()
{
	if(!encoderKnobSelected)
	{
		encoderKnobSelected = true;
		DisplayTask->StringOut(6, currentParamNum % (paramsOnPage - 1), TDisplayTask::fntSystem,
								2, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
	else
	{
		encoderKnobSelected = false;
		DisplayTask->StringOut(6, currentParamNum % (paramsOnPage - 1), TDisplayTask::fntSystem,
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
			printPage();
			currentParamNum++;
			tim5_start(0);
		}
	}
	else
	{
		if(*(paramsList[currentParamNum]->valuePtr()) < paramsList[currentParamNum]->maxValue())
		{
			*(paramsList[currentParamNum]->valuePtr()) = enc_speed_inc(*(paramsList[currentParamNum]->valuePtr()), 0);
			DisplayTask->ParamIndic(paramsList[currentParamNum]->xDisplayPosition(),
									currentParamNum % (paramsOnPage - 1),
									*(paramsList[currentParamNum]->valuePtr()));

			DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(), currentParamNum, *(paramsList[currentParamNum]->valuePtr()));
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
			printPage();
			currentParamNum--;
			tim5_start(0);
		}
	}
	else
	{
		if(*(paramsList[currentParamNum]->valuePtr()) > paramsList[currentParamNum]->minValue())
		{
			*(paramsList[currentParamNum]->valuePtr()) = enc_speed_dec(*(paramsList[currentParamNum]->valuePtr()), 0);
			DisplayTask->ParamIndic(paramsList[currentParamNum]->xDisplayPosition(),
									currentParamNum % (paramsOnPage - 1),
									*(paramsList[currentParamNum]->valuePtr()));

			DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(), currentParamNum, *(paramsList[currentParamNum]->valuePtr()));
		}
	}

	clean_flag();
}

void ParamListMenu::printPage()
{
	int8_t newPageNumber;
	if(currentParamNum > 0) newPageNumber = (currentParamNum - 1) / paramsOnPage;
	else newPageNumber = 0;

	if(newPageNumber != currentPageNumber)
	{
		strelka_t drawStrelka;

		if(pagesCount == 1) drawStrelka = STRELKA_NONE;
		if(newPageNumber < pagesCount) drawStrelka = STRELKA_DOWN;
		if(newPageNumber == pagesCount) drawStrelka = STRELKA_UP;
		if(newPageNumber > 0 && newPageNumber < pagesCount) drawStrelka = STRELKA_UPDOWN;

		DisplayTask->Clear();
		DisplayTask->Icon_Strel(iconFormMenuType(m_menuType), drawStrelka);
		currentPageNumber = newPageNumber;
	}

	uint8_t stringCount = paramsCount - currentPageNumber * paramsOnPage;

	for(uint8_t i = 0; i < stringCount; i++)
	{
		uint8_t displayParamNum = i + currentPageNumber * paramsOnPage;
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem , 0, (uint8_t*)(paramsList[displayParamNum]->name()));
		DisplayTask->ParamIndic(paramsList[displayParamNum]->xDisplayPosition(), i, *(paramsList[displayParamNum]->valuePtr()));
	}
}
