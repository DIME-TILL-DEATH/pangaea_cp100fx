#ifndef TUNERMENU_H_
#define TUNERMENU_H_

#include "abstractmenu.h"

class TunerMenu: public AbstractMenu
{
public:
	TunerMenu(AbstractMenu *parent);

	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void show(TShowMode showMode = FirstShow) override;

	void returnToParent() override;

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

private:
	void exitTunerMenu();
};

#endif /* TUNERMENU_H_ */
