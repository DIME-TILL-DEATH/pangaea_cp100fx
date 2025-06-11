#ifndef SRC_APPLICATION_GUI_STRINGPARAM_H_
#define SRC_APPLICATION_GUI_STRINGPARAM_H_

#include "baseparam.h"

class StringParam : public BaseParam
{
public:
	StringParam(const char* name, uint8_t* paramValuePtr, char** newStrings, uint8_t strCount);
	~StringParam();

	uint8_t* getString(uint8_t stringNum);
private:
	char strings[16][16];
	uint8_t m_stringCount;
};


#endif /* SRC_APPLICATION_GUI_STRINGPARAM_H_ */
