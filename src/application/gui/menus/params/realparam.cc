#include "realparam.h"

#include "display.h"


RealParam::RealParam(const char* name, void* paramValuePtr)
	:BaseParam(BaseParam::GUI_PARAMETER_REAL, name, paramValuePtr)
{
	m_minDisplayValue = m_minValue;
	m_maxDisplayValue = m_maxValue;

	m_k2 = (m_minDisplayValue-m_maxDisplayValue)/(m_minValue-m_maxValue);
	m_k1 = m_minDisplayValue-(m_minValue*m_k2);
	m_displayValue = m_k1 + *m_valuePtr*m_k2;
}

void RealParam::increaseParam()
{

}

void RealParam::decreaseParam()
{

}

void RealParam::printParam(uint8_t yDisplayPosition)
{
//	float fractional, integer;
//
//	before fractional = modf(m_displayValue, integer);

	char string[16];
//	memset(string, 0, 16);
//
//	uint8_t symbolNum = 0;
//
//	if(m_signVisible)
//	{
//		if(m_displayValue>0) string[symbolNum] = plusSymbol;
//		else string[symbolNum];
//		symbolNum++;
//	}

	ksprintf(string, "%*.*f %s", 5, m_precision, m_displayValue, m_unitsName);
	DisplayTask->StringOut(m_xDisplayPosition, yDisplayPosition, TDisplayTask::fntSystem , 0, (uint8_t*)string);
}

void RealParam::calcDisplayValue()
{
	m_displayValue = m_k1 + *m_valuePtr*m_k2;
}
