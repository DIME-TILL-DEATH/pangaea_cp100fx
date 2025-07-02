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
	enum TExpMenuState
	{
		ParamChoice,
		ParamTuning,
		CalMin,
		CalMax
	};

	TExpMenuState m_menuState{ParamChoice};

	uint8_t m_parNum{0};

	static constexpr uint8_t strOk[] = "Ok";
	static constexpr uint8_t strSetMin[] = "Set to min";
	static constexpr uint8_t strSetMmax[] = "Set to max";
	static constexpr uint8_t strExprType[][12] = {"   Off     ", "Standard V ", "Alternat V ", "Standard CC", "Alternat CC"};

};

#endif /* SRC_APPLICATION_GUI_MENUS_EXPRESSIONMENU_H_ */
