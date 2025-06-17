#ifndef COPYPRESETMENU_H_
#define COPYPRESETMENU_H_

#include "abstractmenu.h"

class CopyMenu : public AbstractMenu
{
public:
	CopyMenu(AbstractMenu* parent);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

private:
	uint8_t copyPresetNum = 0;

	void updatePresetData();
	void copyPreset();
};



#endif /* COPYPRESETMENU_H_ */
