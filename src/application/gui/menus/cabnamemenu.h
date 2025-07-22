#ifndef SRC_APPLICATION_GUI_MENUS_CABNAMEMENU_H_
#define SRC_APPLICATION_GUI_MENUS_CABNAMEMENU_H_

#include "abstractmenu.h"

class CabNameMenu: public AbstractMenu
{
public:
	CabNameMenu(AbstractMenu* topLevelMenu);

	void task() override;

	void encoderPressed() override;
	void keyUp() override;

	void show(TShowMode showMode = FirstShow) override;
private:
	uint8_t m_currentShowCab{0};
	uint8_t m_delay{0};
};

#endif /* SRC_APPLICATION_GUI_MENUS_CABNAMEMENU_H_ */
