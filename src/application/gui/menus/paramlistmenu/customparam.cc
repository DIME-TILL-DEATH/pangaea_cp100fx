#include <eeprom.h>
#include "customparam.h"

#include "display_task.h"
#include "io_task.h"


CustomParam::CustomParam(TDisplayType displayType, TParamDescriptor* paramDesc)
	: BaseParam(BaseParam::GUI_PARAMETER_CUSTOM, paramDesc)
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
	if(increaseCallback && m_descriptor) increaseCallback(m_descriptor->ptr);
	else BaseParam::increaseParam();
}

void CustomParam::decreaseParam()
{
	if(decreaseCallback && m_descriptor) decreaseCallback(m_descriptor->ptr);
	else BaseParam::decreaseParam();
}

const char* CustomParam::name()
{
	if(nameCallback && m_descriptor) return nameCallback(m_descriptor->ptr);
	else return BaseParam::name();
}

uint32_t CustomParam::value() const
{
	if(valueCallback && m_descriptor) return valueCallback(m_descriptor->ptr);
	else return 0;
}

void CustomParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled) return;

	switch(m_displayType)
	{
		case TDisplayType::Number:
		{
			DisplayTask->ParamIndNum(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		}

		case TDisplayType::String:
		{
			if(*(uint8_t*)(m_descriptor->ptr) < m_stringCount)
				DisplayTask->StringOut(m_xDisplayPosition, yDisplayPosition, Font::fntSystem , Font::fnsNormal, (uint8_t*)m_strings[*(uint8_t*)(m_descriptor->ptr)]);
			break;
		}

		case TDisplayType::Level:
		{
			DisplayTask->ParamInd(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		}

		case TDisplayType::Mix:
		{
			DisplayTask->ParamIndMix(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		}

		case TDisplayType::Pan:
		{
			DisplayTask->ParamIndPan(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		}

		case TDisplayType::Custom:
		{
			if(printCallback && m_descriptor) printCallback(m_descriptor->ptr);
			break;
		}
	}
}

void CustomParam::setData()
{
	if(setToDspCallback && m_descriptor) return setToDspCallback(m_descriptor->ptr);
	else return BaseParam::setData();
}
