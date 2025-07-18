#ifndef NAMEEDITMENU_H_
#define NAMEEDITMENU_H_

#include "abstractmenu.h"

class NameEditMenu: public AbstractMenu
{
public:
	NameEditMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
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
	uint8_t keyShift;
	uint8_t symbol;

	uint8_t nameCursorPos;
	uint8_t symbolCursorPos;

	bool encoderKnobPressed;

	constexpr static uint8_t ascii_low[] = " abcdefghijklmnopqrstuvwxyz0123456789!@#$%";
	constexpr static uint8_t ascii_high[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ{}()-+_=<>?*.,/&";

	static constexpr uint8_t chartStringLength = 21;
};

#endif /* NAMEEDITMENU_H_ */
