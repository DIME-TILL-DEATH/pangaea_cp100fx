#include "appdefs.h"
#include "BF706_send.h"
#include "eepr.h"
#include "gui_task.h"

#include "system.h"

#include "init.h"

#include "preset.h"

volatile uint8_t gui_fl;
volatile uint8_t contr_fl;
volatile uint8_t ext_send_fl;

void dsp_send(uint8_t address, uint16_t data)
{
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, data);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, address);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

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

void send_cab_data(uint8_t val, uint8_t presetNum, uint8_t menu_fl)
{

	//presetNum == 0; //current
	uint32_t send_buf;
	uint32_t a;
	extern bool cab_data_ready;
	if(cab_data_ready != true && currentMenu->menuType() != MENU_COPY)
	{
		kgp_sdk_libc::memset(preset_temp, 0, 24576);
		preset_temp[0] = 0xff;
		preset_temp[1] = 0xff;
		preset_temp[2] = 0x7f;
	}


	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, presetNum);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_PRIMARY);

	uint32_t dataSize;

	if(cab_type != 2)
		dataSize = 8192;
	else
		dataSize = 4096;

	for(uint32_t i = 0; i < dataSize; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);
		if(val)
		{
			if(!menu_fl)
			{
				send_buf = preset_temp[i * 3] << 8;
				send_buf |= preset_temp[i * 3 + 1] << 16;
				send_buf |= preset_temp[i * 3 + 2] << 24;
			}
			else
			{
				if(i < 4096)
				{
					uint8_t offset = 15 + 15 + 512 + 512 + 2;

					send_buf = preset_temp[i * 3 + offset] << 8;
					send_buf |= preset_temp[i * 3 + 1 + offset] << 16;
					send_buf |= preset_temp[i * 3 + 2 + offset] << 24;
				}
				else
				{
					a = i - 4096;
					if(cab_type == 2)
					{
						send_buf = preset_temp[a * 3 + 13408] << 8;
						send_buf |= preset_temp[a * 3 + 1 + 13408] << 16;
						send_buf |= preset_temp[a * 3 + 2 + 13408] << 24;
					}
					else
					{
						send_buf = preset_temp[a * 3 + 25760] << 8;
						send_buf |= preset_temp[a * 3 + 1 + 25760] << 16;
						send_buf |= preset_temp[a * 3 + 2 + 25760] << 24;
					}
				}
			}
		}
		else
		{
			if(i < 4096)
			{
				send_buf = cab1.data[i * 3] << 8;
				send_buf |= cab1.data[i * 3 + 1] << 16;
				send_buf |= cab1.data[i * 3 + 2] << 24;
			}
			else
			{
				a = i - 4096;
				send_buf = cab2.data[a * 3] << 8;
				send_buf |= cab2.data[a * 3 + 1] << 16;
				send_buf |= cab2.data[a * 3 + 2] << 24;
			}
		}

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)) {};
		SPI_I2S_SendData(SPI2, send_buf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)) {};
		SPI_I2S_SendData(SPI2, send_buf);
	}

	for(uint32_t i = 0; i < 512; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, 0);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

		if(!menu_fl)
			SPI_I2S_SendData(SPI2, currentPreset.modules.rawData[i]);
		else
			SPI_I2S_SendData(SPI2, preset_temp[i + 30]);
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void send_cab_data1(uint8_t val, uint8_t num)
{
	uint32_t send_buf;
	extern bool cab_data_ready;
	if(cab_data_ready != true && currentMenu->menuType() != MENU_COPY)
	{
		kgp_sdk_libc::memset(preset_temp, 0, 12288);
		preset_temp[0] = 0xff;
		preset_temp[1] = 0xff;
		preset_temp[2] = 0x7f;
	}

	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, num);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_SECONDARY);

	for(int i = 0; i < 4096; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);
		if(val)
		{
			send_buf = preset_temp[i * 3] << 8;
			send_buf |= preset_temp[i * 3 + 1] << 16;
			send_buf |= preset_temp[i * 3 + 2] << 24;
		}
		else
		{
			send_buf = cab2.data[i * 3] << 8;
			send_buf |= cab2.data[i * 3 + 1] << 16;
			send_buf |= cab2.data[i * 3 + 2] << 24;
		}
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf >> 16);
		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE))
		{
		}
		;
		SPI_I2S_SendData(SPI2, send_buf);
	}
	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_SendPresetData(uint8_t* data)
{
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	for(uint32_t i = 0; i < 512; i++)
	{
//		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
//		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, DSP_ADDRESS_PRESET_FULL);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, data[i]);
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_SendPrimaryCabData(uint8_t* data, uint8_t presetNum) // (0, presetNum, 0
{
	if(!data) return;

	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, presetNum);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_PRIMARY);

	uint32_t dataSize;

	if(cab_type != CAB_CONFIG_STEREO)
		dataSize = 8192;
	else
		dataSize = 4096;

	uint32_t sendBuf;
	for(uint32_t i = 0; i < dataSize; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		if(i < 4096)
		{
			sendBuf = data[i * 3] << 8;
			sendBuf |= data[i * 3 + 1] << 16;
			sendBuf |= data[i * 3 + 2] << 24;
		}
		else
		{
			uint32_t a = i - 4096;
			sendBuf = data[a * 3] << 8;
			sendBuf |= data[a * 3 + 1] << 16;
			sendBuf |= data[a * 3 + 2] << 24;
		}

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf);
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	for(uint32_t i = 0; i < 512; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, 0);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

		SPI_I2S_SendData(SPI2, currentPreset.modules.rawData[i]);
	}

	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_SendSecondaryCabData(uint8_t* data, uint8_t presetNum)
{
	if(!data) return;

	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, presetNum);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_SECONDARY);

	uint32_t send_buf;
	for(int i = 0; i < 4096; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		send_buf = data[i * 3] << 8;
		send_buf |= data[i * 3 + 1] << 16;
		send_buf |= data[i * 3 + 2] << 24;


		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, send_buf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, send_buf);
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void DSP_ErasePrimaryCab(uint8_t presetNum)
{
	while(EXTI_GetITStatus(EXTI_Line9) == RESET);
	EXTI_ClearITPendingBit (EXTI_Line9);

	GPIO_ResetBits(GPIOA, GPIO_Pin_1);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, presetNum);

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI2, DSP_ADDRESS_CAB_DATA_PRIMARY);

	uint32_t dataSize;

	if(cab_type != CAB_CONFIG_STEREO)
		dataSize = 8192;
	else
		dataSize = 4096;

	uint32_t sendBuf = 0x7fffff00;
	for(uint32_t i = 0; i < dataSize; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf >> 16);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, sendBuf);

		sendBuf = 0;
	}

	while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

	for(uint32_t i = 0; i < 512; i++)
	{
		while(EXTI_GetITStatus(EXTI_Line9) == RESET);
		EXTI_ClearITPendingBit(EXTI_Line9);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
		SPI_I2S_SendData(SPI2, 0);

		while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));

		SPI_I2S_SendData(SPI2, currentPreset.modules.rawData[i]);
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

void gui_send(uint8_t num, uint16_t val)
{
	while((contr_fl) || (ext_send_fl));
	gui_fl = 1;
	switch(num)
	{
		case 7:
			dsp_send(DSP_ADDRESS_CAB, num | (currentPreset.modules.rawData[vol + num] << 8));
		break;

		break;

		case 14:
			dsp_send(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[0]);
		break;

		case 16:
			send_cab_data(val, 0, 0);
		break;
		case 17:
			send_cab_data1(val, 0);
		break;

		case 22:
			dsp_send(DSP_ADDRESS_EQ_BAND, num | (currentPreset.modules.rawData[f1 + num] << 8));
		break;

		case 25:
		{
			volatile int8_t a = sys_para[121 + num];
			if(num != 2)
			{
				a += 24;
				dsp_send(DSP_ADDRESS_EQ, (num + 8) | a << 8);
			}
			else
			{
				dsp_send(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS | (sys_para[System::MASTER_EQ_FREQ_LO] << 8));
				dsp_send(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS | (sys_para[System::MASTER_EQ_FREQ_HI] << 8));
			}
			break;
		}

	}
	gui_fl = 0;
}

volatile uint8_t master_volume_controller = 127;
void ext_send(uint8_t data)
{
	while((gui_fl) || (contr_fl));
	ext_send_fl = 1;
	dsp_send(21, data); //master_volum_contr(data);
	master_volume_controller = data;
	ext_send_fl = 0;
}

