#include "BF706_send.h"

#include "sharc.h"

#include "eepr.h"
#include "system.h"
#include "preset.h"
#include "ui_task.h"

volatile uint8_t gui_fl;
volatile uint8_t contr_fl;
volatile uint8_t ext_send_fl;

void dsp_send(uint8_t address, uint16_t data)
{
	SHARC_WaitForReady();

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	SHARC_SendData(data);
	SHARC_SendData(address);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

// To task!
void DSP_GuiSendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value)
{
	while((contr_fl) || (ext_send_fl));

	gui_fl = 1;
	dsp_send(module_address, parameter_address | value << 8);
	gui_fl = 0;
}

void DSP_ContrSendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value)
{
	while((gui_fl) || (ext_send_fl));

	contr_fl = 1;
	dsp_send(module_address, parameter_address | value << 8);
	contr_fl = 0;
}


void DSP_ExtSendParameter(dsp_module_address_t module_address, uint8_t parameter_address, uint8_t value)
{
	while((gui_fl) || (contr_fl));
	ext_send_fl = 1;
	dsp_send(module_address, parameter_address | value << 8); //master_volum_contr(data);
	ext_send_fl = 0;
}

void DSP_SendPrimaryData(uint8_t* cabMainData, uint8_t* cabAuxData, uint8_t* modulesData, uint8_t presetNum) // (0, presetNum, 0
{
	if(!modulesData || !cabMainData || !cabAuxData) return;
	SHARC_WaitForReady();

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	SHARC_SendData(presetNum);
	SHARC_SendData(DSP_ADDRESS_CAB_DATA_PRIMARY);


	uint32_t sendBuf;
	for(uint32_t i = 0; i < 4096; i++)
	{
		sendBuf = cabMainData[i * 3] << 8;
		sendBuf |= cabMainData[i * 3 + 1] << 16;
		sendBuf |= cabMainData[i * 3 + 2] << 24;

		SHARC_WaitForReady();
		SHARC_SendData(sendBuf >> 16);
		SHARC_SendData(sendBuf);
	}

	if(cab_type != CAB_CONFIG_STEREO && cabAuxData)
	{
		for(uint32_t i = 0; i < 4096; i++)
		{
			sendBuf = cabAuxData[i * 3] << 8;
			sendBuf |= cabAuxData[i * 3 + 1] << 16;
			sendBuf |= cabAuxData[i * 3 + 2] << 24;

			SHARC_WaitForReady();
			SHARC_SendData(sendBuf >> 16);
			SHARC_SendData(sendBuf);
		}
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	for(uint32_t i = 0; i < 512; i++)
	{
		SHARC_WaitForReady();
		SHARC_SendData(0);
		SHARC_SendData(modulesData[i]);
	}

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_SendPrimaryCabData(uint8_t* cabMainData, uint8_t* cabAuxData, uint8_t presetNum)
{
	DSP_SendPrimaryData(cabMainData, cabAuxData, currentPreset.modulesBuf, presetNum);
}

void DSP_SendSecondaryCabData(uint8_t* data, uint8_t presetNum)
{
	if(!data) return;

	SHARC_WaitForReady();

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	SHARC_SendData(presetNum);
	SHARC_SendData(DSP_ADDRESS_CAB_DATA_SECONDARY);

	uint32_t send_buf;
	for(int i = 0; i < 4096; i++)
	{
		send_buf = data[i * 3] << 8;
		send_buf |= data[i * 3 + 1] << 16;
		send_buf |= data[i * 3 + 2] << 24;

		SHARC_WaitForReady();
		SHARC_SendData(send_buf >> 16);
		SHARC_SendData(send_buf);
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_ErasePrimaryCab(uint8_t presetNum)
{
	SHARC_WaitForReady();

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	SHARC_SendData(presetNum);
	SHARC_SendData(DSP_ADDRESS_CAB_DATA_PRIMARY);

	uint32_t dataSize;

	if(cab_type != CAB_CONFIG_STEREO)
		dataSize = 8192;
	else
		dataSize = 4096;

	uint32_t sendBuf = 0x7fffff00;
	for(uint32_t i = 0; i < dataSize; i++)
	{
		SHARC_WaitForReady();

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf);

		sendBuf = 0;
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	for(uint32_t i = 0; i < 512; i++)
	{
		SHARC_WaitForReady();

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, 0);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

		SPI_I2S_SendData(SPI2, currentPreset.modulesBuf[i]);
	}

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_EraseSecondaryCab(uint8_t presetNum)
{
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, presetNum);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_SECONDARY);

	uint32_t send_buf = 0x7fffff00;
	for(int i = 0; i < 4096; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, send_buf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, send_buf);

		send_buf = 0;
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

