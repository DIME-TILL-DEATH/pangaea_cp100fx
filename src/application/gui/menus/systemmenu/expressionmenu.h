#ifndef SRC_APPLICATION_GUI_MENUS_EXPRESSIONMENU_H_
#define SRC_APPLICATION_GUI_MENUS_EXPRESSIONMENU_H_

#include "../abstractmenu.h"

class ExpressionMenu: public AbstractMenu
{
public:
	ExpressionMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};
};

#endif /* SRC_APPLICATION_GUI_MENUS_EXPRESSIONMENU_H_ */
