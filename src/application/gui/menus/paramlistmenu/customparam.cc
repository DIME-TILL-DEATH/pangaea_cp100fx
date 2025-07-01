#include "../paramlistmenu/customparam.h"

#include "BF706_send.h"
#include "enc.h"
#include "eepr.h"

#include "display.h"

CustomParam::CustomParam(TDisplayType displayType, const char* name, void* paramValuePtr)
	:BaseParam(BaseParam::GUI_PARAMETER_CUSTOM, name, paramValuePtr)
{
	m_displayType = displayType;
}

CustomParam::~CustomParam()
{
	if(m_strings)
	{
		for(int i = 0; i<m_stringCount; i++)
				delete[] m_strings[i];
	}
}

void CustomParam::setStrings(std::initializer_list<const char*> stringList, uint8_t maxStringLength)
{
	if(m_strings)
	{
		for(int i = 0; i<m_stringCount; i++)
				delete[] m_strings[i];
	}

	m_stringCount = stringList.size();
	m_maxStringLength = maxStringLength;

	m_strings = new char*[m_stringCount];
	for(int i = 0; i<m_stringCount; i++)
		m_strings[i] = new char[m_maxStringLength];

	uint8_t strCounter = 0;
	for(auto strIter = stringList.begin(); strIter != stringList.end(); ++strIter)
	{
		kgp_sdk_libc::strcpy(m_strings[strCounter++], *strIter);
	}

	m_maxValue = m_stringCount;
}

void CustomParam::increaseParam()
{
	if(increaseCallback) increaseCallback(m_valuePtr);
}

void CustomParam::decreaseParam()
{
	if(decreaseCallback) decreaseCallback(m_valuePtr);
}

uint32_t CustomParam::value() const
{
	if(valueCallback) return valueCallback(m_valuePtr);
	else return 0;
}

void CustomParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled) return;

	switch(m_displayType)
	{
		case TDisplayType::Number:
		{
			DisplayTask->ParamIndicNum(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		}

		case TDisplayType::String:
		{
			if(*m_valuePtr < m_stringCount)
				DisplayTask->StringOut(m_xDisplayPosition, yDisplayPosition, TDisplayTask::fntSystem , 0, (uint8_t*)m_strings[*m_valuePtr]);
			break;
		}

		case TDisplayType::Custom:
		{
			if(printCallback) printCallback(m_valuePtr);
			break;
		}
	}
}
