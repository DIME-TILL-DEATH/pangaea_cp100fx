#include "preset.h"

Preset::TPreset __CCM_BSS__ currentPreset;

Preset::TCabinet __CCM_BSS__ cab1;
Preset::TCabinet __CCM_BSS__ cab2;

uint8_t Preset::impulsePath[512];

uint8_t __CCM_BSS__ ccmBuffer[4096 * 3 * 2];

uint8_t __CCM_BSS__ preset_temp[38560];

uint16_t __CCM_BSS__ delay_time;

volatile uint8_t __CCM_BSS__ currentPresetNumber;
volatile uint8_t __CCM_BSS__ preselectedPresetNumber;
