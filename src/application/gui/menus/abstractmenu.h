#ifndef ABSTRACTMENU_H_
#define ABSTRACTMENU_H_

#include "appdefs.h"

#include "paramlistmenu/stringoutparam.h"

enum gui_menu_type
{
	MENU_MAIN,
	MENU_MODULES,
	MENU_CABSIM,
	MENU_CABTYPE,
	MENU_CABBROWSER,
	MENU_PRESET_VOLUME,
	MENU_PRESET_NAME_EDIT,
	MENU_PRESET_SAVE,
	MENU_PRESET_WRITE,
	MENU_TUNER,
	MENU_ATTENUATOR,
	MENU_MASTER_VOLUME,
	MENU_EQ_BAND,
	MENU_MASTER_EQ,
	MENU_CABNAME,
	MENU_EXPRESSION,
	MENU_COPY,
	MENU_ERASE,
	MENU_MIDI_PC,
	MENU_TAP_DELAY,
	MENU_FSW_TYPE,
	MENU_FSW_MODE,
	MENU_FSW_CONTROL,
	MENU_TUNER_EXT,

	// Menus with icons
	MENU_DELAY = 128,
	MENU_TREMOLO,
	MENU_REVERB,
	MENU_EARLY_REFLECTIONS,
	MENU_EQ,
	MENU_CHORUS,

	MENU_CONTROLLERS = 135,
	MENU_PA,
	MENU_PHASER,
	MENU_FLANGER,
	MENU_SYSTEM,
	MENU_PREAMP,
	MENU_COMPRESSOR,
	MENU_GATE,
	MENU_RESONANCE_FILTER,

	MENU_ABSTRACT = 255


};

class AbstractMenu
{
public:
	enum TShowMode
	{
		FirstShow,
		ReturnShow
	};

	enum TReturnMode
	{
		KeepChild,
		DeleteChild
	};

	virtual ~AbstractMenu() {};

	virtual void show(TShowMode swhoMode = FirstShow) {};
	virtual void refresh() {};
	virtual void returnFromChildMenu(TReturnMode returnMode = DeleteChild);
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

	void setTopLevelMenu(AbstractMenu* parent);
	void showChild(AbstractMenu* child);

	void setRunningString(StringOutParam* strings);
	StringOutParam* getRunningString();

protected:
	AbstractMenu* topLevelMenu = nullptr;
	AbstractMenu* shownChildMenu = nullptr;

	gui_menu_type m_menuType{MENU_ABSTRACT};

	constexpr static uint8_t ascii_low[] = " abcdefghijklmnopqrstuvwxyz0123456789!@#$%";
	constexpr static uint8_t ascii_high[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ{}()-+_=<>?*.,/&";

	StringOutParam* m_runningString{nullptr};
//	uint8_t m_runningStringsCount{0};

};

extern AbstractMenu* currentMenu;


#endif /* ABSTRACTMENU_H_ */
