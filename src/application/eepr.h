#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"

void EEPROM_Start();

bool EEPROM_LoadPreset(uint8_t presetNum, Preset::TPresetData* loadPreset);
void EEPROM_LoadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData);

void EEPROM_WriteSys(void);
void EEPROM_SavePreset(uint8_t presetNum, Preset::TPresetData* savePreset);

void EEPROM_CopyPreset(uint8_t targetPresetNum, const Preset::TSelectionMask& selectionMask);
void EEPROM_PresetErase(uint8_t presetNum);

#endif
