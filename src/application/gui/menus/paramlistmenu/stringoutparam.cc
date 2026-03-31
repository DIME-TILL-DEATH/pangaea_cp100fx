#include "stringoutparam.h"

#include "../../../tasks/display_task.h"
#include "abstractmenu.h"

StringOutParam::StringOutParam(const char* stringPtr)
	:BaseParam(BaseParam::GUI_PARAMETER_STRING_OUT, stringPtr, nullptr)
{
	m_stringPtr = stringPtr;
	m_name = "";
}

void StringOutParam::setRunning(bool enable, AbstractMenu* owner)
{
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

void StringOutParam::resetRunning()
{
	m_symbolOffset = 0;
	task();

//	TIM_SetAutoreload(TIM5, 0xffff);
	TIM_SetCounter(TIM5, 0);
}

void StringOutParam::printParam(uint8_t yDisplayPosition)
{
	m_yDisplayPosition = yDisplayPosition;

	if(!m_running) DisplayTask->StringOut(m_xDisplayPosition, m_yDisplayPosition, Font::fntSystem, Font::fnsNormal, (uint8_t*)m_stringPtr);
}

void StringOutParam::task()
{
	if(!m_stringPtr) return;

	uint8_t strLength = kgp_sdk_libc::strlen(m_stringPtr);

	if(m_symbolOffset == 0) TIM_SetAutoreload(TIM5, 0xffff);
	else TIM_SetAutoreload(TIM5, 0x1fff);

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
		nameBuffer[i] = fullString[m_symbolOffset+i-1];

		if((m_symbolOffset+i) == strLength)
		{
			m_symbolOffset = 0;
			nameBuffer[i+1] = 0;
			break;
		}
	}

	DisplayTask->StringOut(m_xDisplayPosition, m_yDisplayPosition, Font::fntSystem, Font::fnsNormal, (uint8_t*)nameBuffer);
}
