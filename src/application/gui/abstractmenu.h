#ifndef ABSTRACTMENU_H_
#define ABSTRACTMENU_H_

#include "appdefs.h"

typedef enum
{
	MENU_MAIN,
	MENU_MODULES,
	MENU_PA,
	MENU_CABSIM,
	MENU_CABTYPE,
	MENU_CABBROWSER,
	MENU_EQ,
	MENU_DELAY,
	MENU_PHASER,
	MENU_FLANGER,
	MENU_CHORUS,
	MENU_REVERB,
	MENU_TREMOLO,
	MENU_VOLUME,
	MENU_CONTROLLERS,
	MENU_NAME_EDIT,
	MENU_PRESET_SAVE,
	MENU_PRESET_WRITE,
	MENU_TUNER,
	MENU_SYSTEM,
	MENU_EARLY_REFLECTIONS,
	MENU_ATTENUATOR,
	MENU_MASTER_VOLUME,
	MENU_ADC_CALIBRATE,
	MENU_EQ_BAND,
	MENU_MASTER_EQ,
	MENU_CABNAME,
	MENU_FSW,
	MENU_FSW_TYPE,
	MENU_EXPRESSION,
	MENU_SW_SELECT0,
	MENU_COPY,
	MENU_PREAMP,
	MENU_ERASE,
	MENU_GATE,
	MENU_COMPRESSOR,
	MENU_MIDI_PC,
	MENU_TAP_DELAY,
	MENU_SW_MODE,
	MENU_SW_SELECT1,
	MENU_RESONANCE_FILTER,
	MENU_TUNER_EXT,
	MENU_ABSTRACT = 255
}gui_menu_type;

class AbstractMenu
{
public:
	virtual void show() {};
	virtual void task() {};

	virtual void encoderPressed() {};
	virtual void encoderClockwise() {};
	virtual void encoderCounterClockwise() {};

	virtual void keyUp();
	virtual void keyDown() {};

	virtual void key1() {};
	virtual void key2() {};
	virtual void key3() {};
	virtual void key4() {};
	virtual void key5() {};

	gui_menu_type menuType();

protected:
	AbstractMenu* topLevelMenu = nullptr;
	gui_menu_type m_menuType{MENU_ABSTRACT};

	constexpr static uint8_t ascii_low1[] = " abcdefghijklmnopqrst";
	constexpr static uint8_t ascii_low2[] = "uvwxyz0123456789!@#$%";
	constexpr static uint8_t ascii_hig1[] = "ABCDEFGHIJKLMNOPQRSTU";
	constexpr static uint8_t ascii_hig2[] = "VWXYZ{}()-+_=<>?*.,/&";
};

extern AbstractMenu* currentMenu;


#endif /* ABSTRACTMENU_H_ */
