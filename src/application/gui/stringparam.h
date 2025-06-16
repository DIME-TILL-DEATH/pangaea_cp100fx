#ifndef STRINGPARAM_H_
#define STRINGPARAM_H_

#include "baseparam.h"

class StringParam : public BaseParam
{
public:
	StringParam(const char* name, uint8_t* paramValuePtr, char** newStrings, uint8_t strCount);
	~StringParam();

	uint8_t* getString(uint8_t stringNum);

	void setDisableMask(uint8_t stringNum, std::initializer_list<uint8_t> disableMask);
	uint8_t* getDisableMask(uint8_t stringNum);
	uint8_t* getDisableMask();
private:
	char strings[8][16];
	uint8_t m_stringCount;

	uint8_t m_disableMask[8][16];
};


#endif /* STRINGPARAM_H_ */
