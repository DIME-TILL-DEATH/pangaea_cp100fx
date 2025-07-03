#ifndef SRC_APPLICATION_PRESET_H_
#define SRC_APPLICATION_PRESET_H_

#include "appdefs.h"
#include "controllers.h"

namespace Preset
{

enum PcOutType
{
	MidiIn,
	Map,
	Set
};

typedef struct
{

}TModulesData;

typedef union
{
	TModulesData paramData;
	uint8_t rawData[512];
}UModulesData;

typedef struct
{
	uint8_t name[15];
	uint8_t comment[15];

	UModulesData modules;

	Controller::TController controller[controllersCount];

	uint8_t dummyFill[512 - sizeof(Controller::TController) * controllersCount - 2];

	uint8_t pcOut;
	uint8_t set;


	uint8_t number;
}TPreset;

}

extern Preset::TPreset currentPreset;

#endif /* SRC_APPLICATION_PRESET_H_ */
