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

void press_execute(uint8_t num);
void hold_execute(uint8_t num);

void setMode(FswButton button, FswMode mode);
}
#endif /* _FOOTSWITCH_H_ */
