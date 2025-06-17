#include "baseparam.h"

#include "enc.h"

BaseParam::BaseParam(gui_param_type paramType, const char* name, void* paramValuePtr)
{
	m_type = paramType;
	m_name = name;
	m_valuePtr = (uint8_t*)paramValuePtr;
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

const char* BaseParam::name()
{
	if(m_disabled) return " -- ";
	else return m_name;
}

uint8_t BaseParam::setByteSize(uint8_t size)
{
	m_byteSize = size;
}

uint8_t BaseParam::value() const
{
	if(!m_valuePtr) return 0;
	else return *m_valuePtr + m_offset;
}

void BaseParam::increaseParam()
{
	if(!m_valuePtr) return;

	if(m_stepSize == 1)
		*m_valuePtr = enc_speed_inc(value(), 0);
	else
		*m_valuePtr += m_stepSize;
}

void BaseParam::decreaseParam()
{
	if(!m_valuePtr) return;

	if(m_stepSize == 1)
		*m_valuePtr = enc_speed_dec(value(), 0);
	else
		*m_valuePtr -= m_stepSize;
}
