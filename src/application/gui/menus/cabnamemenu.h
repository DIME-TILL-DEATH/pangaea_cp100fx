#ifndef _CABNAMEMENU_H_
#define _CABNAMEMENU_H_

#include "abstractmenu.h"

class CabNameMenu: public AbstractMenu
{
public:
	CabNameMenu(AbstractMenu* topLevelMenu);

	void task() override;

	void encoderPressed() override;
	void keyUp() override;

	void show(TShowMode showMode = FirstShow) override;

	void refresh() override;
private:
	uint8_t m_currentShowCab{0};
	uint8_t m_delay{0};
};

#endif /* _CABNAMEMENU_H_ */
