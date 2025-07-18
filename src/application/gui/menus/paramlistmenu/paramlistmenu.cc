#include "paramlistmenu.h"

#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "icon_bit.h"
#include "display.h"
#include "enc.h"
#include "cc.h"

#include "stringoutparam.h"

#include "system.h"

ParamListMenu::ParamListMenu(AbstractMenu* parent, gui_menu_type menuType)
{
	topLevelMenu = parent;
	m_menuType = menuType;

	m_icon = iconFormMenuType(m_menuType);
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

void ParamListMenu::setVolumeIndicator(TDisplayTask::TVolIndicatorType volIndicatorType,
		dsp_indicator_source_t indicatorSource, uint8_t* indicatorParPtr)
{
	m_volIndicatorType = volIndicatorType;
	m_indicatorSource = indicatorSource;
	m_indicatorParam_ptr = indicatorParPtr;
}

void ParamListMenu::setIcon(bool drawIcon, icon_t icon)
{
	m_drawIcon = drawIcon;
	m_icon = icon;
}

void ParamListMenu::show(TShowMode showMode)
{
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
	DisplayTask->SetVolIndicator(m_volIndicatorType, m_indicatorSource, m_indicatorParam_ptr);
}

void ParamListMenu::refresh()
{
	printPage();
}

void ParamListMenu::task()
{
	if(m_paramsCount == 1) m_encoderKnobSelected = true;

	if(!m_encoderKnobSelected)
	{
		DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
								blinkFlag_fl * 2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
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
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
									2, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
		else
		{
			m_encoderKnobSelected = false;
			DisplayTask->StringOut(leftPad, m_currentParamNum % paramsOnPage, Font::fntSystem,
									0, (uint8_t*)(m_paramsList[m_currentParamNum]->name()));
		}
	}

	tim5_start(1);
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
}

void ParamListMenu::keyDown()
{
	System::TapTempo(m_tapDst);
	refresh();
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
		DisplayTask->Icon_Strel(m_icon, drawStrelka);

	}
	m_currentPageNumber = newPageNumber;

	uint8_t stringCount = m_paramsCount - m_currentPageNumber * paramsOnPage;

	for(uint8_t i = 0; i < min(stringCount, (uint8_t)4); i++)
	{
		uint8_t displayParamNum = i + m_currentPageNumber * paramsOnPage;

		bool highlight = (m_currentParamNum == displayParamNum) && (m_paramsCount > 1);

		if(m_paramsList[displayParamNum]->type() == BaseParam::GUI_PARAMETER_DUMMY) continue;

		m_paramsList[displayParamNum]->printParam(i);
		DisplayTask->StringOut(leftPad, i, Font::fntSystem , 2 * highlight, (uint8_t*)(m_paramsList[displayParamNum]->name()));
	}
}

