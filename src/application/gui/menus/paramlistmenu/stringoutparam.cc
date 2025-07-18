#include "stringoutparam.h"

#include "abstractmenu.h"
#include "display.h"

StringOutParam::StringOutParam(const char* stringPtr)
	:BaseParam(BaseParam::GUI_PARAMETER_STRING_OUT, stringPtr, nullptr)
{
	m_stringPtr = stringPtr;

	m_name = "";

//	kgp_sdk_libc::memset(m_nameBuffer, 0, bufSize);

	m_timeCounter = m_timeThreshold;
}

void StringOutParam::setRunning(bool enable, AbstractMenu* owner)
{
//	AbstractMenu* ownerMenu = static_cast<AbstractMenu*>(owner);

	if(enable)
	{
		m_running = true;
		owner->setRunningString(this);
	}
	else
	{
		m_running = false;
		owner->setRunningString(nullptr);
	}
}

void StringOutParam::printParam(uint8_t yDisplayPosition)
{
	m_yDisplayPosition = yDisplayPosition;

	if(!m_running) DisplayTask->StringOut(m_xDisplayPosition, m_yDisplayPosition, Font::fntSystem, 0, (uint8_t*)m_stringPtr);
}

void StringOutParam::task()
{
	if(m_timeCounter++ >= m_timeThreshold)
	{
		uint8_t strLength = m_stringPtr[0];

		m_timeThreshold = 150000;
		m_symbolOffset++;

		uint8_t nameBuffer[32];
		kgp_sdk_libc::memset(nameBuffer, 0, 32);

		const char* fullString = m_stringPtr;
		if(strLength == 0)
		{
			fullString = " No IR loaded";
			strLength = 13;
		}


		for(uint8_t i = 0; i<20; i++)
		{
			nameBuffer[i] = fullString[m_symbolOffset+i];

			if((m_symbolOffset+i+1) == strLength)
			{
				m_symbolOffset = 0;
				nameBuffer[i+1] = 0;
				m_timeThreshold = 1500000;
				break;
			}
		}

		DisplayTask->StringOut(m_xDisplayPosition, m_yDisplayPosition, Font::fntSystem, 0, (uint8_t*)nameBuffer);

		m_timeCounter = 0;
	}
}
