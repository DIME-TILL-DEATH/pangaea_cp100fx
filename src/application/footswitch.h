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
}
#endif /* SRC_APPLICATION_FOOTSWITCH_H_ */
