#ifndef SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWCONTROLMENU_H_
#define SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWCONTROLMENU_H_

#include "abstractmenu.h"

typedef struct
{
	uint8_t* fs;
	uint8_t* pr_start;
	uint8_t* k1_cc;
}TFswControls;

class FswControlMenu: public AbstractMenu
{
public:
	FswControlMenu(AbstractMenu* parent, TFswControls& fswControls);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};
	TFswControls m_fswControls;

	char str_temp[22]{0};

	static constexpr uint8_t presetNumPos[4] = {58, 46, 37, 28};
	static constexpr uint8_t strFswType[][12] = {"Default    ", "Controller ", "Tuner      ", "Preset Map1", "Preset Map2", "Preset Map3", "Preset Map4"};

	void strPresetInit(char *adr, uint16_t val);
	void printPage();
};

#endif /* SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWCONTROLMENU_H_ */
