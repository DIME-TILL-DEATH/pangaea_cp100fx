#ifndef SRC_APPLICATION_GUI_MENUS_MIDIMAPMENU_H_
#define SRC_APPLICATION_GUI_MENUS_MIDIMAPMENU_H_

#include "abstractmenu.h"

class MidiMapMenu: public AbstractMenu
{
public:
	MidiMapMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_pcNum{0};
};

#endif /* SRC_APPLICATION_GUI_MENUS_MIDIMAPMENU_H_ */
