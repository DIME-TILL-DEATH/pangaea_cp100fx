#ifndef SRC_APPLICATION_GUI_MENUS_PARAMLISTMENU_STRINGOUTPARAM_H_
#define SRC_APPLICATION_GUI_MENUS_PARAMLISTMENU_STRINGOUTPARAM_H_

#include "../paramlistmenu/baseparam.h"

//#include "../abstractmenu.h"

class AbstractMenu;

class StringOutParam: public BaseParam
{
public:
	StringOutParam(const char* stringPtr);

	uint32_t value() const override {return 0;};
	void increaseParam() override {};
	void decreaseParam() override {};

	void printParam(uint8_t yPos) override;

	void setRunning(bool enable, AbstractMenu* owner);

	void task();

private:
	const char* m_stringPtr;

	bool m_running{false};

	uint8_t m_yDisplayPosition{0};

	uint8_t m_symbolOffset{0};

	static constexpr uint8_t bufSize = 32;
	uint8_t m_nameBuffer[bufSize];

	uint32_t m_timeThreshold{150000};
	uint32_t m_timeCounter{0};
};

#endif /* SRC_APPLICATION_GUI_MENUS_PARAMLISTMENU_STRINGOUTPARAM_H_ */
