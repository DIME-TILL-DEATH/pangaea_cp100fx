#ifndef EQBANDMENU_H_
#define EQBANDMENU_H_

#include "../abstractmenu.h"

class EqBandMenu: public AbstractMenu
{
public:
	EqBandMenu(AbstractMenu* parent, uint8_t bandNum);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyDown() override;

	void key3() override;

private:
	uint8_t m_bandNum;
	uint8_t m_paramNum{0};

	bool encoderKnobSelected{false};

	static constexpr uint8_t gerz[] = "Hz";
	static constexpr uint8_t eq_p_l[][2] = {"F", "Q"};
	static constexpr uint8_t def_eq_band[][10] = {"F 120Hz ", "F 360Hz ", "F 800Hz ", "F 2000Hz", "F 6000Hz"};
};

#endif /* EQBANDMENU_H_ */
