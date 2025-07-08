#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"

enum
{
	CAB_SIM_DISABLED,
	MIDI_CHANNEL,
	CAB_SIM_CONFIG,
	EXPRESSION_TYPE,

	FSW1_PRESS_TYPE, FSW2_PRESS_TYPE, FSW3_PRESS_TYPE,

	/*calibration*/
	EXPR_CAL_MIN_HI, EXPR_CAL_MIN_LO, EXPR_CAL_MAX_HI, EXPR_CAL_MAX_LO,

	/*preset map to foot press*/
	FSW1_PRESS_PR1, FSW1_PRESS_PR2, FSW1_PRESS_PR3, FSW1_PRESS_PR4,
	FSW2_PRESS_PR1, FSW2_PRESS_PR2, FSW2_PRESS_PR3, FSW2_PRESS_PR4,
	FSW3_PRESS_PR1, FSW3_PRESS_PR2, FSW3_PRESS_PR3, FSW3_PRESS_PR4,

	SPDIF_OUT_TYPE,

	/*num CC to express and foot*/
	EXP_CCN,
	FSW1_CTRL_PRESS_CC, FSW2_CTRL_PRESS_CC, FSW3_CTRL_PRESS_CC,

	MIDI_PC_IND, /*service*/

	LAST_PRESET_NUM = 31,

	/*FSW button mode*/
	FSW1_MODE = 32, FSW2_MODE, FSW3_MODE,

	FSW1_HOLD_TYPE, FSW2_HOLD_TYPE, FSW3_HOLD_TYPE,

	/*preset map to foot hold*/
	FSW1_HOLD_PR1, FSW1_HOLD_PR2, FSW1_HOLD_PR3, FSW1_HOLD_PR4,
	FSW2_HOLD_PR1, FSW2_HOLD_PR2, FSW2_HOLD_PR3, FSW2_HOLD_PR4,
	FSW3_HOLD_PR1, FSW3_HOLD_PR2, FSW3_HOLD_PR3, FSW3_HOLD_PR4,

	FSW1_CTRL_HOLD_CC, FSW2_CTRL_HOLD_CC, FSW3_CTRL_HOLD_CC,

	TAP_TYPE,
	TUNER_EXTERNAL,
	/*speed foot*/FSW_SPEED,
	TIME_FORMAT,	// Sec, BPM
	TAP_HIGH, /*global temp hi*/
	STORE_EXP_LEVEL, /*save expression level*/
	SWAP_SWITCH,
	TUNER_SPEED,

	MASTER_EQ_ON = 120,
	MASTER_EQ_LOW = 121,
	MASTER_EQ_MID = 122,
	MASTER_EQ_HIGH = 124,

	PHONES_VOLUME = 125,
	MASTER_VOLUME,
	ATTENUATOR,

	MIDI_MAP_START,

	MASTER_EQ_FREQ_VAL_LO = 508,
	MASTER_EQ_FREQ_VAL_HI,

	MASTER_EQ_FREQ_LO = 510,
	MASTER_EQ_FREQ_HI
};

extern int16_t mstEqMidFreq;

extern uint16_t delay_time;
extern volatile uint8_t currentPresetNumber;
extern volatile uint8_t preselectedPresetNumber;
extern volatile uint32_t flash_adr;
//extern uint8_t currentPreset.modules.rawData[];
extern const uint8_t prog_data_init[];
extern uint8_t preset_temp[];
//extern uint8_t presetName[];
//extern uint8_t presetComment[];
extern const uint8_t nameInit[];
extern uint8_t imya_t[];
extern uint8_t imya1_t[];

extern uint8_t sys_para[];
extern uint8_t imp_buf_uint[];
//extern char cab1.name[];
//extern char cab2.name[];
extern char name_buf_temp[];
extern const uint8_t no_loaded[];
extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;
extern uint8_t cab_num;
extern uint8_t __CCM_BSS__ preset_temp[];

void write_sys(void);
void eepr_write(uint8_t nu);
void EEPROM_loadPreset(uint8_t presetNum);
void EEPROM_loadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData);
void eepr_read_imya(uint8_t nu);
void write_prog_temp(uint8_t nu);
void read_prog_temp(uint8_t nu);
void preset_erase(uint8_t nu);
//void eepr_read_prog_data(uint8_t nu);

#endif
