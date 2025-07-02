#ifndef SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWTYPEMENU_H_
#define SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWTYPEMENU_H_

#include "abstractmenu.h"

class FswTypeMenu: public AbstractMenu
{
public:
	FswTypeMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};

	static constexpr uint8_t strFswMenu[][12] = {"FSW-DOWN", "FSW-CONFIRM", "FSW-UP", "Speed FS"};
};

#endif /* SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWTYPEMENU_H_ */
