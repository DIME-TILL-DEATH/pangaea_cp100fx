#ifndef MASTERVOLUMEMENU_H_
#define MENUS_MASTERVOLUMEMENU_H_

#include "paramlistmenu.h"

class MasterVolumeMenu: public ParamListMenu
{
public:
	MasterVolumeMenu(AbstractMenu* parentMenu);

	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;
};

#endif /* MASTERVOLUMEMENU_H_ */
