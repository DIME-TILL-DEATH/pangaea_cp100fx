#include "baseparam.h"

#include "BF706_send.h"
#include "enc.h"
#include "eepr.h"

#include "display.h"

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

void BaseParam::setScaling(uint8_t stepSize, int32_t offset)
{
	m_stepSize = stepSize;
	m_offset = offset;
}

void BaseParam::setBounds(int32_t minBound, int32_t maxBound)
{
	m_minValue = minBound;
	m_maxValue = maxBound;
}

const char* BaseParam::name()
{
	if(m_disabled) return " -- ";
	else return m_name;
}

void BaseParam::setByteSize(uint8_t size)
{
	m_byteSize = size;
}

void BaseParam::setIndicatorType(TIndicatorType indicatorType)
{
	m_indicatorType = indicatorType;
}

uint32_t BaseParam::value() const
{
	if(!m_valuePtr) return 0;
	else
	{
		uint32_t fullValue = 0;
		kgp_sdk_libc::memcpy(&fullValue, m_valuePtr, m_byteSize);
		return fullValue + m_offset;
	}
}

void BaseParam::setInverse(bool isInverse)
{
	m_inverse = isInverse;
}

void BaseParam::increaseParam()
{
	if(!m_valuePtr) return;

	int32_t data = 0;
	if(m_byteSize>1) kgp_sdk_libc::memcpy(&data, m_valuePtr, m_byteSize);
	else data = (int8_t)(*m_valuePtr);

	if(data < m_maxValue)
	{
		if(m_type != GUI_PARAMETER_NUM)
			encoderSpeedIncrease();
		else
			*m_valuePtr += m_stepSize;
	}
}

void BaseParam::decreaseParam()
{
	if(!m_valuePtr) return;

	int32_t data = 0;
	if(m_byteSize>1) kgp_sdk_libc::memcpy(&data, m_valuePtr, m_byteSize);
	else data = (int8_t)(*m_valuePtr);

	if(data > m_minValue)
	{
		if(m_type != GUI_PARAMETER_NUM)
			encoderSpeedDecrease();
		else
			*m_valuePtr -= m_stepSize;
	}
}

void BaseParam::setToDsp()
{
	if(m_bytePosition == NOT_SEND_POS) return;

	for(uint8_t i=0; i<m_byteSize; i++)
	{
		if(m_bytePosition == PARAM_EQUAL_POS)
			DSP_gui_set_parameter(m_moduleAddress, *(m_valuePtr+i) + m_offset, 0);
		else
			DSP_gui_set_parameter(m_moduleAddress,	m_bytePosition + i,
					*(m_valuePtr + m_byteSize - 1 - i) + m_offset);
	}
}

void BaseParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled)
	{
		return;
	}

	// переделать на IndicatorType
	switch(m_type)
	{
		case BaseParam::GUI_PARAMETER_LEVEL:
			DisplayTask->ParamIndic(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_MIX:
			DisplayTask->ParamIndicMix(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_PAN:
			DisplayTask->ParamIndicPan(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_VOLUME:
			DisplayTask->ParamIndicNum(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_NUM:
			DisplayTask->ParamIndicNum(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		default: break;
	}
}

void BaseParam::encoderSpeedIncrease()
{
	TIM_Cmd(TIM6, DISABLE);

	int32_t data = 0;
	if(m_byteSize>1) kgp_sdk_libc::memcpy(&data, m_valuePtr, m_byteSize);
	else data = (int8_t)(*m_valuePtr);

	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update))
	{
		data += 1;
	}
	else
	{
		if(TIM_GetCounter(TIM6) > 0x3fff)
		{
			data += 1;
		}
		else
		{
			uint8_t curStep;

			if(TIM_GetCounter(TIM6) > 0x1fff) curStep = 2 * m_stepSize;
			else
			{
				if(TIM_GetCounter(TIM6) > 0xfff) curStep = 4 * m_stepSize;
				else
				{
					if(TIM_GetCounter(TIM6) > 0x7ff) curStep = 8 * m_stepSize;
					else curStep = 16 * m_stepSize;
				}
			}

			if(data < (m_maxValue - curStep - 1)) data += curStep;
			else data += 1;
		}
	}

	kgp_sdk_libc::memcpy(m_valuePtr, &data, m_byteSize);

	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
}

void BaseParam::encoderSpeedDecrease()
{
	TIM_Cmd(TIM6, DISABLE);

	int32_t data = 0;
	if(m_byteSize>1) kgp_sdk_libc::memcpy(&data, m_valuePtr, m_byteSize);
	else data = (int8_t)(*m_valuePtr);

	if(TIM_GetFlagStatus(TIM6,TIM_FLAG_Update))
	{
		data -= 1;
	}
	else
	{
		if(TIM_GetCounter(TIM6) > 0x3fff)
		{
			data -= 1;
		}
		else
		{
			uint8_t curStep;

			if(TIM_GetCounter(TIM6) > 0x1fff) curStep = 2 * m_stepSize;
			else
			{
				if(TIM_GetCounter(TIM6) > 0xfff) curStep = 4 * m_stepSize;
				else
				{
					if(TIM_GetCounter(TIM6) > 0x7ff) curStep = 8 * m_stepSize;
					else curStep = 16 * m_stepSize;
				}
			}

			if(data > (m_minValue + curStep - 1)) data -= curStep;
			else data -= 1;
		}
	}

	kgp_sdk_libc::memcpy(m_valuePtr, &data, m_byteSize);

	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
}

int16_t BaseParam::encSpeedInc(int16_t data, int16_t max, uint8_t stepSize)
{
	TIM_Cmd(TIM6, DISABLE);

	if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update))
	{
		data += stepSize;
	}
	else
	{
		if(TIM_GetCounter(TIM6) > 0x3fff)
			data += stepSize;
		else
		{
			if(TIM_GetCounter(TIM6) > 0x1fff)
			{
				if(data < (max - 1)) data += 2 * stepSize;
				else data += stepSize;
			}
			else
			{
				if(TIM_GetCounter(TIM6) > 0xfff)
				{
					if(data < (max - 3)) data += 4 * stepSize;
					else data += 1 * stepSize;
				}
				else
				{
					if(TIM_GetCounter(TIM6) > 0x7ff)
					{
						if(data < (max - 7)) data += 8 * stepSize;
						else data += 1 * stepSize;
					}
					else
					{
						if(data < (max - 49)) data += 50 * stepSize;
						else data += 1 * stepSize;
					}
				}
			}
		}
	}

	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
	return data;
}

int16_t BaseParam::encSpeedDec(int16_t data, int16_t min, uint8_t stepSize)
{
	TIM_Cmd(TIM6, DISABLE);

	if(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update))
	{
		data -= stepSize;
	}
	else
	{
		if(TIM_GetCounter(TIM6) > 0x3fff)
			data -= stepSize;
		else
		{
			if(TIM_GetCounter(TIM6) > 0x1fff)
			{
				if(data > (min + 1)) data -= 2 * stepSize;
				else data -= stepSize;
			}
			else
			{
				if(TIM_GetCounter(TIM6) > 0xfff)
				{
					if(data > (min + 3)) data -= 4 * stepSize;
					else data -= stepSize;
				}
				else
				{
					if(TIM_GetCounter(TIM6) > 0x7ff)
					{
						if(data > (min + 7)) data -= 8 * stepSize;
						else data -= stepSize;
					}
					else
					{
						if(data > (min + 49)) data -= 50 * stepSize;
						else data -= stepSize;
					}
				}
			}
		}
	}

	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
	return data;
}
