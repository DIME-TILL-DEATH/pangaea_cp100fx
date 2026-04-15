#ifndef _TUNEREXTMENU_H_
#define _TUNEREXTMENU_H_

#include "paramlistmenu.h"

class TunerExtMenu: public ParamListMenu
{
public:
	TunerExtMenu(AbstractMenu* parent);

	void showInputMidiCC(uint8_t midiCC);

	void refresh() override;
};

#endif /* _TUNEREXTMENU_H_ */
