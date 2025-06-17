#ifndef STRINGPARAM_H_
#define STRINGPARAM_H_

#include "baseparam.h"

class StringParam : public BaseParam
{
public:
	StringParam(const char* name, uint8_t* paramValuePtr, char** newStrings, uint8_t strCount);
	~StringParam();

	uint8_t* getString(uint8_t stringNum);

	void setAffectedParamsList(BaseParam** affectedParamList, uint8_t affectedParamCount);
	void setDisableMask(uint8_t stringNum, std::initializer_list<uint8_t> disableMask);
	uint8_t* getDisableMask(uint8_t stringNum);
	uint8_t* getDisableMask();

	void increaseParam() override;
	void decreaseParam() override;

private:
	char strings[24][16];
	uint8_t m_stringCount;

	uint8_t m_disableMask[24][16];
	BaseParam* m_affectedParamsList[16];
	uint8_t m_affectedParamsCount{0};
};


#endif /* STRINGPARAM_H_ */
