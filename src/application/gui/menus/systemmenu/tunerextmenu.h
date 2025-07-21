#ifndef SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_TUNEREXTMENU_H_
#define SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_TUNEREXTMENU_H_

#include "paramlistmenu.h"

class TunerExtMenu: public ParamListMenu
{
public:
	TunerExtMenu(AbstractMenu* parent);

	void showInputMidiCC(uint8_t midiCC);
};

#endif /* SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_TUNEREXTMENU_H_ */
