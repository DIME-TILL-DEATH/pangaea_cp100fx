#ifndef CUSTOMPARAM_H_
#define CUSTOMPARAM_H_

#include "baseparam.h"

class CustomParam: public BaseParam
{
public:

	enum TDisplayType
	{
		Number,
		String,
		Custom
	};

	CustomParam(TDisplayType displayType, const char* name, void* paramValuePtr);
	~CustomParam();

	uint32_t value() const override;
	void increaseParam() override;
	void decreaseParam() override;
	void printParam(uint8_t yDisplayPosition) override;

	void setStrings(std::initializer_list<const char*> stringList, uint8_t maxStringLength);

	void (*increaseCallback)(void *valuePtr){nullptr};
	void (*decreaseCallback)(void *valuePtr){nullptr};
	uint32_t (*valueCallback)(void *valuePtr){nullptr};

	void (*encoderPressCallback)(void* parameter){nullptr};
	void (*keyDownCallback)(void* parameter){nullptr};
	void (*printCallback)(void* parameter){nullptr};

private:
	TDisplayType m_displayType;

	char** m_strings;
	uint8_t valuesTable;
	uint8_t m_stringCount;
	uint8_t m_maxStringLength;
};

#endif /* CUSTOMPARAM_H_ */
