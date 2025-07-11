#ifndef SRC_APPLICATION_PRESET_H_
#define SRC_APPLICATION_PRESET_H_

#include "appdefs.h"
#include "controllers.h"

#define CAB_DATA_SIZE 12288

namespace Preset
{

enum PcOutType
{
	MidiIn,
	Map,
	Set
};

#pragma pack(push, 1)
typedef struct
{
    uint8_t amp;
    uint8_t cab;
    uint8_t eq;
    uint8_t delay;
    uint8_t phaser;
    uint8_t flanger;
    uint8_t chorus;
    uint8_t early_reflections;
    uint8_t reverb;
    uint8_t tremolo;
    uint8_t preamp;
    uint8_t gate;
    uint8_t compressor;
    uint8_t resonance_filter;
}TEnableData;

typedef struct
{
    uint8_t volume;
    uint8_t pan;
    uint8_t delay;
}TCabData;

typedef struct
{
    uint8_t mix;
    uint8_t rate;
    uint8_t width;
    uint8_t center;
    uint8_t feedback;
    uint8_t type;
}TPhaserData;

typedef struct
{
    uint8_t mix;    // volume
    uint8_t rate;
    uint8_t width;
    uint8_t delay;
    uint8_t type;
}TChorusData;

typedef struct
{
    uint8_t volumeFirst;
    uint8_t feedback;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t panFirst;
    uint8_t volumeSecond;
    uint8_t panSecond;
    uint8_t offset; // d - offset?
    uint8_t modulation;
    uint8_t rate;
    uint8_t direction;
}TDelayData;

typedef struct
{
    uint8_t mix;
    uint8_t lfo;
    uint8_t rate;
    uint8_t width;
    uint8_t delay;
    uint8_t feedback;
}TFlangerData;

typedef struct
{
    uint8_t mix;
    uint8_t time;
    uint8_t size;
    uint8_t damping;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t detune;
}TReverbData;

typedef struct
{
    uint8_t depth;  // volume
    uint8_t rate;
    uint8_t lfo;
    uint8_t ms;
}TTremoloData;

typedef struct
{
    uint8_t master;
    uint8_t type;
    uint8_t level;
}TPaData;

typedef struct
{
    uint8_t volume;
    uint8_t type;
}TErData;

typedef struct
{
    uint8_t gain;
    uint8_t volume;
    uint8_t pre_0;
    uint8_t low;
    uint8_t mid;
    uint8_t high;
}TPreampData;

typedef struct
{
    uint8_t threshold;
    uint8_t attack;
    uint8_t decay;
}TGateData;

typedef struct
{
    uint8_t threshold;
    uint8_t ratio;
    uint8_t volume;
    uint8_t attack;
    uint8_t knee;
}TCompressorData;

typedef struct
{
    uint8_t mix;
    uint8_t f_type;
    uint8_t f_mod;
    uint8_t lfo_rate;
    uint8_t lpf;
    uint8_t hpf;
    uint8_t resonance;
    uint8_t dyn_threshold;
    uint8_t dyn_attack;
    uint8_t dyn_release;
    uint8_t volume;
    uint8_t ext;
}TRfData;

typedef struct
{
    TEnableData switches;

    uint8_t dummy1[27];

    uint8_t cab_mono;
    TCabData cab1;
    TCabData cab2;

    uint8_t eq_gain[5];

    uint8_t hpf;
    uint8_t lpf;
    uint8_t presence;

    TPhaserData phaser;
    TChorusData chorus;
    TDelayData delay;
    TFlangerData flanger;
    TReverbData reverb;
    TTremoloData tremolo;

    uint8_t preset_volume;

    TPaData pa;
    TErData early_reflections;

    uint8_t eq_freq[5];
    uint8_t eq_q[5];

    TPreampData preamp;
    TGateData gate;
    TCompressorData compressor;

    uint8_t hpf_phaser;
    uint8_t hpf_flanger;
    uint8_t hpf_chorus;

    uint8_t delay_tap;
    uint8_t tremolo_tap;

    TRfData resonance_filter;

    uint8_t reverb_type;
    uint8_t reverb_diffusion;

    uint8_t resonance_filter_gen_type;

    uint8_t volume_control;

    uint8_t eq_pre_post;

    uint16_t delay_time;

    uint8_t phaser_pre_post;
    uint8_t flanger_pre_post;

    uint8_t reverb_predelay;

    uint8_t foot_ind1[3]; // /*foot ind*/fo1,fo2,fo3
    uint8_t foot_ind2[3]; // /*foot ind*/fo11,fo12,fo13

    uint8_t tremolo_lfo_type;

    uint8_t delay_tail;
    uint8_t reverb_tail;

    uint8_t dummy2[350];

    uint8_t bpm_delay;
}TModulesData;
#pragma pack(pop)

typedef union
{
	TModulesData paramData;
	uint8_t rawData[512];
}UModulesData;

typedef struct
{
	uint8_t data[12288];
	char name[64];
}TCabinet;

typedef struct
{
	uint8_t name[15];
	uint8_t comment[15];

	UModulesData modules;

	Controller::TController controller[controllersCount];

	uint8_t dummyFill[512 - sizeof(Controller::TController) * controllersCount - 2];

	uint8_t pcOut;
	uint8_t set;


	//=============
	//uint8_t number;
}TPreset;

typedef struct
{
	uint8_t name[15];
	uint8_t comment[15];

	TEnableData switches;

	char cab1Name[64];
	char cab2Name[64];
}TPresetBrief;


}

extern Preset::TPreset currentPreset;
extern Preset::TCabinet __CCM_BSS__ cab1;
extern Preset::TCabinet __CCM_BSS__ cab2;

#endif /* SRC_APPLICATION_PRESET_H_ */
