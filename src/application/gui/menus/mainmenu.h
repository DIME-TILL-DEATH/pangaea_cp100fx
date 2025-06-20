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

	constexpr static uint8_t att_db [][4] = {" +4"," +3"," +2"," +1","  0"," -1"," -2"," -3"," -4"," -5"," -6"," -7"," -8"," -9","-10","-11",
		"-12","-13","-14","-15","-16","-17","-18","-19","-20","-21","-23","-24","-27","-30","-34","-35","-36","-37",
		"-38","-40","-41","-42","-43","-44","-45","-46","-47","-48","-49","-50","-51","-52","-53","-54","-55","-56",
		"-57","-58","-59","-60"};

	constexpr static char atten[] = {"Attenuator"};
	constexpr static uint8_t decib[] = "dB";


	constexpr static uint8_t master_eq_of[] = {"Master EQ Bypass"};
	constexpr static uint8_t master_eq_on[] = {"EQ on/off press EDIT"};
	constexpr static uint8_t mas_eq_list   [][9] ={"Low","Mid","Mid Freq","High"};
	constexpr static uint8_t master_vo[][14] = {"Master Volume","Phones Volume"};
};


#endif
