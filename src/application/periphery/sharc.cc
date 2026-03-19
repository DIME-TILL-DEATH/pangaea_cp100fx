#include "sharc.h"

#include "periphery.h"
#include "serial.h"

#include "eepr.h"
#include "system.h"
#include "modules.h"

#include "par_bitmap.h"

void SHARC_SpiInit(TSharcSpiMode mode)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	SPI_Cmd(SPI2, DISABLE);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_StructInit(&SPI_InitStructure);
	SPI_I2S_DeInit (SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	if(mode == TSharcSpiMode::SPI_SLAVE)
	{
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	}
	else
	{
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	}
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);

	HW_Delay(0xffffff); // 0x1ffffff start, 0x3ffffff end
}

void SHARC_StartupLoad()
{
	SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Reset);
	SHARC_SendData(0);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);	//reset pin

	for(size_t i = 0; i<3; i++)
		SHARC_SendData(0);

	uint32_t binarySize;
	uint8_t* binaryStart;
#ifdef __STEREO_MOD__
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_start;
	extern uint8_t _binary_Pangaea_CP100FX_stereo_ldr_end;

	binaryStart = &_binary_Pangaea_CP100FX_stereo_ldr_start;
	binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
#endif

#ifdef __MONO_MOD__
	if(cab_type != CAB_CONFIG_STEREO)
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_ldr_end;

		binaryStart = &_binary_Pangaea_CP100FX_1_ldr_start;
		binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_ldr_end-&_binary_Pangaea_CP100FX_1_ldr_start);
	}
	else
	{
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_start;
		extern uint8_t _binary_Pangaea_CP100FX_1_duble_fir_ldr_end;

		binaryStart = &_binary_Pangaea_CP100FX_1_duble_fir_ldr_start;
		binarySize = (size_t)(&_binary_Pangaea_CP100FX_1_duble_fir_ldr_end-&_binary_Pangaea_CP100FX_1_duble_fir_ldr_start);
	}
#endif
	for(uint32_t i = 0; i<binarySize; i++)
		SHARC_SendData(binaryStart[i]);
}

void SHARC_WaitForReady()
{
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);
}

void SHARC_SendData(uint16_t data)
{
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, data);
}

void SHARC_LoadAllData()
{
	uint32_t buf;
	uint8_t loadProgress = 0;

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	for(uint8_t i = 0; i<99; i++)
	{
		if(EEPROM_LoadPreset(i))
		{
			SHARC_WaitForReady();
			SHARC_SendData(i);
			SHARC_SendData(i);

			for(uint32_t ii = 0; ii<4096; ii++)	//CAB_DATA_SIZE / 3
			{
				buf = currentPreset.cab1Data[ii*3]<<8;
				buf |= currentPreset.cab1Data[ii*3+1]<<16;
				buf |= currentPreset.cab1Data[ii*3+2]<<24;

				SHARC_WaitForReady();
				SHARC_SendData(buf>>16);
				SHARC_SendData(buf);
			}

			for(uint32_t ii = 0; ii<4096; ii++) //CAB_DATA_SIZE / 3
			{
				if(cab_type == CAB_CONFIG_STEREO)
				{
					buf = currentPreset.cab2Data[ii*3]<<8;
					buf |= currentPreset.cab2Data[ii*3+1]<<16;
					buf |= currentPreset.cab2Data[ii*3+2]<<24;
				}
				else
				{
					buf = currentPreset.cabAuxData[ii*3]<<8;
					buf |= currentPreset.cabAuxData[ii*3+1]<<16;
					buf |= currentPreset.cabAuxData[ii*3+2]<<24;
				}

				SHARC_WaitForReady();
				SHARC_SendData(buf>>16);
				SHARC_SendData(buf);
			}

			for(uint32_t ii = 0; ii<512; ii++) //sizeof(Preset::TModulesData)
			{
				buf = currentPreset.modulesBuf[ii];

				SHARC_WaitForReady();
				SHARC_SendData(buf>>16);
				SHARC_SendData(buf);
			}
		}

		loadProgress++;
		progressBar((168/2-50), 3, loadProgress);
	}

	SHARC_WaitForReady();
	SHARC_SendData(100);
	SHARC_SendData(100);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}
