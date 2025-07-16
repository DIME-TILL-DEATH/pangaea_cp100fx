#include "preset.h"

Preset::TPreset __CCM_BSS__ currentPreset;

Preset::TCabinet __CCM_BSS__ cab1;
Preset::TCabinet __CCM_BSS__ cab2;

uint8_t Preset::impulsePath[512];

uint8_t __CCM_BSS__ ccmBuffer[4096 * 3 * 2];
