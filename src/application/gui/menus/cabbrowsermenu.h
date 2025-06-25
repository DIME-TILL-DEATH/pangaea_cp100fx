#ifndef SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_
#define SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_

#include "abstractmenu.h"

class CabBrowserMenu: public AbstractMenu
{
public:
	CabBrowserMenu(AbstractMenu *parent);

	void show(TShowMode showMode = FirstShow) override;

	void encoderPressed() override;
	void encoderCounterClockwise() override;
	void encoderClockwise() override;

	void keyUp() override;

};

#endif /* SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_ */
