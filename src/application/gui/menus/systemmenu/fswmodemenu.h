#ifndef _FSWMODEMENU_H_
#define _FSWMODEMENU_H_

#include "abstractmenu.h"

#include "footswitch.h"

class FswModeMenu: public AbstractMenu
{
public:
	FswModeMenu(AbstractMenu* parent, Footswitch::FswButton fswNumber);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void refresh() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};
	Footswitch::FswButton m_fswNumber{Footswitch::DOWN};

	static constexpr uint8_t strMode[] = "Mode";
	static constexpr uint8_t strModeType[][7] = {"Single", "Double"};
	static constexpr uint8_t strReleaseHold[][10] = {"OnRelease", "OnHold"};
};

#endif /* _FSWMODEMENU_H_ */
