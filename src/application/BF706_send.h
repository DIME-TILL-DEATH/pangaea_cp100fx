#ifndef BF706_SEND_H_
#define BF706_SEND_H_

#include "appdefs.h"
#include "modules.h"

enum dsp_module_address_t
{
	DSP_ADDRESS_PRESET_FULL = 0,
	DSP_ADDRESS_PRESET_VOLUME,
	DSP_ADDRESS_CAB_DATA_PRIMARY,
	DSP_ADDRESS_CAB,
	DSP_ADDRESS_MODULES_ENABLE,
	DSP_ADDRESS_CHORUS,
	DSP_ADDRESS_PHASER,
	DSP_ADDRESS_REVERB,
	DSP_ADDRESS_DELAY,
	DSP_ADDRESS_EQ,
	DSP_ADDRESS_TREMOLO,
	DSP_ADDRESS_AMP,
	DSP_ADDRESS_PROGRAMM_CHANGE,
	DSP_ADDRESS_FLANGER,
	DSP_ADDRESS_TUN_PROC,
	DSP_ADDRESS_IND_SRC,
	DSP_ADDRESS_CAB_DRY_MUTE,
	DSP_ADDRESS_EARLY_REFLECTIONS,
	DSP_ADDRESS_CAB_DATA_SECONDARY,
	DSP_ADDRESS_MASTER = 19,
	DSP_ADDRESS_EQ_BAND = 22,
	DSP_ADDRESS_AMP_TYPE,
	DSP_ADDRESS_DSP_RUN,
	DSP_ADDRESS_SPDIF,
	DSP_ADDRESS_PREAMP,
	DSP_ADDRESS_CAB_CONFIG,
	DSP_ADDRESS_GATE,
	DSP_ADDRESS_COMPRESSOR,
	DSP_ADDRESS_RESONANCE_FILTER,
	DSP_ADDRESS_MUTE,
	DSP_ADDRESS_GLOBAL_TEMPO
};

enum dsp_indicator_source_t
{
	DSP_INDICATOR_OUT = 0,
	DSP_INDICATOR_CAB1,
	DSP_INDICATOR_AMP,
	DSP_INDICATOR_CAB2,
	DSP_INDICATOR_FLANGER,
	DSP_INDICATOR_CHORUS,
	DSP_INDICATOR_TREMOLO,
	DSP_INDICATOR_PHASER,
	DSP_INDICATOR_PREAMP,
	DSP_INDICATOR_IN
};

extern uint32_t send_buf;
extern uint8_t spi_stat;
extern uint8_t __CCM_BSS__ preset_temp[];

void DSP_GuiSendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value);
void DSP_ContrSendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value);

void DSP_SendPresetData(uint8_t* data);
void DSP_SendPrimaryCabData(uint8_t* data, uint8_t presetNum = 0);
void DSP_SendSecondaryCabData(uint8_t* data, uint8_t presetNum = 0); // 0 - currentPreset

void DSP_ErasePrimaryCab(uint8_t presetNum);
void DSP_EraseSecondaryCab(uint8_t presetNum);

void send_cab_data(uint8_t val, uint8_t num, uint8_t menu_fl);
void send_cab_data1(uint8_t val, uint8_t num);
void gui_send(uint8_t num, uint16_t val);
void contr_send(uint8_t num, uint16_t val);
void ext_send(uint8_t data);

#endif /* BF706_SEND_H_ */
