#include "tunerextmenu.h"

TunerExtMenu::TunerExtMenu(AbstractMenu* parent)
	:ParamListMenu(parent, MENU_TUNER_EXT)
{
	BaseParam* ccParam = new BaseParam(BaseParam::GUI_PARAMETER_NUM, "    CC#", &sys_para[System::TUNER_EXTERNAL]);
	ccParam->setScaling(1, -128);
	ccParam->setBounds(-128, -1);

	setParams(&ccParam, 1);
	setIcon(false, ICON_NONE);
}

void TunerExtMenu::showInputMidiCC(uint8_t midiCC)
{
	DisplayTask->StringOut(12, 3, Font::fntSystem, 0, (uint8_t*)"Midi in CC#->");
	DisplayTask->ParamIndicNum(100, 3, midiCC);
}
