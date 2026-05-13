#ifndef _ATTENUATORMENU_H_
#define _ATTENUATORMENU_H_

#include "abstractmenu.h"
#include "paramlistmenu.h"

class AttenuatorMenu: public ParamListMenu
{
public:
	AttenuatorMenu(AbstractMenu* parentMenu);

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

private:
	void exitMenu();
};
#endif /* _ATTENUATORMENU_H_ */

