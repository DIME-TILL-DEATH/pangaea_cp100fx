#ifndef SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_
#define SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_

#include "../abstractmenu.h"

class CabBrowserMenu: public AbstractMenu
{
public:
	CabBrowserMenu(AbstractMenu *parent, uint8_t cabNumber);

	void show(TShowMode showMode = FirstShow) override;

	void encoderPressed() override;
	void encoderCounterClockwise() override;
	void encoderClockwise() override;

	void keyUp() override;

private:
	uint8_t m_cabNumber{0};

};

#endif /* SRC_APPLICATION_GUI_MENUS_CABBROWSERMENU_H_ */
