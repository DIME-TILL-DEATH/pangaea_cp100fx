#include "stringparam.h"

#include "display.h"


StringParam::StringParam(const char* name, uint8_t* paramValuePtr, char** newStrings, uint8_t strCount)
			:BaseParam(BaseParam::GUI_PARAMETER_LIST, name, paramValuePtr)
{
	if(strCount <= 24)
	{
		for(int i=0; i<strCount; i++)
		{
			kgp_sdk_libc::memcpy(strings[i], newStrings[i], 24);
		}

		m_stringCount = strCount;
		m_maxValue = strCount - 1;
	}
}

StringParam::~StringParam()
{
//	for(int i=0; i<m_stringCount; i++)
//	{
//		for(int a=0; a<16; a++)
//		{
//			delete(&strings[a][i]);
//		}
//	}
}

uint8_t* StringParam::getString(uint8_t stringNum)
{
	return (uint8_t*)strings[stringNum];
}

void StringParam::setAffectedParamsList(BaseParam** affectedParamList, uint8_t affectedParamCount)
{
	for(int i=0; i<affectedParamCount; i++)
	{
		m_affectedParamsList[i] = affectedParamList[i];
	}

	m_affectedParamsCount = affectedParamCount;
}

void StringParam::setDisableMask(uint8_t stringNum, std::initializer_list<uint8_t> disableMask)
{
//	kgp_sdk_libc::memcpy(m_disableMask[stringNum], disableMask, 16);
	uint8_t i = 0;
	for (auto val = disableMask.begin(); val != disableMask.end() && i!=16; ++val)
	{
		m_disableMask[stringNum][i] = *val;
		i++;
	}
}

uint8_t* StringParam::getDisableMask(uint8_t stringNum)
{
	return m_disableMask[stringNum];
}

uint8_t* StringParam::getDisableMask()
{
	return m_disableMask[value()];
}

void StringParam::increaseParam()
{
	if(!m_valuePtr) return;

	*m_valuePtr += 1;

	for(int i=0; i<m_affectedParamsCount; i++)
	{
		m_affectedParamsList[i]->setDisabled(m_disableMask[*m_valuePtr][i]);
	}
}

void StringParam::decreaseParam()
{
	if(!m_valuePtr) return;

	*m_valuePtr -= 1;

	for(int i=0; i<m_affectedParamsCount; i++)
	{
		m_affectedParamsList[i]->setDisabled(m_disableMask[*m_valuePtr][i]);
	}
}

void StringParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled) return;

	DisplayTask->StringOut(m_xDisplayPosition, yDisplayPosition, TDisplayTask::fntSystem , 0, getString(*m_valuePtr));
}
