#include "stringoutparam.h"

#include "display.h"

StringOutParam::StringOutParam(const char* stringPtr)
	:BaseParam(BaseParam::GUI_PARAMETER_STRING_OUT, stringPtr, nullptr)
{
	m_stringPtr = stringPtr;

	kgp_sdk_libc::memset(m_nameBuffer, 0, 22);
}

void StringOutParam::printParam(uint8_t yDisplayPosition)
{
	m_yDisplayPosition = yDisplayPosition;
}

void StringOutParam::task()
{
	if(m_timeCounter++ >= m_timeThreshold)
	{
		uint8_t a = m_stringPtr[0];

		m_timeThreshold = 150000;
		m_symbolOffset++;

		for(uint8_t i = 0; i<20; i++)
		{
			m_nameBuffer[i] = m_stringPtr[m_symbolOffset+i];

			if((m_symbolOffset+i+1)==a)
			{
				m_symbolOffset = 0;
				m_nameBuffer[i+1] = 0;
				m_timeThreshold = 1500000;
				break;
			}
		}

		DisplayTask->StringOut(m_xDisplayPosition, m_yDisplayPosition, TDisplayTask::fntSystem, 0, (uint8_t*)m_nameBuffer);

		m_timeCounter = 0;
	}
}
