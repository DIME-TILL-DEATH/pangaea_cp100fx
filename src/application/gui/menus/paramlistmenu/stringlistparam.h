#ifndef STRINGPARAM_H_
#define STRINGPARAM_H_

#include "baseparam.h"

class StringListParam : public BaseParam
{
public:
	StringListParam(const char* name, uint8_t* paramValuePtr,
			std::initializer_list<const char*> stringList, uint8_t maxStringLength);
	~StringListParam();

	uint8_t* getString(uint8_t stringNum);

	void setAffectedParamsList(BaseParam** affectedParamList, uint8_t affectedParamCount);
	void setDisableMask(uint8_t stringNum, std::initializer_list<uint8_t> disableMask);
	uint8_t* getDisableMask();

	void increaseParam() override;
	void decreaseParam() override;

	void printParam(uint8_t yPos) override;

private:
	char** m_strings{nullptr};
	uint8_t m_stringCount;
	uint8_t m_maxStringLength;

//	uint8_t m_disableMask[24][16]{0};
	static constexpr uint8_t maxAffectedParamsCount = 16;
	uint8_t** m_disableMask{nullptr};
	BaseParam* m_affectedParamsList[maxAffectedParamsCount];
	uint8_t m_affectedParamsCount{0};
};


#endif /* STRINGPARAM_H_ */
