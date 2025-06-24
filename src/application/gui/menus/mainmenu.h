#ifndef MAINMENU_H_
#define MAINMENU_H_

#include "abstractmenu.h"
#include "modulesmenu.h"


class MainMenu : public AbstractMenu
{
public:
	MainMenu();

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

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

private:
	ModulesMenu modulesMenu{this};

	constexpr static uint8_t master_eq_of[] = {"Master EQ Bypass"};
	constexpr static uint8_t master_eq_on[] = {"EQ on/off press EDIT"};
	constexpr static uint8_t mas_eq_list   [][9] ={"Low","Mid","Mid Freq","High"};
};


#endif
