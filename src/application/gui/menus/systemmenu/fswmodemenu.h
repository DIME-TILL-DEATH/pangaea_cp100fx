#ifndef SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWMODEMENU_H_
#define SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWMODEMENU_H_

#include "abstractmenu.h"

#include "footswitch.h"

class FswModeMenu: public AbstractMenu
{
public:
	FswModeMenu(AbstractMenu* parent, uint8_t fswNumber);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};
	uint8_t m_fswNumber{0};

	static constexpr uint8_t strMode[] = "Mode";
	static constexpr uint8_t strModeType[][7] = {"Single", "Double"};
	static constexpr uint8_t strReleaseHold[][10] = {"OnRelease", "OnHold"};
};

#endif /* SRC_APPLICATION_GUI_MENUS_SYSTEMMENU_FSWMODEMENU_H_ */
