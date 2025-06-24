#ifndef SRC_APPLICATION_GUI_MENUS_ATTENUATORMENU_H_
#define SRC_APPLICATION_GUI_MENUS_ATTENUATORMENU_H_

#include "abstractmenu.h"
#include "paramlistmenu.h"

class AttenuatorMenu: public ParamListMenu
{
public:
	AttenuatorMenu(AbstractMenu* parentMenu);

	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;
};
#endif /* SRC_APPLICATION_GUI_MENUS_ATTENUATORMENU_H_ */

