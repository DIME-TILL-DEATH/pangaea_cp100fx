#include "submenuparam.h"

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name, AbstractMenu* menu)
				:BaseParam(paramType, name, nullptr)
{
	m_menu = menu;
}

void SubmenuParam::showSubmenu()
{
	m_menu->show();
}
