#ifndef _FOOTSWITCH_H_
#define _FOOTSWITCH_H_

#include "appdefs.h"

namespace Footswitch
{
enum FswButton{
	DOWN = 0,
	CONFIRM,
	UP
};

enum FswMode
{
	Single,
	Double
};

enum FswModeName
{
	onPress = 0,
	onHold
};

enum FswType
{
	Default,
	Controller,
	Tuner,
	PresetMap1,
	PresetMap2,
	PresetMap3,
	PresetMap4
};

const uint8_t expr_menu_str[][10] = {"Type", "Calibrate", "CC#", "Store Lev"};

extern const char* fswCommandString;
extern const char* fswModeStringHandler;
extern const char* fswPressTypeStringHandler;
extern const char* fswHoldTypeStringHandler;
extern const char* fswPressCcStringHandler;
extern const char* fswHoldCcStringHandler;
extern const char* fswPressMapStringHandler;
extern const char* fswHoldMapStringHandler;

void press_execute(uint8_t num);
void hold_execute(uint8_t num);

void setMode(FswButton button, FswMode mode);

void setPressType(Footswitch::FswButton button, Footswitch::FswType type);
void setHoldType(Footswitch::FswButton button, Footswitch::FswType type);

void setPressCc(Footswitch::FswButton button, uint8_t ccValue);
void setHoldCc(Footswitch::FswButton button, uint8_t ccValue);

void setPressMap(Footswitch::FswButton button, uint8_t mapNum, uint8_t presetNum);
void setHoldMap(Footswitch::FswButton button, uint8_t mapNum, uint8_t presetNum);
}
#endif /* _FOOTSWITCH_H_ */
