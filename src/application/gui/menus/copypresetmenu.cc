#include "copypresetmenu.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "fonts/allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"

extern uint8_t copy_temp;

CopyPresetMenu::CopyPresetMenu(AbstractMenu* parent)
{
	m_menuType = MENU_COPY;
	topLevelMenu = parent;
}

void CopyPresetMenu::show(TShowMode showMode)
{
	current_menu = MENU_COPY;
	currentMenu = this;
	read_prog_temp(copy_temp);
	imya_temp = 1;
	for(uint8_t i = 0 ; i < 15 ; i++)imya_t[i] = preset_temp[i];
	for(uint8_t i = 0 ; i < 15 ; i++)imya1_t[i] = preset_temp[15 + i];

	DisplayTask->Main_scr();
	DisplayTask->Prog_ind(copy_temp);
	DisplayTask->StringOut(10,3,TDisplayTask::fntSystem,0,(uint8_t*)"Copy to ->");

	clean_flag();
}

void CopyPresetMenu::task()
{

}

void CopyPresetMenu::encoderPressed()
{

}

void CopyPresetMenu::encoderClockwise()
{

}

void CopyPresetMenu::encoderCounterClockwise()
{

}

void CopyPresetMenu::keyUp()
{

}

void CopyPresetMenu::keyDown()
{

}

void CopyPresetMenu::key1()
{

}

void CopyPresetMenu::key2()
{

}

void CopyPresetMenu::key3()
{

}

void CopyPresetMenu::key4()
{

}

void CopyPresetMenu::key5()
{

}


