#ifndef SRC_APPLICATION_SYSTEM_H_
#define SRC_APPLICATION_SYSTEM_H_

#include "appdefs.h"

namespace System
{
	typedef enum
	{
		TAP_OFF,
		TAP_RFILTER,
		TAP_DELAY,
		TAP_TREMOLO
	}TapDestination;

	void TapTempo(TapDestination tapDst);
};

extern uint32_t tap_global;
extern uint32_t tap_temp;

#endif /* SRC_APPLICATION_SYSTEM_H_ */
