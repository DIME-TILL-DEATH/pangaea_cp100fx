#include "abstractmenu.h"

const uint8_t AbstractMenu::ascii_low1[];
const uint8_t AbstractMenu::ascii_low2[];
const uint8_t AbstractMenu::ascii_hig1[];
const uint8_t AbstractMenu::ascii_hig2[];

gui_menu_type AbstractMenu::menuType()
{
	return m_menuType;
}

void AbstractMenu::keyUp()
{
	if(topLevelMenu)
	{
		currentMenu = topLevelMenu;
		topLevelMenu->show();
	}
}
