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

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

	void refresh() override;

	void showInputMidiCC(uint8_t midiCC);

private:
	enum ControllerMenuParams
	{
		Controller,
		Source,
		Destination,
		MinValue,
		MaxValue,
		PcOut,
		Set
	};

	bool m_encoderKnobSelected{false};
	uint8_t m_parNum{0};

	uint8_t currentPageNum{0};

	uint8_t m_controllerNum{0};
	uint8_t m_controllerDstVisual{0};

	static constexpr uint8_t paramsCount = 7;

	static constexpr uint8_t strControllersMenu[][7] = {"Contr", "Source", "Destin", "MinVal", "MaxVal", "PC Out", "SET"};
	static constexpr uint8_t strOutPcList[][8] = {"MIDI IN", "  MAP  ", "  SET  "};
	static constexpr uint8_t strControllerExt[][12] = {"Expression ", " FSW DOWN  ", "FSW CONFIRM", "  FSW UP   "};

	static constexpr uint8_t dst_visual_to_model[55] =
	{
			Controller::Dst::PresetLevel, Controller::Dst::VolCtrlOnOff,
			Controller::Dst::PreampOnOff, Controller::Dst::PreampGain,  Controller::Dst::PreampVolume,
			Controller::Dst::PreampLow, Controller::Dst::PreampMid, Controller::Dst::PreampHigh,
			Controller::Dst::AmpOnOff, Controller::Dst::AmpVolume, Controller::Dst::AmpSlave,
			Controller::Dst::CabSimOnOff,
			Controller::Dst::Cab1Volume, Controller::Dst::Cab2Volume,
			Controller::Dst::EqualOnOff,
			Controller::Dst::EqBand1Lev, Controller::Dst::EqBand2Lev, Controller::Dst::EqBand3Lev,
			Controller::Dst::EqBand4Lev, Controller::Dst::EqBand5Lev,
			Controller::Dst::DelayOnOff, Controller::Dst::DelayMix, Controller::Dst::DelayFeedback, Controller::Dst::DelayTap, //23
			Controller::Dst::PhaserOnOff, Controller::Dst::PhaserMix, Controller::Dst::PhaserRate,
			Controller::Dst::FlangerOnOff, Controller::Dst::FlangerMix, Controller::Dst::FlangerRate,
			Controller::Dst::ChorusOnOff, Controller::Dst::ChorusMix, Controller::Dst::ChorusRate,
			Controller::Dst::ReverbOnOff, Controller::Dst::ReverbMix, Controller::Dst::ReverbTime, Controller::Dst::ReverbType,
			Controller::Dst::TremoloOnOff, Controller::Dst::TremoloIntensity, Controller::Dst::TremoloRate, Controller::Dst::TremoloTap,
			Controller::Dst::CompressorOnOff, Controller::Dst::CompressorThreshold, Controller::Dst::CompressorVolume,
			Controller::Dst::RfOnOff, Controller::Dst::RfLFOrate, Controller::Dst::RfFreq, Controller::Dst::RfLFOTAP,
			Controller::Dst::EROnOff, Controller::Dst::ERMix,
			Controller::Dst::GateOnOff, Controller::Dst::GateThresh,
			Controller::Dst::HPFfrequency, Controller::Dst::LPFfrequency, Controller::Dst::PresenceVal
	};

	static constexpr uint8_t dst_model_to_visual[55] =
	{
		2, 8, 9, 10, 11, 14, 20, 21, //7
		22, 23, 24, 25, 26, 27, 28, 29, //15
		30, 31, 32, 33, 34, 35, 36, 37, //23
		38, 0, 39, 40, 41, 42, 43, 44, //31
		45, 47, 48, 46, 1, 12, 13, 49, //39
		50, 51, 52, 53, 54, 3, 4, 5, //47
		6, 7, 15, 16, 17, 18, 19
	};   //53

	static constexpr uint8_t strMidiDstList[][14] =
	{
			/*0*/"PR On Off    ",
			/*1*/"PA On Off    ", "PA Master    ", "PA Level     ",
			/*4*/"IR On Off    ", "EQ On Off    ",
			/*6*/"DL On Off    ",  "DL Mix      ", "DL Feedback  ", "DL TAP       ",
			/*10*/"PH On Off    ", "PH Mix      ", "PH Rate      ",
			/*13*/"FL On Off    ", "FL Mix      ", "FL Rate      ",
			/*16*/"CH On Off    ", "CH Mix      ", "CH Rate      ",
			/*19*/"RV On Off    ", "RV Mix      ", "RV Time      ",
			/*22*/"TR On Off    ", "TR Intensity ", "TR Rate      ",
			/*25*/"Preset Level ",
			/*26*/"TR TAP       ",
			/*27*/"CM On Off    ", "CM Threshold ", "CM Volume    ",
			/*30*/"RF On Off    ", "RF LFO rate  ", "RF frequency ",
			/*33*/"ER On Off    ", "ER Mix       ",
			/*35*/"RF LFO TAP   ",
			/*36*/"Vol Ct On Off",
			/*37*/"Cab1 Volume  ", "Cab2 Volume  ",
			/*39*/"GT On Off    ", "GT Threshold ",
			/*41*/"HPF frequency", "LPF frequency", "Presence val ",
			/*44*/"PR Gain      ", "PR Volume    ", "PR Low       ", "PR Mid       ", "PR High      ",
			/*49*/"EQ Band1 Lev ", "EQ Band2 Lev ", "EQ Band3 Lev ", "EQ Band4 Lev ", "EQ Band5 Lev ",
			/*54*/"RV Type      "};
//			/*0*/"Preamp On Off",
//			/*1*/"Amplif On Off", "Amplif Volume", "Amplif Slave ",
//			/*4*/"CabSim On Off", "Equal On Off ",
//			/*6*/"Delay On Off ", "Delay Volume ", "Del. Feedback", "Delay TAP    ",
//			/*10*/"Phaser On Off", "Phaser Volume", "Phaser Rate  ",
//			/*13*/"Flanger OnOff", "Flang  Volume", "Flang  Rate  ",
//			/*16*/"Chorus On Off", "Chorus Volume", "Chorus Rate  ",
//			/*19*/"Reverb On Off", "Reverb Volume", "Reverb Time  ",
//			/*22*/"Tremolo OnOff", "Tremolo Inten", "Tremolo Rate ",
//			/*25*/"Preset Level ",
//			/*26*/"Tremolo TAP  ",
//			/*27*/"Compr On Off ", "Compr Thresh ", "Compr Volume ",
//			/*30*/"Filt  On Off ", "Filt LFOrate ", "Filt freq    ",
//			/*33*/"ER On Off    ", "ER Volume    ",
//			/*35*/"Filt LFO TAP ",
//			/*36*/"Vol Ct On Off",
//			/*37*/"Cab1 Volume  ", "Cab2 Volume  ",
//			/*39*/"Gate On Off  ", "Gate Thresh  ",
//			/*41*/"HPF frequency", "LPF frequency", "Presence val ",
//			/*44*/"Preamp Gain  ", "Preamp Volume", "Preamp Low   ", "Preamp Mid   ", "Preamp High  ",
//			/*49*/"Eq Band1 Lev ", "Eq Band2 Lev ", "Eq Band3 Lev ", "Eq Band4 Lev ", "Eq Band5 Lev ",
//			/*54*/"Reverb Type  "};

	void printSources();
	void printPage(uint8_t pageNum);
};

#endif /* SRC_APPLICATION_GUI_MENUS_MODULESMENU_CONTROLLERSMENU_H_ */
