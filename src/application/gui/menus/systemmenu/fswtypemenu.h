#ifndef _FSWTYPEMENU_H_
#define _FSWTYPEMENU_H_

#include "paramlistmenu.h"

class FswTypeMenu: public ParamListMenu
{
public:
	FswTypeMenu(AbstractMenu* parent);

	static AbstractMenu* create(AbstractMenu* parent);

private:
	static AbstractMenu* createFswDownMenu(AbstractMenu* parent);
	static AbstractMenu* createFswConfirmMenu(AbstractMenu* parent);
	static AbstractMenu* createFswUpMenu(AbstractMenu* parent);
};

#endif /*  */
