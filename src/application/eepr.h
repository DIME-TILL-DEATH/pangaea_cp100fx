#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"

void EEPROM_Start();

bool EEPROM_LoadPreset(uint8_t presetNum, Preset::TPresetData* loadPreset);
bool EEPROM_LoadPresetBrief(uint8_t presetNum, Preset::TPresetBrief* presetData);
bool EEPROM_LoadPresetHeader(uint8_t presetNum, Preset::TPresetHeader* presetData);
bool EEPROM_LoadCabData(uint8_t presetNum, uint8_t* buf, uint8_t* cab1Name, uint8_t* cab2Name);

void EEPROM_SaveSystemData(void);
void EEPROM_DelayedSaveSystemData();

void EEPROM_SavePreset(uint8_t presetNum, Preset::TPresetData* savePreset);
void EEPROM_SavePresetHeader(uint8_t numPreset, Preset::TPresetHeader* presetData);
void EEPROM_SaveCabData(uint8_t presetNum, uint8_t* cabData, uint8_t* cab1Name, uint8_t* cab2Name);

void EEPROM_ErasePreset(uint8_t presetNum);

#endif
