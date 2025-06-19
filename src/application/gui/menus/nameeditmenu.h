#ifndef NAMEEDITMENU_H_
#define SNAMEEDITMENU_H_

#include "abstractmenu.h"

class NameEditMenu: public AbstractMenu
{
public:
	NameEditMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow);
	void task();

	void encoderPressed();
	void encoderClockwise();
	void encoderCounterClockwise();

	void keyUp();
	void keyDown();

	void key1();
	void key2();
	void key3();
	void key4();
	void key5();

private:
	uint8_t keyShift;
	uint8_t symbol;

	uint8_t nameCursorPos;
	uint8_t symbolCursorPos;

	bool encoderKnobPressed;

	static constexpr uint8_t chartStringLength = 21;
};

#endif /* NAMEEDITMENU_H_ */
