#ifndef SRC_APPLICATION_GUI_MENUS_MASTEREQMENU_H_
#define SRC_APPLICATION_GUI_MENUS_MASTEREQMENU_H_

#include "paramlistmenu.h"

class MasterEqMenu: public ParamListMenu
{
public:
	MasterEqMenu(AbstractMenu* parentMenu);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;
private:
	static void printMidFreqCallback(void* parameter);
	static void setMidFreqCallback(void* parameter);
};

#endif /* SRC_APPLICATION_GUI_MENUS_MASTEREQMENU_H_ */
