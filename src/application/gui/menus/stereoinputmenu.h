#ifndef _STEREOINPUTMENU_H_
#define _STEREOINPUTMENU_H_

#include "abstractmenu.h"

class StereoInputMenu: public AbstractMenu
{
public:
	StereoInputMenu(AbstractMenu* parent);

	void show(TShowMode showMode = FirstShow) override;

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;
};

#endif /* _STEREOINPUTMENU_H_ */
