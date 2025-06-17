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
	for(int i=0; i<maxParamCount; i++)
	{
		if(paramsList[i]) delete paramsList[i];
	}
}

void ParamListMenu::setParams(BaseParam** settlingParamList, uint8_t setlingParamCount, uint8_t pages)
{
	for(int i=0; i<maxParamCount; i++)
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

void ParamListMenu::show(TShowMode showMode)
{
	current_menu = m_menuType;
	currentMenu = this;

	if(showMode == FirstShow)
	{
		currentPageNumber = -1;

		for(int i = 0; i<paramsCount; i++)
		{
			if(paramsList[i]->type() == BaseParam::GUI_PARAMETER_LIST)
			{
				StringParam* strParam = static_cast<StringParam*>(paramsList[i]);
				uint8_t* valDisableMask = strParam->getDisableMask();
				for(int a=0; a<paramsCount; a++)
				{
					paramsList[a]->setDisabled(valDisableMask[a]);
				}
			}
		}
		encoderKnobSelected = false;
		currentParamNum = 0;
	}

	printPage();
}

void ParamListMenu::task()
{
	if(!encoderKnobSelected)
	{
		DisplayTask->StringOut(leftPad, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								tim5_fl * 2, (uint8_t*)(paramsList[currentParamNum]->name()));
	}
}

void ParamListMenu::encoderPressed()
{
	if(paramsList[currentParamNum]->disabled()) return;

	if(paramsList[currentParamNum]->type() == BaseParam::GUI_PARAMETER_DELAY_TIME ||
			paramsList[currentParamNum]->type() == BaseParam::GUI_PARAMETER_SUBMENU)
	{
		SubmenuParam* submenuParam = static_cast<SubmenuParam*>(paramsList[currentParamNum]);
		submenuParam->showSubmenu(this);
	}
	else
	{
		if(!encoderKnobSelected)
		{
			encoderKnobSelected = true;
			DisplayTask->StringOut(leftPad, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
									2, (uint8_t*)(paramsList[currentParamNum]->name()));
		}
		else
		{
			encoderKnobSelected = false;
			DisplayTask->StringOut(leftPad, currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
									0, (uint8_t*)(paramsList[currentParamNum]->name()));
		}
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
			do{
				currentParamNum++; // Порядок важен!
			}while(paramsList[currentParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY && currentParamNum < paramsCount);
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(paramsList[currentParamNum]->value() - paramsList[currentParamNum]->offset() < paramsList[currentParamNum]->maxValue())
		{
//			increaseParam(paramsList[currentParamNum]);

			paramsList[currentParamNum]->increaseParam();
			printParam(paramsList[currentParamNum], currentParamNum % paramsOnPage);

			if(paramsList[currentParamNum]->bytePosition() != NOT_SEND_POS)
			{
				if(paramsList[currentParamNum]->bytePosition() == PARAM_EQUAL_POS)
					DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(), paramsList[currentParamNum]->value(), 0);
				else
					DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(),	paramsList[currentParamNum]->bytePosition(), paramsList[currentParamNum]->value());
			}
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
			do{
				currentParamNum--; // Порядок важен!
			}while(paramsList[currentParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY && currentParamNum > 0);
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(paramsList[currentParamNum]->value() - paramsList[currentParamNum]->offset() > paramsList[currentParamNum]->minValue())
		{
//			decreaseParam(paramsList[currentParamNum]);
			paramsList[currentParamNum]->decreaseParam();
			printParam(paramsList[currentParamNum], currentParamNum % paramsOnPage);

			if(paramsList[currentParamNum]->bytePosition() != NOT_SEND_POS)
			{
				if(paramsList[currentParamNum]->bytePosition() == PARAM_EQUAL_POS)
					DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(), paramsList[currentParamNum]->value(), 0);
				else
					DSP_gui_set_parameter(paramsList[currentParamNum]->moduleAddress(),	paramsList[currentParamNum]->bytePosition(), paramsList[currentParamNum]->value());
			}
		}
	}

	clean_flag();
}

void ParamListMenu::keyDown()
{
//	if(m_canTap)
//	{
//		// Tremolo tap
//		if(tap_temp_global() && !sys_para[tap_typ])
//		{
//			trem_time = tap_global / 48.0f / tap_tim_v[prog_data[t_tap_t]];
//			if(trem_time < 2731)
//			{
//				if(!sys_para[tim_type])
//				{
//					gui_send(10,5);
//				}
//				else
//				{
//					if(trem_time < 2728 && trem_time > 249)
//					{
//						while(trem_time < bpm_time[temp++]);
//						trem_time = bpm_time[temp];
//						gui_send(10,5);
//					}
//				}
//			}
//
//		}
//		clean_flag();
//	}

	// moog tap
//	if(prog_data[mog_gen_t])
//	{
//		moog_time = tap_global / 48.0f;
//		gui_send(31,13);
//	}
//   clean_flag();

	// delay tap
//	  if(tap_temp_global() && !sys_para[tap_typ])
//	  {
//		  delay_time = tap_global / 3.0f / tap_tim_v[prog_data[d_tap_t]];
//		  if(delay_time < 2731)
//		  {
//			  uint8_t temp = 0;
//			  if(!sys_para[tim_type])
//			  {
//				  if(par_num < 4)DisplayTask->DelayTimeInd(53,1,delay_time);
//    			  gui_send(3,1);
//			  }
//			  else {
//				  if(delay_time < 2728 && delay_time > 249)
//				  {
//					  while(delay_time < bpm_time[temp++]);
//					  delay_time = bpm_time[temp];
//					  prog_data[bpm_del] = 60000 / delay_time;
//					  DisplayTask->ParamIndicNum(53,1,prog_data[bpm_del]);
//					  DisplayTask->StringOut(90,1,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
//	      			  gui_send(3,1);
//				  }
//			  }
//		  }

//	  if(tap_temp_global() && !sys_para[tap_typ])
//	  {
//		  delay_time = tap_global / 3.0f / tap_tim_v[prog_data[d_tap_t]];
//  		  if(delay_time < 2731)
//  		  {
//  			  uint8_t temp = 0;
//  			  if(!sys_para[tim_type])
//  			  {
//  				  if(par_num < 4)DisplayTask->DelayTimeInd(53,0,delay_time);
//      			  gui_send(3,1);
//  			  }
//  			  else {
//  				  if(delay_time < 2728 && delay_time > 249)
//  				  {
//  					  while(delay_time < bpm_time[temp++]);
//  					  delay_time = bpm_time[temp];
//  					  prog_data[bpm_del] = 60000 / delay_time;
//  					  DisplayTask->ParamIndicNum(53,0,prog_data[bpm_del]);
//  					  DisplayTask->StringOut(90,0,TDisplayTask::fntSystem , 0 , (uint8_t*)"BPM");
//  	      			  gui_send(3,1);
//  				  }
//  			  }
//  		  }
//	  }
	  clean_flag();
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
		if(paramsList[displayParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY) continue;
		printParam(paramsList[displayParamNum], i);
		DisplayTask->StringOut(leftPad, i, TDisplayTask::fntSystem , 0, (uint8_t*)(paramsList[displayParamNum]->name()));
	}
}

void ParamListMenu::printParam(BaseParam* param, uint8_t yPos)
{
	if(param->disabled()) return;

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
			DisplayTask->ParamIndicPan(param->xDisplayPosition(), yPos, param->value());
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
		case BaseParam::GUI_PARAMETER_DELAY_TIME:
		{
			DisplayTask->DelayTimeInd(param->xDisplayPosition(), yPos, delay_time);

			if(!sys_para[tim_type]) DisplayTask->DelayTimeInd(param->xDisplayPosition(), yPos, delay_time);
			else
			{
				DisplayTask->ParamIndicNum(param->xDisplayPosition(), yPos, 60000/delay_time);
				DisplayTask->StringOut(param->xDisplayPosition() + 45, yPos,TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
			}
			break;
		}
	}
}

