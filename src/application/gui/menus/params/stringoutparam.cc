#include "stringoutparam.h"

StringOutParam::StringOutParam(const char* stringPtr)
	:BaseParam(BaseParam::GUI_PARAMETER_STRING_OUT, stringPtr, nullptr)
{
	m_stringPtr = stringPtr;
}

