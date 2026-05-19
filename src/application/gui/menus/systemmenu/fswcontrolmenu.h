#ifndef _FSWCONTROLMENU_H_
#define _FSWCONTROLMENU_H_

#include "abstractmenu.h"

#include "footswitch.h"

class FswControlMenu: public AbstractMenu
{
public:
	FswControlMenu(AbstractMenu* parent, Footswitch::FswButton button, Footswitch::FswModeName);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;
	void refresh() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};

	uint8_t* fsType_ptr;
	uint8_t* mapPreset;
	uint8_t* ctrlCC_ptr;

	Footswitch::FswButton m_button;
	Footswitch::FswModeName m_mode;

	char presetMapString[22]{0};

	static constexpr uint8_t presetNumPos[4] = {58, 46, 37, 28};
	static constexpr uint8_t strFswType[][12] = {"Default    ", "Controller ", "Tuner      ", "Preset Map1", "Preset Map2", "Preset Map3", "Preset Map4"};

	void presetNumToString(char *adr, uint16_t val);
	void printPage();
};

#endif /* SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWCONTROLMENU_H_ */
