#ifndef COPYPRESETMENU_H_
#define COPYPRESETMENU_H_

#include "abstractmenu.h"

class CopyPresetMenu : public AbstractMenu
{
public:
	CopyPresetMenu(AbstractMenu* parent);

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

};



#endif /* COPYPRESETMENU_H_ */
