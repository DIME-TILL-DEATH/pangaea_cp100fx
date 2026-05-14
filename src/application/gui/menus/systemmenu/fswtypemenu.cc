#include "fswtypemenu.h"
#include "fswmodemenu.h"

#include "footswitch.h"
#include "syssettings_handlers.h"

FswTypeMenu::FswTypeMenu(AbstractMenu* parent)
		:ParamListMenu(parent, MENU_FSW_TYPE)
{
}

AbstractMenu* FswTypeMenu::create(AbstractMenu* parent)
{
	FswTypeMenu* fswTypeMenu = new FswTypeMenu(parent);

	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	params[0] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "FSW-DOWN", &FswTypeMenu::createFswDownMenu, fswTypeMenu);
	params[1] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "FSW-CONFIRM", &FswTypeMenu::createFswConfirmMenu, fswTypeMenu);
	params[2] = new SubmenuParam(BaseParam::GUI_PARAMETER_SUBMENU, "FSW-UP", &FswTypeMenu::createFswUpMenu, fswTypeMenu);
	params[3] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, &SysSettingsDesc.fswSpeed);

	fswTypeMenu->setParams(params, paramNum);
	fswTypeMenu->setIcon(false, ICON_NONE);
	return fswTypeMenu;
}

AbstractMenu* FswTypeMenu::createFswDownMenu(AbstractMenu* parent)
{
	return new FswModeMenu(parent, Footswitch::FswButton::DOWN);
}

AbstractMenu* FswTypeMenu::createFswConfirmMenu(AbstractMenu* parent)
{
	return new FswModeMenu(parent, Footswitch::FswButton::CONFIRM);
}

AbstractMenu* FswTypeMenu::createFswUpMenu(AbstractMenu* parent)
{
	return new FswModeMenu(parent, Footswitch::FswButton::UP);
}
