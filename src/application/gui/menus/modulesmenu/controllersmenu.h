#ifndef SRC_APPLICATION_GUI_MENUS_MODULESMENU_CONTROLLERSMENU_H_
#define SRC_APPLICATION_GUI_MENUS_MODULESMENU_CONTROLLERSMENU_H_

#include "abstractmenu.h"

class ControllersMenu: public AbstractMenu
{
public:
	ControllersMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};

	uint8_t m_controllerNum{0};
	uint8_t m_controllerDst{0};

	static constexpr uint8_t strControllersMenu[][7] = {"Contr", "Source", "Destin", "MinVal", "MaxVal", "PC Out", "SET"};
	static constexpr uint8_t strOutPcList[][8] = {"MIDI IN", "  MAP  ", "  SET  "};
	static constexpr uint8_t strControllerExt[][12] = {"Expression ", " FSW DOWN  ", "FSW CONFIRM", "  FSW UP   "};

	static constexpr uint8_t dest_tabl[55] =
			{25, 36, 0, 44, 45, 46, 47, 48, //7
			1, 2, 3, 4, 37, 38, 5, 49, //15
			50, 51, 52, 53, 6, 7, 8, 9, //23
			10, 11, 12, 13, 14, 15, 16, 17, //31
			18, 19, 20, 21, 54, 22, 23, 24, //39
			26, 27, 28, 29, 30, 31, 32, 35, //47
			33, 34, 39, 40, 41, 42, 43};   //53

	static constexpr uint8_t dest_tabl_start[55] =
			{2, 8, 9, 10, 11, 14, 20, 21, //7
			22, 23, 24, 25, 26, 27, 28, 29, //15
			30, 31, 32, 33, 34, 35, 36, 37, //23
			38, 0, 39, 40, 41, 42, 43, 44, //31
			45, 47, 48, 46, 1, 12, 13, 49, //39
			50, 51, 52, 53, 54, 3, 4, 5, //47
			6, 7, 15, 16, 17, 18, 19};   //53

	static constexpr uint8_t strMidiDstList[][14] =
			{
			/*0*/"Preamp On Off",
			/*1*/"Amplif On Off", "Amplif Volume", "Amplif Slave ",
			/*4*/"CabSim On Off", "Equal On Off ",
			/*6*/"Delay On Off ", "Delay Volume ", "Delay Fedback", "Delay Tap    ",
			/*10*/"Phaser On Off", "Phaser Volume", "Phaser Rate  ",
			/*13*/"Flanger OnOff", "Flang  Volume", "Flang  Rate  ",
			/*16*/"Chorus On Off", "Chorus Volume", "Chorus Rate  ",
			/*19*/"Reverb On Off", "Reverb Volume", "Reverb Time  ",
			/*22*/"Tremolo OnOff", "Tremolo Inten", "Tremolo Rate ",
			/*25*/"Preset Level ",
			/*26*/"Tremolo TAP  ",
			/*27*/"Compr On Off ", "Compr Thresh ", "Compr Volume ",
			/*30*/"Filt  On Off ", "Filt LFOrate ", "Filt freq    ",
			/*33*/"ER On Off    ", "ER Volume    ",
			/*35*/"Filt LFO TAP ",
			/*36*/"Vol Ct On Off",
			/*37*/"Cab1 Volume  ", "Cab2 Volume  ",
			/*39*/"Gate On Off  ", "Gate Thresh  ",
			/*41*/"HPF frequency", "LPF frequency", "Presence val ",
			/*44*/"Preamp Gain  ", "Preamp Volume", "Preamp Low   ", "Preamp Mid   ", "Preamp High  ",
			/*49*/"Eq Band1 Lev ", "Eq Band2 Lev ", "Eq Band3 Lev ", "Eq Band4 Lev ", "Eq Band5 Lev ",
			/*54*/"Reverb Type  "};
};

#endif /* SRC_APPLICATION_GUI_MENUS_MODULESMENU_CONTROLLERSMENU_H_ */
