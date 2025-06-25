#include "paramlistmenu.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "gui/allFonts.h"
#include "gui/icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"

#include "params/stringoutparam.h"

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
	for(int i=0; i<m_paramsCount; i++)
	{
		if(m_paramsList[i]) delete m_paramsList[i];
	}
}

void ParamListMenu::setParams(BaseParam** settlingParamList, uint8_t setlingParamCount)
{
	for(int i=0; i<maxParamCount; i++)
	{
		m_paramsList[i] = nullptr; // clean up
	}

	m_paramsCount = setlingParamCount;

	for(int i=0; i<m_paramsCount; i++)
	{
		m_paramsList[i] = settlingParamList[i];
	}

	m_pagesCount = ceil((float)m_paramsCount/(float)paramsOnPage);
}

void ParamListMenu::setVolumeIndicator(TDisplayTask::TVolIndicatorType volIndicatorType, dsp_indicator_source_t indicatorSource)
{
	m_volIndicatorType = volIndicatorType;
	m_indicatorSource = indicatorSource;
}

void ParamListMenu::setIcon(bool drawIcon, uint8_t iconNumber)
{
	ParamListMenu::m_drawIcon = drawIcon;
}

void ParamListMenu::show(TShowMode showMode)
{
	current_menu = m_menuType;
	currentMenu = this;

	DisplayTask->Clear();

	if(showMode == FirstShow)
	{
		m_currentPageNumber = -1;

//		for(int a=0; a<m_paramsCount; a++) m_paramsList[a]->setDisabled(0); //cleanup

		for(int i = 0; i<m_paramsCount; i++)
		{
			if(m_paramsList[i]->type() == BaseParam::GUI_PARAMETER_LIST)
			{
				StringListParam* strParam = static_cast<StringListParam*>(m_paramsList[i]);
				uint8_t* valDisableMask = strParam->getDisableMask();
				for(int a=0; a<m_paramsCount; a++)
				{
					m_paramsList[a]->setDisabled(m_paramsList[a]->disabled() | valDisableMask[a]);
				}
			}
		}
		m_encoderKnobSelected = false;
		m_firstSelectableParam = 0;
		while(m_paramsList[m_firstSelectableParam]->type() == BaseParam::GUI_PARAMETER_DUMMY
			||m_paramsList[m_firstSelectableParam]->type() == BaseParam::GUI_PARAMETER_STRING_OUT) m_firstSelectableParam++;

		m_currentParamNum = m_firstSelectableParam;
		m_lastSelectableParam = m_paramsCount - 1;
	}

	printPage();
	DisplayTask->SetVolIndicator(m_volIndicatorType, m_indicatorSource);
}

void ParamListMenu::task()
{
	if(m_paramsCount == 1) m_encoderKnobSelected = true;

	if(!m_encoderKnobSelected)
	{
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
								tim5_fl * 2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
	}
}

void ParamListMenu::encoderPressed()
{
	if(m_paramsList[m_currentParamNum]->disabled()) return;

	// CustomParam
	if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME ||
			m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_SUBMENU)
	{
		SubmenuParam* submenuParam = static_cast<SubmenuParam*>(m_paramsList[m_currentParamNum]);
		submenuParam->showSubmenu(this);
	}
	else
	{
		if(!m_encoderKnobSelected)
		{
			m_encoderKnobSelected = true;
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
									2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
		else
		{
			m_encoderKnobSelected = false;
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, TDisplayTask::fntSystem,
									0, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
	}

	tim5_start(1);
	clean_flag();
}

void ParamListMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum < m_lastSelectableParam) //(paramsCount - 1)
		{
			do{
				m_currentParamNum++; // Порядок важен!
			}while((m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY
					|| m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_STRING_OUT)
					&& m_currentParamNum < m_lastSelectableParam);
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(!m_paramsList[m_currentParamNum]->inverse()) m_paramsList[m_currentParamNum]->increaseParam();
		else m_paramsList[m_currentParamNum]->decreaseParam();

		m_paramsList[m_currentParamNum]->setToDsp();

		// Support CustomParam
		if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_LIST)
			printPage();	// whole page to show "disabled" param changes
		else
			m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
	}

	clean_flag();
}

void ParamListMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_currentParamNum > m_firstSelectableParam)
		{
			do{
				m_currentParamNum--; // Порядок важен!
			}while((m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY
					|| m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_STRING_OUT)
					&& m_currentParamNum > m_firstSelectableParam);
			printPage();
			tim5_start(0);
		}
	}
	else
	{
		if(!m_paramsList[m_currentParamNum]->inverse()) m_paramsList[m_currentParamNum]->decreaseParam();
				else m_paramsList[m_currentParamNum]->increaseParam();


		m_paramsList[m_currentParamNum]->setToDsp();

		if(m_paramsList[m_currentParamNum]->type() == BaseParam::GUI_PARAMETER_LIST)
			printPage();	// whole page to show "disabled" param changes
		else
			m_paramsList[m_currentParamNum]->printParam(m_currentParamNum % paramsOnPage);
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

void ParamListMenu::printPage(bool forceDrawIcon)
{
	int8_t newPageNumber;
	if(m_currentParamNum > 0) newPageNumber = m_currentParamNum / paramsOnPage;
	else newPageNumber = 0;

	if((newPageNumber != m_currentPageNumber || forceDrawIcon) && m_drawIcon)
	{
		strelka_t drawStrelka;

		if(newPageNumber < m_pagesCount - 1) drawStrelka = STRELKA_DOWN;
		if(newPageNumber > 0 && newPageNumber < m_pagesCount) drawStrelka = STRELKA_UPDOWN;
		if(newPageNumber == m_pagesCount - 1) drawStrelka = STRELKA_UP;
		if(m_pagesCount == 1) drawStrelka = STRELKA_NONE;

		DisplayTask->Clear();
		DisplayTask->Icon_Strel(iconFormMenuType(m_menuType), drawStrelka);

	}
	m_currentPageNumber = newPageNumber;

	uint8_t stringCount = m_paramsCount - m_currentPageNumber * paramsOnPage;

	for(uint8_t i = 0; i < min(stringCount, (uint8_t)4); i++)
	{
		uint8_t displayParamNum = i + m_currentPageNumber * paramsOnPage;

		bool highlight = (m_currentParamNum == displayParamNum) && (m_pagesCount > 1);

		if(m_paramsList[displayParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY
			|| m_paramsList[displayParamNum]->type() == BaseParam::GUI_PARAMETER_STRING_OUT) continue;

		m_paramsList[displayParamNum]->printParam(i);
		DisplayTask->StringOut(leftPad, i, TDisplayTask::fntSystem , 2 * highlight, (uint8_t*)(m_paramsList[displayParamNum]->name()));
	}
}

