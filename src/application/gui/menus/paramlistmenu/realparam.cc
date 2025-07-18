#include "../paramlistmenu/realparam.h"

#include "display.h"


RealParam::RealParam(const char* name, void* paramValuePtr)
	:BaseParam(BaseParam::GUI_PARAMETER_REAL, name, paramValuePtr)
{
	m_minDisplayValue = m_minValue;
	m_maxDisplayValue = m_maxValue;

	m_k2 = (m_minDisplayValue-m_maxDisplayValue)/(m_minValue-m_maxValue);
	m_k1 = m_minDisplayValue-(m_minValue*m_k2);
	calcDisplayValue();
}

void RealParam::setDisplayPrecision(uint8_t precision)
{
	m_precision = precision;
}

void RealParam::setDisplayBounds(float minDisplayValue, float maxDisplayValue)
{
	m_minDisplayValue = minDisplayValue;
	m_maxDisplayValue = maxDisplayValue;

	m_k2 = (m_minDisplayValue-m_maxDisplayValue)/(m_minValue-m_maxValue);
	m_k1 = m_minDisplayValue-(m_minValue*m_k2);
}

void RealParam::setUnits(const char* units, uint8_t strSize)
{
	kgp_sdk_libc::memcpy(m_unitsName, units, strSize);
}

//void RealParam::increaseParam()
//{
//
//}
//
//void RealParam::decreaseParam()
//{
//
//}

void RealParam::printParam(uint8_t yDisplayPosition)
{
	char string[16];
	kgp_sdk_libc::memset(string, 0, 16);

	calcDisplayValue();

	if(m_precision > 0) ksprintf(string, "%1f %s", m_displayValue, m_unitsName); //strcpy format string
	else
	{
		int32_t integer = round(m_displayValue);
		ksprintf(string, "%d %s", integer, m_unitsName);
	}


	switch(m_indicatorType)
	{
		case TIndicatorType::IndBarTransparent:
		{
			DisplayTask->ParamIndicTransparent(m_xDisplayPosition, yDisplayPosition, ((m_displayValue+abs(m_minDisplayValue))*(127.0f/abs(m_maxDisplayValue-m_minDisplayValue))));
			DisplayTask->Clear_str(m_xDisplayPosition + 40, yDisplayPosition, Font::fntSystem, 8);
			DisplayTask->StringOut(m_xDisplayPosition + 40, yDisplayPosition, Font::fntSystem , 0, (uint8_t*)string);
			break;
		}

		case TIndicatorType::IndNone:
		{
			DisplayTask->Clear_str(m_xDisplayPosition, yDisplayPosition, Font::fntSystem, 8);
			DisplayTask->StringOut(m_xDisplayPosition, yDisplayPosition, Font::fntSystem , 0, (uint8_t*)string);
			break;
		}

		default: break;
	}

}

void RealParam::calcDisplayValue()
{
	int32_t fullValue = 0;

	if(m_byteSize>1)
	{
		kgp_sdk_libc::memcpy(&fullValue, m_valuePtr, m_byteSize);
	}
	else
	{
		fullValue = (int8_t)(*m_valuePtr);
	}

	m_displayValue = m_k1 + fullValue*m_k2;
}
