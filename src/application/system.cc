#include "system.h"

#include "eepr.h"

#include "display.h"
#include "gui_task.h"

#include "preset.h"

float tapCoeffs[6] = {1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};

uint32_t del_time = 48000;

uint32_t tap_temp;
uint32_t tap_global;

void System::TapTempo(TapDestination tapDst)
{
	if(tapDst == TapDestination::TAP_OFF) return;

	if(tap_temp <= 131070)
	{
		tap_global = tap_temp >> 4;

		if(sys_para[TAP_TYPE]) // global temp On
		{
			sys_para[TAP_HIGH] = (tap_global/3)>>8;

			DSP_GuiSendParameter(DSP_ADDRESS_GLOBAL_TEMPO, sys_para[TAP_TYPE], sys_para[TAP_HIGH]); //	gui_send(33, tap_global/3);

			uint16_t moog_time = tap_global/3.0f;
			DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS,(moog_time >> 8) << 8);
			DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS,(moog_time >> 8));

			delay_time = tap_global/3.0f/tap_time_coefs[currentPreset.modules.rawData[d_tap_t]];
			if(delay_time<2731)
			{
				DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, (delay_time >> 8) << 8);
				DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, (delay_time) << 8);
			}

			uint16_t trem_time = tap_global/3.0f/tap_time_coefs[currentPreset.modules.rawData[t_tap_t]];
			if(trem_time<2731)
			{
				DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, (trem_time >> 8) << 8);
				DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, (trem_time << 8));
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
						uint16_t moog_time = tap_global / 48.0f;
						DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS,(moog_time >> 8) << 8);
						DSP_GuiSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS,(moog_time >> 8));
					}
					break;
				}

				case TAP_DELAY:
				{
					delay_time = tap_global / 3.0f / tapCoeffs[currentPreset.modules.rawData[DELAY_TAP]];
					if(delay_time < 2731)
					{
						uint8_t temp = 0;
						if(!sys_para[TIME_FORMAT])
						{
							DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, (delay_time >> 8) << 8);
							DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, (delay_time) << 8);
						}
						else
						{
							if(delay_time < 2728 && delay_time > 249)
							{
								while(delay_time < bpm_time[temp++]);
								delay_time = bpm_time[temp];
								currentPreset.modules.rawData[BPM_GLOBAL] = 60000 / delay_time;

								DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, (delay_time >> 8) << 8);
								DSP_GuiSendParameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, (delay_time) << 8);
							}
						}
					}
					break;
				}

				case TAP_TREMOLO:
				{
					uint16_t trem_time = tap_global / 48.0f / tapCoeffs[currentPreset.modules.rawData[TREMOLO_TAP]];
					if(trem_time < 2731)
					{
						if(!sys_para[TIME_FORMAT])
						{
							DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, (trem_time >> 8) << 8);
							DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, (trem_time << 8));
						}
						else
						{
							if(trem_time < 2728 && trem_time > 249)
							{
								uint8_t temp = 0;
								while(trem_time < bpm_time[temp++]);
								trem_time = bpm_time[temp];

								DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, (trem_time >> 8) << 8);
								DSP_GuiSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, (trem_time << 8));
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
