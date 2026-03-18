#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"

extern const uint8_t prog_data_init[];

extern uint8_t sys_para[];
extern uint8_t imp_buf_uint[];

extern int16_t mstEqMidFreq;

void EEPROM_Start();
void EEPROM_WriteSys(void);
void EEPROM_WritePreset(uint8_t nu);
void EEPROM_loadPreset(uint8_t presetNum);
void EEPROM_loadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData);

void EEPROM_WriteTempPreset(uint8_t nu);
void EEPROM_LoadPresetToBuffer(uint8_t presetNum, uint8_t* buffer);
void EEPROM_PresetErase(uint8_t nu);
void EEPROM_LoadAllIr();

#endif
