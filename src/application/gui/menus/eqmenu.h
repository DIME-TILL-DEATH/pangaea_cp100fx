#ifndef EQMENU_H_
#define EQMENU_H_

#include "abstractmenu.h"

class EqMenu : public AbstractMenu
{
public:
	EqMenu(AbstractMenu* parent);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

//	void keyUp() override;
	void keyDown() override;

//	void key1() override();
private:
	uint8_t eq_num;
	bool encoderKnobSelected;
};


#endif /* EQMENU_H_ */
