#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"

extern const uint8_t prog_data_init[];

void EEPROM_Start();

bool EEPROM_LoadPreset(uint8_t presetNum);
void EEPROM_LoadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData);

void EEPROM_WriteSys(void);
void EEPROM_WritePreset(uint8_t presetNum);

void EEPROM_CopyPreset(uint8_t targetPresetNum, const Preset::TSelectionMask& selectionMask);
void EEPROM_PresetErase(uint8_t presetNum);

#endif
