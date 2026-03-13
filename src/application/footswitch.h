#ifndef SRC_APPLICATION_FOOTSWITCH_H_
#define SRC_APPLICATION_FOOTSWITCH_H_

#include "appdefs.h"

namespace Footswitch
{
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
}
#endif /* SRC_APPLICATION_FOOTSWITCH_H_ */
