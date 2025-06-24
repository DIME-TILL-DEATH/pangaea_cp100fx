#ifndef SRC_APPLICATION_GUI_MENUS_PARAMS_STRINGOUTPARAM_H_
#define SRC_APPLICATION_GUI_MENUS_PARAMS_STRINGOUTPARAM_H_

#include "baseparam.h"

class StringOutParam: public BaseParam
{
public:
	StringOutParam(const char* stringPtr);

	uint32_t value() const override {return 0;};
	void increaseParam() override {};
	void decreaseParam() override {};

	void printParam(uint8_t yPos) override;

private:
	const char* m_stringPtr;
};

#endif /* SRC_APPLICATION_GUI_MENUS_PARAMS_STRINGOUTPARAM_H_ */
