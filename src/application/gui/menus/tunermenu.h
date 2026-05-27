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
	void refresh() override;

	void returnToParent() override;

	void encoderPressed() override;

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

private:
	uint8_t m_parNum{0};

	void exitTunerMenu();
};

#endif /* TUNERMENU_H_ */
