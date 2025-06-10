#include "paramlistmenu.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

ParamListMenu::ParamListMenu(AbstractMenu* parent, gui_menu_type menuType)
{
	topLevelMenu = parent;
	m_menuType = menuType;
}

void ParamListMenu::setParams(BaseParam** settlingParamList, uint8_t setlingParamCount)
{
	paramsCount = setlingParamCount;

	for(int i=0; i<paramsCount; i++)
	{
		paramsList[i] = settlingParamList[i];
	}
}

void ParamListMenu::show()
{
	DisplayTask->Clear();
	DisplayTask->Icon_Strel(14, 0);

	for(uint8_t i = 0; i < min((uint8_t)paramsCount, (uint8_t)(paramsOnPage - 1)); i++)
	{
		DisplayTask->StringOut(6, i, TDisplayTask::fntSystem , 0, (uint8_t*)(paramsList[i]->name()));
		DisplayTask->ParamIndic(paramsList[i]->xDisplayPosition(), i, *(paramsList[i]->valuePtr()));
	}

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
			DisplayTask->StringOut(6, currentParamNum, TDisplayTask::fntSystem, 0, (uint8_t*)(paramsList[currentParamNum]->name()));
			currentParamNum++;
			DisplayTask->StringOut(6, currentParamNum, TDisplayTask::fntSystem, 2, (uint8_t*)(paramsList[currentParamNum]->name()));
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
			DisplayTask->StringOut(6, par_num,TDisplayTask::fntSystem, 0, (uint8_t*)(paramsList[currentParamNum]->name()));
			currentParamNum--;
			DisplayTask->StringOut(6, par_num,TDisplayTask::fntSystem, 2, (uint8_t*)(paramsList[currentParamNum]->name()));
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
