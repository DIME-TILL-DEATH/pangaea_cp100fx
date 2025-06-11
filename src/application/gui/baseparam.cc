#include "baseparam.h"

BaseParam::BaseParam(gui_param_type paramType, const char* name, uint8_t* paramValuePtr)
{
	m_type = paramType;
	m_name = name;
	m_valuePtr = paramValuePtr;
}

void BaseParam::setDspAddress(dsp_module_address_t moduleAddress, uint8_t bytePosition)
{
	m_moduleAddress = moduleAddress;
	m_bytePosition = bytePosition;
}

void BaseParam::setDisplayPosition(uint8_t xCoord)
{
	m_xDisplayPosition = xCoord;
}

void BaseParam::setScaling(uint8_t stepSize, uint8_t offset)
{
	m_stepSize = stepSize;
	m_offset = offset;
}

void BaseParam::setBounds(uint8_t minBound, uint8_t maxBound)
{
	m_minValue = minBound;
	m_maxValue = maxBound;
}
