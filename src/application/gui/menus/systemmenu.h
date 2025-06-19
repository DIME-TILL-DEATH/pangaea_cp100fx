#ifndef SYSTEMMENU_H_
#define SYSTEMMENU_H_

#include "paramlistmenu.h"

class SystemMenu: public ParamListMenu
{
public:
	SystemMenu(AbstractMenu* parent, gui_menu_type menuType);

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyDown() override;
};

#endif /* SYSTEMMENU_H_ */
