#include "system.h"

#include "eepr.h"

#include "display.h"
#include "gui_task.h"

#include "preset.h"

float tapCoeffs[6] = {1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};

uint32_t del_time = 48000;

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

volatile uint32_t tap_temp;
volatile uint32_t tap_global;

void System::TapTempo(TapDestination tapDst)
{
	if(tapDst == TapDestination::TAP_OFF) return;

	if(tap_temp <= 131070)
	{
		tap_global = tap_temp >> 4;

		if(sys_para[TAP_TYPE] != System::TAP_TYPE_PRESET) // global temp On
		{
			sys_para[TAP_HIGH] = (tap_global/3)>>8;

			DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[TAP_HIGH]);

			moog_time = tap_global/3.0f;
			DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, moog_time >> 8);
			DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, moog_time);

			delay_time = tap_global/3.0f/tap_time_coefs[currentPreset.modules.rawData[DELAY_TAP]];
			if(delay_time<2731)
			{
				DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, delay_time >> 8);
				DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, delay_time);
			}

			trem_time = tap_global/3.0f/tap_time_coefs[currentPreset.modules.rawData[TREMOLO_TAP]];
			if(trem_time<2731)
			{
				DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, trem_time >> 8);
				DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, trem_time);
			}
		}
		else
		{
			switch(tapDst)
			{
				case TAP_RFILTER:
				{
					if(currentPreset.modules.rawData[RFILTER_LFO_TYPE])
					{
						moog_time = tap_global / 48.0f;
						DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, moog_time >> 8);
						DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, moog_time );
					}
					break;
				}

				case TAP_DELAY:
				{
					delay_time = tap_global / 3.0f / tapCoeffs[currentPreset.modules.rawData[DELAY_TAP]];
					if(delay_time < 2731)
					{
						uint8_t temp = 0;
						if(sys_para[TIME_FORMAT] == TIME_FORMAT_SEC)
						{
							DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, delay_time >> 8);
							DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, delay_time);
						}
						else
						{
							if(delay_time < 2728 && delay_time > 249)
							{
								while(delay_time < bpm_time[temp++]);
								delay_time = bpm_time[temp];
								currentPreset.modules.rawData[BPM_DELAY] = 60000 / delay_time;

								DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, delay_time >> 8);
								DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, delay_time);
							}
						}
					}
					break;
				}

				case TAP_TREMOLO:
				{
					trem_time = tap_global / 3.0f / tapCoeffs[currentPreset.modules.rawData[TREMOLO_TAP]];

					if(trem_time < 2731)
					{
						if(!sys_para[TIME_FORMAT])
						{
							DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, trem_time >> 8);
							DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, trem_time);
						}
						else
						{
							if(trem_time < 2728 && trem_time > 249)
							{
								uint8_t temp = 0;
								while(trem_time < bpm_time[temp++]);
								trem_time = bpm_time[temp];

								DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, trem_time >> 8);
								DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, trem_time);
							}
						}
					}
					break;
				}

				default: break;
			}
		}
	}
	tap_temp = 0;
}
