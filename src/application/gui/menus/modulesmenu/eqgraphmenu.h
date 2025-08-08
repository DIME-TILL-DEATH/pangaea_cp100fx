#ifndef SRC_APPLICATION_GUI_MENUS_MODULESMENU_EQGRAPHMENU_H_
#define SRC_APPLICATION_GUI_MENUS_MODULESMENU_EQGRAPHMENU_H_

#include "abstractmenu.h"

class EqGraphMenu: public AbstractMenu
{
public:
	EqGraphMenu(AbstractMenu* parent);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

//	void keyUp() override;
	void keyDown() override;

	void key3() override;
private:
	uint8_t paramNum{0};
	uint8_t bandNum{0};
	bool encoderKnobSelected{false};

	bool updateResponse{false};
	void printPage();

	int8_t* eqFreq_ptr[7];
	int8_t minParamVal[7];
	int8_t maxParamVal[7];

	static constexpr uint8_t paramNames[][3] = {"B:", "F:", "G:", "Q:"};
	static constexpr uint8_t bandNames[][4] = {" 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " HP", " LP"};
};

#endif /* SRC_APPLICATION_GUI_MENUS_MODULESMENU_EQGRAPHMENU_H_ */
