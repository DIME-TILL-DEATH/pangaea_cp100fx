#include "stringparam.h"


StringParam::StringParam(const char* name, uint8_t* paramValuePtr, char** newStrings, uint8_t strCount)
			:BaseParam(BaseParam::GUI_PARAMETER_LIST, name, paramValuePtr)
{
	if(strCount <= 16)
	{
		for(int i=0; i<strCount; i++)
		{
			kgp_sdk_libc::memcpy(strings[i], newStrings[i], 16);
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
