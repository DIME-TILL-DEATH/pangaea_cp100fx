#ifndef SHARC_H_
#define SHARC_H_

#include "appdefs.h"

#include "modules.h"

typedef enum{
	SPI_MASTER = 0,
	SPI_SLAVE
}TSharcSpiMode;

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

void SHARC_SpiInit(TSharcSpiMode mode);
void SHARC_StartupLoad();
void SHARC_WaitForReady();
void SHARC_SendData(uint16_t data);
void SHARC_LoadAllData();

void DSP_SendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value = 0);

void DSP_SendPrimaryData(uint8_t* cabMainData, uint8_t* cabAuxData,
		uint8_t* modulesData, uint8_t presetNum = 0);

void DSP_SendCab2Data(uint8_t* data, uint8_t presetNum = 0); // 0 - currentPreset

#endif /* SHARC_H_ */
