#include "pot.h"

#include "periphery.h"
#include "gpio.h"

#include "system.h"
#include "preset.h"

void pot_send_data(uint32_t dat, uint8_t num)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	HW_Delay(6);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	HW_Delay(6);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	if(num & 2)
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	HW_Delay(6);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	HW_Delay(6);

	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	if(num & 1)
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	HW_Delay(6);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	HW_Delay(6);

	for(uint8_t i = 0; i < 8; i++)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);

		if((dat & 0x80) == 0)
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		else
			GPIO_SetBits(GPIOB, GPIO_Pin_10);
		HW_Delay(10);

		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		HW_Delay(10);
		dat = dat << 1;
	}
}

const uint16_t att_db_num[56] =
{510, 459, 421, 390, 368, 344, 331, 320, 311, 303, 297, 291, 287, 283, 279, 277, 274, 272, 269, 268, 266, 264, 263, 262,
		261, 260, 259, 258, 257, 256, 253, 249, 227, 207, 182, 144, 128, 105, 100, 89, 80, 70, 62, 55, 48, 43, 38, 34,
		30, 27, 24, 22, 19, 17, 15, 13};

void HW_WritePot()
{
	HW_PortBConf(0);
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);

#ifdef __MONO_MOD__

	uint16_t att_val;
	if(sys_para[System::ATTENUATOR_MODE])
	att_val = att_db_num[currentPreset.modules.paramData.attenuator];
	else
	att_val = att_db_num[sys_para[System::ATTENUATOR]];

	uint8_t b;
	if(att_val > 255)
	{
		b = att_val - 255;
		att_val = 0;
	}
	else
	{
		b = 0;
		att_val = 255 - att_val;
	}

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(sys_para[System::PHONES_VOLUME] << 1, 0);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_Delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(att_val,1);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_Delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(sys_para[System::PHONES_VOLUME] << 1, 2);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_Delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(b, 3);
#endif

#ifdef __STEREO_MOD__
	pot_send_data(sys_para[System::PHONES_VOLUME] << 1, 0);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(sys_para[System::MASTER_VOLUME] << 1, 1);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(sys_para[System::PHONES_VOLUME] << 1, 2);
	GPIO_SetBits(GPIOC,GPIO_Pin_6);
	HW_delay(6);

	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	pot_send_data(sys_para[System::MASTER_VOLUME] << 1, 3);
#endif
	GPIO_SetBits(GPIOC, GPIO_Pin_6);
	HW_PortBConf(1);
}
