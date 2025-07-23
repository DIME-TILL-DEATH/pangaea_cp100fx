#ifndef SRC_APPLICATION_GUI_MENUS_TAPMENU_H_
#define SRC_APPLICATION_GUI_MENUS_TAPMENU_H_

#include "abstractmenu.h"

#include "system.h"

class TapMenu: public AbstractMenu
{
public:
	TapMenu(AbstractMenu* parent, System::TapDestination tapDst = System::TapDestination::TAP_GLOBAL);

	void show(TShowMode swhoMode = FirstShow) override;
	void refresh() override;
	void task() override;

	void keyUp() override;
	void keyDown() override;

private:
	uint8_t m_delay{4};
	void draw();

	System::TapDestination m_tapDst;
};

#endif /* SRC_APPLICATION_GUI_MENUS_TAPMENU_H_ */
