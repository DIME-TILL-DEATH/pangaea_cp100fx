#include "submenuparam.h"

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name, AbstractMenu* menu, void* param)
				:BaseParam(paramType, name, param)
{
	m_menu = menu;
}

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name,
		AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), void* param)
				:BaseParam(paramType, name, param)
{
	m_submenuCreationFunction = submenuCreationFunction;
}

void SubmenuParam::showSubmenu()
{
	if(m_menu) m_menu->show();
}

void SubmenuParam::showSubmenu(AbstractMenu* parent)
{
	if(m_submenuCreationFunction)
	{
		AbstractMenu* newSubMenu = m_submenuCreationFunction(parent);
		currentMenu = newSubMenu;
		newSubMenu->show();
	}
}
