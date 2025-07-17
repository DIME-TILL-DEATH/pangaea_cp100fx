#ifndef EEPR_H_
#define EEPR_H_

#include "appdefs.h"

#include "preset.h"


//extern volatile uint32_t flash_adr;

extern const uint8_t prog_data_init[];
//extern uint8_t preset_temp[];

//extern const uint8_t nameInit[];
extern uint8_t imya_t[];
extern uint8_t imya1_t[];

extern uint8_t sys_para[];
extern uint8_t imp_buf_uint[];

//extern const uint8_t no_loaded[];
extern volatile uint16_t adc_low;
extern volatile uint16_t adc_high;
extern volatile uint16_t adc_val;
extern volatile float adc_val1;

extern int16_t mstEqMidFreq;

void write_sys(void);
void eepr_write(uint8_t nu);
void EEPROM_loadPreset(uint8_t presetNum);
void EEPROM_loadBriefPreset(uint8_t presetNum, Preset::TPresetBrief* presetData);
//void eepr_read_imya(uint8_t nu);
void write_prog_temp(uint8_t nu);
void read_prog_temp(uint8_t nu);
void preset_erase(uint8_t nu);
//void eepr_read_prog_data(uint8_t nu);

#endif
