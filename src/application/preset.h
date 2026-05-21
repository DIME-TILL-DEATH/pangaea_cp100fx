#ifndef _PRESET_H_
#define _PRESET_H_

#include "appdefs.h"
#include "controller.h"

#include "modules.h"

#define PRESET_NAME_STRING_SIZE 15
#define PRESET_COMMENT_STRING_SIZE 15

#define CAB_NAME_STRING_SIZE 64
#define CAB_DATA_SIZE 4096 * 3	// =12288

#define PRESET_DATA_OFFSET PRESET_NAME_STRING_SIZE + PRESET_COMMENT_STRING_SIZE
#define CAB1_DATA_OFFSET PRESET_DATA_OFFSET + 512*2 + 2
#define CAB1_NAME_OFFSET CAB1_DATA_OFFSET + CAB_DATA_SIZE
#define CAB2_DATA_OFFSET CAB1_NAME_OFFSET + CAB_NAME_STRING_SIZE
#define CAB2_NAME_OFFSET CAB2_DATA_OFFSET + CAB_DATA_SIZE
#define CAB1_AUX_DATA_OFFSET CAB2_NAME_OFFSET + CAB_DATA_SIZE

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

    union{
    	uint16_t delay_time;
    	struct{
    		uint8_t delay_time_lo;
    		uint8_t delay_time_hi;
    	};
    };

    uint8_t phaser_pre_post;
    uint8_t flanger_pre_post;

    uint8_t reverb_predelay;

    uint8_t foot_ind_press[3];
    uint8_t foot_ind_hold[3];

    uint8_t tremolo_lfo_type;

#ifdef __STEREO_MOD__
    uint8_t in_left_en;
	uint8_t in_right_en;
	uint8_t left_pan;
	uint8_t right_pan;
#endif

    uint8_t delay_tail;
    uint8_t reverb_tail;

#ifdef __MONO_MOD__
    uint8_t dummy2[349];
    uint8_t attenuator;
#endif

#ifdef __STEREO_MOD__
    uint8_t dummy2[346];
#endif

    uint8_t bpm;
}TModulesData;

typedef struct
{
	uint8_t name[15];
	uint8_t comment[15];

	TModulesData paramData;

	char cab1Name[64];
	char cab2Name[64];
}TPresetBrief;

typedef struct
{
	uint8_t name[PRESET_NAME_STRING_SIZE];
	uint8_t comment[PRESET_COMMENT_STRING_SIZE];

	TModulesData paramData;

	Controller::TController controller[Controller::controllersCount];

	uint8_t dummyFill[512 - sizeof(Controller::TController) * Controller::controllersCount - 2];

	uint8_t pcOut;
	uint8_t set;

	uint16_t delayTime;
}TPresetHeader;

typedef struct
{

	union{
		TPresetHeader presetHeader;
		struct{
			uint8_t name[PRESET_NAME_STRING_SIZE];
			uint8_t comment[PRESET_COMMENT_STRING_SIZE];

			union
			{
				TModulesData paramData;
				uint8_t modulesBuf[512];
			};

			Controller::TController controller[Controller::controllersCount];

			uint8_t dummyFill[512 - sizeof(Controller::TController) * Controller::controllersCount - 2];

			uint8_t pcOut;
			uint8_t set;

			uint16_t delayTime;
		};
	};

	union{
		uint8_t cabBuf[CAB_DATA_SIZE * 3 + CAB_NAME_STRING_SIZE * 2];
		struct{
			uint8_t cab1Data[CAB_DATA_SIZE];
			uint8_t cab1NameSize;
			uint8_t cab1Name[CAB_NAME_STRING_SIZE - 1];
			uint8_t cab2Data[CAB_DATA_SIZE];
			uint8_t cab2NameSize;
			uint8_t cab2Name[CAB_NAME_STRING_SIZE - 1];
#ifdef __MONO_MOD__
			uint8_t cabAuxData[CAB_DATA_SIZE];
#endif
		};
	};

	uint8_t currentImpulsePath[256];
	uint8_t currentImpulseName[256];
}TPresetData;
#pragma pack(pop)

typedef struct
{
	uint8_t name;
	uint8_t comment;
	uint8_t controllers;
	uint8_t rf;
	uint8_t gt;
	uint8_t cm;
	uint8_t pr;
	uint8_t pa;
	uint8_t ir;
	uint8_t eq;
	uint8_t fl;
	uint8_t ph;
	uint8_t ch;
	uint8_t dl;
	uint8_t er;
	uint8_t rv;
	uint8_t tr;
	uint8_t pv;
	uint8_t att;
}TSelectionMask;

extern const char* nameCommandString;
extern const char* commentCommandString;

extern uint16_t moog_time;
extern uint16_t trem_time;

void Change(uint8_t presetNumber);
void SetDefaultValues(TPresetData* preset);
void SetDefaultValues(TPresetHeader* preset);
void Copy(uint8_t targetPresetNum, const Preset::TSelectionMask& selectionMask);
void Erase();
}

extern volatile uint8_t currentPresetNumber;
extern Preset::TPresetData currentPreset;
extern uint8_t __CCM_BSS__ tempCabBuffer[];
extern uint8_t __CCM_BSS__ tempDataBuffer[];

#endif /* SRC_APPLICATION_PRESET_H_ */
