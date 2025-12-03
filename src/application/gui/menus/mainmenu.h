#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "abstractmenu.h"
#include "modulesmenu.h"

#include "preset.h"

class MainMenu : public AbstractMenu
{
public:
	MainMenu();

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;
	void refresh() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

	void presetUp();
	void presetDown();
	void presetChoose(uint8_t presetNum);
	void presetConfirm();

private:
//	ModulesMenu modulesMenu{this};

	Preset::TPresetBrief m_selectedPresetBrief;
	uint8_t m_preselectedPresetNum;

	constexpr static uint8_t mas_eq_list   [][9] ={"Low","Mid","Mid Freq","High"};
};


#endif
