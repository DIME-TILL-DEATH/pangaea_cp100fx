#include "baseparam.h"

#include "paramlistmenu.h"

#include "display_task.h"
#include "io_task.h"
#include "sharc_task.h"


BaseParam::BaseParam(gui_param_type paramType, TParamDescriptor* paramDescriptor)
{
	m_type = paramType;
	m_descriptor = paramDescriptor;
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

void BaseParam::select(bool& selected)
{
	if(!selected)
	{
		selected = true;
	}
	else
	{
		selected = false;
	}
}

const char* BaseParam::name()
{
	if(!m_descriptor) return "";

	if(m_disabled) return " -- ";
	else
	{
		if(!m_descriptor) return "";
		else return m_descriptor->name;
	}
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
	if(!m_descriptor) return 0;
	else
	{
		uint32_t fullValue = 0;
		kgp_sdk_libc::memcpy(&fullValue, m_descriptor->ptr, m_byteSize);
		return fullValue + m_offset;
	}
}

void BaseParam::setInverse(bool isInverse)
{
	m_inverse = isInverse;
}

void BaseParam::increaseParam()
{
	if(!m_descriptor) return;

	int32_t data = 0;
	if(m_byteSize>1) kgp_sdk_libc::memcpy(&data, m_descriptor->ptr, m_byteSize);
	else data = (int8_t)(*(uint8_t*)(m_descriptor->ptr));

	if(data < m_maxValue)
	{
		if(m_type != GUI_PARAMETER_NUM)
			encoderSpeedIncrease();
		else
			*(uint8_t*)(m_descriptor->ptr) += m_stepSize;
	}
}

void BaseParam::decreaseParam()
{
	if(!m_descriptor) return;

	int32_t data = 0;
	kgp_sdk_libc::memcpy(&data, m_descriptor->ptr, m_byteSize);

	if(data > m_minValue)
	{
		if(m_type != GUI_PARAMETER_NUM)
			encoderSpeedDecrease();
		else
			*(uint8_t*)(m_descriptor->ptr) -= m_stepSize;
	}
}

void BaseParam::setData()
{
	if(!m_descriptor) return;

	if(m_descriptor->setterHandler)
	{
		int32_t data = 0;
		kgp_sdk_libc::memcpy(&data, m_descriptor->ptr, m_byteSize);
		m_descriptor->setterHandler(data);
		return;
	}

	if(m_descriptor->dspPosition == NOT_SEND_POS) return;

	for(uint8_t i=0; i<m_byteSize; i++)
	{
		if(m_descriptor->dspPosition == PARAM_EQUAL_POS)
			SharcTask->setParameter(m_descriptor->dspAddress, *((uint8_t*)m_descriptor->ptr + i) + m_offset, 0);
		else
			SharcTask->setParameter(m_descriptor->dspAddress,	m_descriptor->dspPosition + i,
					*((uint8_t*)m_descriptor->ptr + m_byteSize - 1 - i) + m_offset);
	}
}

void BaseParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled)
	{
		DisplayTask->ClearString(m_xDisplayPosition, yDisplayPosition, Font::fntSystem, 8);
		return;
	}

	// переделать на IndicatorType
	switch(m_type)
	{
		case BaseParam::GUI_PARAMETER_LEVEL:
			DisplayTask->ParamInd(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_MIX:
			DisplayTask->ParamIndMix(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_PAN:
			DisplayTask->ParamIndPan(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_VOLUME:
			DisplayTask->ParamIndNum(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_NUM:
			DisplayTask->ParamIndNum(m_xDisplayPosition, yDisplayPosition, *(uint8_t*)(m_descriptor->ptr) + m_offset);
			break;
		default: break;
	}
}

void BaseParam::encoderSpeedIncrease()
{
	TIM_Cmd(TIM6, DISABLE);

	int32_t data = 0;
	kgp_sdk_libc::memcpy(&data, m_descriptor->ptr, m_byteSize);


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

	kgp_sdk_libc::memcpy(m_descriptor->ptr, &data, m_byteSize);

	TIM_SetCounter(TIM6, 0);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	TIM_Cmd(TIM6, ENABLE);
}

void BaseParam::encoderSpeedDecrease()
{
	if(!m_descriptor) return;

	TIM_Cmd(TIM6, DISABLE);

	int32_t data = 0;
	kgp_sdk_libc::memcpy(&data, m_descriptor->ptr, m_byteSize);


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

	kgp_sdk_libc::memcpy(m_descriptor->ptr, &data, m_byteSize);

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
