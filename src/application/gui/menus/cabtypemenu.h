#ifndef CABTYPEMENU_H_
#define CABTYPEMENU_H_

#include "abstractmenu.h"

class CabTypeMenu: public AbstractMenu
{
public:
	CabTypeMenu(AbstractMenu* parent);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyDown() override;
private:
	uint8_t m_paramNum{0};

	void select();
};

#endif /* CABTYPEMENU_H_ */
