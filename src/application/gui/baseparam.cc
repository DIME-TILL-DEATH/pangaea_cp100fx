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
