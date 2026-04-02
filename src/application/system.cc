#include <eeprom.h>
#include "system.h"

#include "sharc.h"

#include "preset.h"

#include "display_task.h"
#include "sharc_task.h"

volatile uint32_t  tap_temp;
uint8_t  System::cab_type = 0;
uint8_t  System::tun_del_val;
int16_t  mstEqMidFreq;
uint8_t __CCM_BSS__ sys_para[512];

float tapCoeffs[6] = {1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 0.5f};

const uint16_t bpm_time[] =
		{2727, 2609, 2500, 2400, 2308, 2222, 2143, 2069, 2000, 1935, 1875, 1818, 1765, 1714, 1667, 1622, 1579, 1538, 1500, 1463,
		1429, 1395, 1364, 1333, 1304, 1277, 1250, 1224, 1200, 1176, 1154, 1132, 1111, 1091, 1071, 1053, 1034, 1017,
		1000, 984, 968, 952, 938, 923, 909, 896, 882, 870, 857, 845, 833, 822, 811, 800, 789, 779, 769, 759, 750, 741,
		732, 723, 714, 706, 698, 690, 682, 674, 667, 659, 652, 645, 638, 632, 625, 619, 612, 606, 600, 594, 588, 583,
		577, 571, 566, 561, 556, 550, 545, 541, 536, 531, 526, 522, 517, 513, 508, 504, 500, 496, 492, 488, 484, 480,
		476, 472, 469, 465, 462, 458, 455, 451, 448, 444, 441, 438, 435, 432, 429, 426, 423, 420, 417, 414, 411, 408,
		405, 403, 400, 397, 395, 392, 390, 387, 385, 382, 380, 377, 375, 373, 370, 368, 366, 364, 361, 359, 357, 355,
		353, 351, 349, 347, 345, 343, 341, 339, 337, 335, 333, 331, 330, 328, 326, 324, 323, 321, 319, 317, 316, 314,
		313, 311, 309, 308, 306, 305, 303, 302, 300, 299, 297, 296, 294, 293, 291, 290, 288, 287, 286, 284, 283, 282,
		280, 279, 278, 276, 275, 274, 273, 271, 270, 269, 268, 267, 265, 264, 263, 262, 261, 260, 259, 258, 256, 255,
		254, 253, 252, 251, 250};

void System::setStartupValues()
{
	DSP_SendParameter(DSP_ADDRESS_CAB_DRY_MUTE, sys_para[System::CAB_SIM_DISABLED], 0);
	DSP_SendParameter(DSP_ADDRESS_SPDIF, sys_para[System::SPDIF_OUT_TYPE], 0);
	DSP_SendParameter(DSP_ADDRESS_CAB_CONFIG, sys_para[System::CAB_SIM_CONFIG], 0); // left cab bypass

	DSP_SendParameter(DSP_ADDRESS_MASTER, sys_para[System::MASTER_VOLUME], 0);

	if(!sys_para[System::MASTER_EQ_FREQ_LO] && !sys_para[System::MASTER_EQ_FREQ_HI])
	{
		mstEqMidFreq = 1000;
	}
	else
	{
		mstEqMidFreq = sys_para[System::MASTER_EQ_FREQ_LO] << 8;
		mstEqMidFreq |= sys_para[System::MASTER_EQ_FREQ_HI];
	}

	DSP_SendParameter(DSP_ADDRESS_EQ, EQ_MASTER_LOW_GAIN_POS, sys_para[System::MASTER_EQ_LOW] + 24);
	DSP_SendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_GAIN_POS, sys_para[System::MASTER_EQ_MID] + 24);
	DSP_SendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_LO]);
	DSP_SendParameter(DSP_ADDRESS_EQ, EQ_MASTER_MID_FREQ_POS, sys_para[System::MASTER_EQ_FREQ_HI]);
	DSP_SendParameter(DSP_ADDRESS_EQ, EQ_MASTER_HIGH_GAIN_POS, sys_para[System::MASTER_EQ_HIGH] + 24);
	DSP_SendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, sys_para[System::MASTER_EQ_ON]);

	DSP_SendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[System::TAP_TYPE], sys_para[System::GLOBAL_TAP_TIME]);
	if(sys_para[TAP_TYPE] != System::TAP_TYPE_PRESET) // global temp On
	{
		System::setMoogTime(sys_para[GLOBAL_TAP_TIME] * 8);
		System::setDelayTime(sys_para[GLOBAL_TAP_TIME] * 8);
		System::setTremoloTime(sys_para[GLOBAL_TAP_TIME] * 8);
	}

	tun_del_val = (127 - sys_para[System::TUNER_SPEED]) * (90.0f / 127.0f) + 10.0f;
	currentPresetNumber = sys_para[System::LAST_PRESET_NUM];
}

void System::setMoogTime(float quarterInterval)
{
	if(currentPreset.modulesBuf[RFILTER_LFO_TYPE] == 0)
	{
		if(Preset::moog_time < 2731)
		{
			Preset::moog_time = round(quarterInterval);
			if(sys_para[TIME_FORMAT] == TIME_FORMAT_SEC)
			{
				SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, Preset::moog_time >> 8);
				SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, Preset::moog_time );
			}
			else
			{
				if(Preset::moog_time < 2728 && Preset::moog_time > 249)
				{
					uint8_t temp = 0;
					while(Preset::moog_time < bpm_time[temp++]);
					Preset::moog_time = bpm_time[temp];

					SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, Preset::moog_time>> 8);
					SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, Preset::moog_time);
				}
			}
		}
	}
}

void System::setDelayTime(float quarterInterval)
{
	currentPreset.delayTime = round((float)(quarterInterval/tapCoeffs[currentPreset.modulesBuf[DELAY_TAP]]));
	if(currentPreset.delayTime < 2731)
	{
		if(sys_para[TIME_FORMAT] == TIME_FORMAT_SEC)
		{
			SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, currentPreset.delayTime >> 8);
			SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, currentPreset.delayTime);
		}
		else
		{
			if(currentPreset.delayTime < 2728 && currentPreset.delayTime > 249)
			{
				uint8_t temp = 0;

				while(currentPreset.delayTime < bpm_time[temp++]);
				currentPreset.delayTime = bpm_time[temp];
				currentPreset.modulesBuf[BPM_DELAY] = 60000 / currentPreset.delayTime;

				SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, currentPreset.delayTime >> 8);
				SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, currentPreset.delayTime);
			}
		}
	}
}

void System::setTremoloTime(float quarterInterval)
{
	Preset::trem_time = round((float)(quarterInterval/tapCoeffs[currentPreset.modulesBuf[TREMOLO_TAP]]));

	if(Preset::trem_time < 2731)
	{
		if(!sys_para[TIME_FORMAT])
		{
			SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, Preset::trem_time >> 8);
			SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, Preset::trem_time);
		}
		else
		{
			if(Preset::trem_time < 2728 && Preset::trem_time > 249)
			{
				uint8_t temp = 0;
				while(Preset::trem_time < bpm_time[temp++]);
				Preset::trem_time = bpm_time[temp];

				SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, Preset::trem_time >> 8);
				SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, Preset::trem_time);
			}
		}
	}
}

void System::TapTempo(TapDestination tapDst)
{
	if(tapDst == TapDestination::TAP_OFF) return;

	if(tap_temp <= 2730 * 48) // 48kHz dma
	{
		float tap_global = tap_temp / 48;

		if(sys_para[TAP_TYPE] != System::TAP_TYPE_PRESET) // global temp On
		{
			sys_para[GLOBAL_TAP_TIME] = round(tap_global/8);

			SharcTask->setParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[GLOBAL_TAP_TIME]);

			System::setMoogTime(tap_global);
			System::setDelayTime(tap_global);
			System::setTremoloTime(tap_global);

			EEPROM_DelayedSaveSystemData();
		}
		else
		{
			switch(tapDst)
			{
				case TAP_RFILTER:
				{
					System::setMoogTime(tap_global);
					break;
				}

				case TAP_DELAY:
				{
					System::setDelayTime(tap_global);
					break;
				}

				case TAP_TREMOLO:
				{
					System::setTremoloTime(tap_global);
					break;
				}
				default: break;
			}
		}
	}
	tap_temp = 0;
}
