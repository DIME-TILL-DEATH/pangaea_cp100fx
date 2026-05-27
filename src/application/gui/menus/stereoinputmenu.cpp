#include "stereoinputmenu.h"

#include "display_task.h"

StereoInputMenu::StereoInputMenu(AbstractMenu* parent)
	:AbstractMenu()
{
	topLevelMenu = parent;
	m_menuType = MENU_STEREO_INPUT;
}

void StereoInputMenu::show(TShowMode showMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	DisplayTask->StringOut(30, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Input Level");
	DisplayTask->StringOut(15, 2, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Right");
	DisplayTask->StringOut(15, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Left");
	DisplayTask->SetIndicator(TIndicatorType::VOL_INDICATOR_STEREO_IN, DSP_INDICATOR_IN);
}

void StereoInputMenu::keyUp()
{
	topLevelMenu->returnFromChildMenu();
}

void StereoInputMenu::key1()
{
	topLevelMenu->returnFromChildMenu();
}

void StereoInputMenu::key2()
{
	topLevelMenu->key2();
}

void StereoInputMenu::key3()
{
	topLevelMenu->key3();
}

void StereoInputMenu::key4()
{
	topLevelMenu->key4();
}

void StereoInputMenu::key5()
{
	topLevelMenu->key5();
}
