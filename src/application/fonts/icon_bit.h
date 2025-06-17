#ifndef ICON_BIT_H_
#define ICON_BIT_H_

#include "appdefs.h"
#include "../gui/menus/abstractmenu.h"

enum strelka_t
{
	STRELKA_NONE,
	STRELKA_UP,
	STRELKA_DOWN,
	STRELKA_UPDOWN

};

enum icon_t
{
	ICON_DL,
	ICON_TR,
	ICON_RV,
	ICON_ER,
	ICON_EQ,
	ICON_CH,
	ICON_SC,
	ICON_CC,
	ICON_PA,
	ICON_PH,
	ICON_FL,
	ICON_SY,
	ICON_PR,
	ICON_CM,
	ICON_GT,
	ICON_RF
};

icon_t iconFormMenuType(gui_menu_type menuType);
void icon_print(icon_t num, strelka_t strel);

#endif
