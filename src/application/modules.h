#ifndef MODULES_H_
#define MODULES_H_

#include "appdefs.h"

#include "accessors/resonance_filter_setters.h"
#include "accessors/gate_setters.h"
#include "accessors/compressor_setters.h"
#include "accessors/preamp_setters.h"
#include "accessors/amp_setters.h"
#include "accessors/ir_setters.h"
#include "accessors/eq_setters.h"
#include "accessors/phaser_setters.h"
#include "accessors/flanger_setters.h"
#include "accessors/chorus_setters.h"
#include "accessors/delay_setters.h"
#include "accessors/early_setters.h"
#include "accessors/reverb_setters.h"
#include "accessors/tremolo_setters.h"


enum dsp_enable_pos_t
{
	ENABLE_AMP = 0, ENABLE_CAB, ENABLE_EQ, ENABLE_DELAY, ENABLE_PHASER, ENABLE_FLANGER, ENABLE_CHORUS, ENABLE_EARLY_REFLECTIONS,
	ENABLE_REVERB, ENABLE_TREMOLO, ENABLE_PREAMP, ENABLE_GATE, ENABLE_COMPRESSOR, ENABLE_RESONANCE_FILTER, ENABLE_MASTER_EQ
};

#ifdef __MONO_MOD__
enum
{
	CAB_CONFIG_STEREO = 2
};
#endif

enum
{
	CAB_MONO = 41,
	PRESET_VOLUME = 95,
	PRESET_VOLUME_CONTROL = 145,
	BPM_DELAY = 511
};

#endif /* MODULES_H_ */
