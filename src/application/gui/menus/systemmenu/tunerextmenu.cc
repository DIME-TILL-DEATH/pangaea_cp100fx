#include "tunerextmenu.h"

#include "syssettings_handlers.h"

TunerExtMenu::TunerExtMenu(AbstractMenu* parent)
	:ParamListMenu(parent, MENU_TUNER_EXT)
{
	BaseParam* ccParam = new BaseParam(BaseParam::GUI_PARAMETER_NUM, SysSettingsDesc.tunerCc);
	ccParam->setScaling(1, -128);
	ccParam->setBounds(-128, -1);

	setParams(&ccParam, 1);
	setIcon(false, ICON_NONE);
}

void TunerExtMenu::showInputMidiCC(uint8_t midiCC)
{
	DisplayTask->StringOut(12, 3, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Midi in CC#->");
	DisplayTask->ParamIndNum(100, 3, midiCC);
}

void TunerExtMenu::refresh()
{
	if(*(uint8_t*)SysSettingsDesc.tunerCc.ptr & 0x80)
		printPage();
	else
		returnToParent();
}
