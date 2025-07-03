#include "appdefs.h"
#include "eepr.h"
#include "display.h"
#include "cc.h"
#include "cs.h"
#include "enc.h"
#include "display.h"

#include "BF706_send.h"
#include "midi_send.h"

#include "allFonts.h"
#include "gui_task.h"
#include "modulesmenu.h"

#include "controllers.h"
#include "preset.h"
#include "modules.h"

TCCTask *CCTask;

volatile uint8_t midi_b[3];
uint8_t mid_fl;
uint8_t ext_fl;
uint8_t ext_f_fl;
uint8_t ext_data;
uint8_t ext_sourc = 255;
volatile uint8_t tap_del_fl;
volatile uint8_t tap_trem_fl;
volatile uint8_t tap_moog_fl;

void controllerSetData(uint8_t adr, uint8_t data)
{
	float val = currentPreset.controller[adr].minVal + (currentPreset.controller[adr].maxVal - currentPreset.controller[adr].minVal) * data / 127.0f;

	switch(currentPreset.controller[adr].dst)
	{
		case Controller::Dst::PreampOnOff:
			presetData[ENABLE_PREAMP] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PREAMP, presetData[ENABLE_PREAMP]);
		break;
//-------------------------------------------------PA-------------------------------------------------
		case Controller::Dst::AmpOnOff:
			presetData[ENABLE_AMP] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_AMP, presetData[ENABLE_AMP]);
		break;

		case Controller::Dst::AmpVolume:
			presetData[AMP_MASTER] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_AMP, AMP_MASTER_POS, presetData[AMP_MASTER]);
		break;

		case Controller::Dst::AmpSlave:
			presetData[AMP_LEVEL] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_AMP, AMP_LEVEL_POS, presetData[AMP_LEVEL]);
		break;
//-------------------------------------------------CAB SIM--------------------------------------------
		case Controller::Dst::CabSimOnOff:
			presetData[ENABLE_CAB] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CAB, presetData[ENABLE_CAB]);
		break;
//-------------------------------------------------EQ-------------------------------------------------
		case Controller::Dst::EqualOnOff:
			presetData[ENABLE_EQ] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EQ, presetData[ENABLE_EQ]);
		break;
//-------------------------------------------------Delay----------------------------------------------
		case Controller::Dst::DelayOnOff:
			presetData[ENABLE_DELAY] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_DELAY, presetData[ENABLE_DELAY]);
		break;

		case Controller::Dst::DelayVolume:
			presetData[DELAY_MIX] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_MIX_POS, presetData[DELAY_MIX]);
		break;

		case Controller::Dst::DelayFeedback:
			presetData[DELAY_FEEDBACK] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS, presetData[DELAY_FEEDBACK]);
		break;

		case Controller::Dst::DelayTap:
			if(tap_temp_global() && !sys_para[TAP_TYPE])
			{
				delay_time = tap_global / 3.0f / tap_time_coefs[presetData[d_tap_t]];
				if(delay_time < 2731)
				{
					// Через RefreshMenu
					if(sys_para[TIME_FORMAT])
					{
						uint8_t temp = 0;
						if(delay_time < 2728 && delay_time > 249)
						{
							while(delay_time < bpm_time[temp++]);

							delay_time = bpm_time[temp];
						}
					}
					DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_TIME_LO_POS, delay_time >> 8);
					DSP_contr_set_parameter(DSP_ADDRESS_DELAY, DELAY_TIME_HI_POS, delay_time);

				}
				if(tap_trem_fl)
				{
					trem_time = tap_global / 3.0f / tap_time_coefs[presetData[TREMOLO_TAP]];
					if(trem_time > 2730)
						trem_time = 2730;

					DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, trem_time >> 8);
					DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, trem_time);
				}
				if(tap_moog_fl)
				{
					moog_time = tap_global / 3.0f;
					DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, moog_time >> 8);
					DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, moog_time);
				}
			}
		break;
//-------------------------------------------------Phaser---------------------------------------------
		case Controller::Dst::PhaserOnOff:
			presetData[ENABLE_PHASER] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PHASER, presetData[ENABLE_PHASER]);
		break;

		case Controller::Dst::PhaserVolume:
			presetData[PHASER_MIX] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_MIX_POS, presetData[PHASER_MIX]);
		break;

		case Controller::Dst::PhaserRate:
			presetData[PHASER_RATE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PHASER, PHASER_RATE_POS, presetData[PHASER_RATE]);
		break;
//-------------------------------------------------Flanger--------------------------------------------
		case Controller::Dst::FlangerOnOff:
			presetData[ENABLE_FLANGER] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_FLANGER, presetData[ENABLE_FLANGER]);
		break;

		case Controller::Dst::FlangerVolume:
			presetData[FLANGER_MIX] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_FLANGER, FLANGER_MIX_POS, presetData[FLANGER_MIX]);
		break;

		case Controller::Dst::FlangerRate:
			presetData[FLANGER_RATE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_FLANGER, FLANGER_RATE_POS, presetData[FLANGER_RATE]);
		break;
//-------------------------------------------------Chorus---------------------------------------------
		case Controller::Dst::ChorusOnOff:
			presetData[ENABLE_CHORUS] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CHORUS, presetData[ENABLE_CHORUS]);
		break;

		case Controller::Dst::ChorusVolume:
			presetData[CHORUS_MIX] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_MIX_POS, presetData[CHORUS_MIX]);
		break;

		case Controller::Dst::ChorusRate:
			presetData[CHORUS_RATE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_CHORUS, CHORUS_RATE_POS, presetData[CHORUS_RATE]);
		break;
//-------------------------------------------------Reverb---------------------------------------------
		case Controller::Dst::ReverbOnOff:
			presetData[ENABLE_REVERB] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_REVERB, presetData[ENABLE_REVERB]);
		break;

		case Controller::Dst::ReverbVolume:
			presetData[REVERB_MIX] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_MIX_POS, presetData[REVERB_MIX]);
		break;

		case Controller::Dst::ReverbTime:
			presetData[REVERB_TIME] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_TIME_POS, presetData[REVERB_TIME]);
		break;
//-------------------------------------------------Tremolo--------------------------------------------
		case Controller::Dst::TremoloOnOff:
			presetData[ENABLE_TREMOLO] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_TREMOLO, presetData[ENABLE_TREMOLO]);
		break;

		case Controller::Dst::TremoloIntensity:
			presetData[TREMOLO_INTENSITY] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS, presetData[TREMOLO_INTENSITY]);
		break;

		case Controller::Dst::TremoloRate:
			presetData[TREMOLO_RATE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS, presetData[TREMOLO_RATE]);
		break;

		case Controller::Dst::TremoloTap:
			if(!tap_del_fl)
			{
				if(tap_temp_global() && !sys_para[TAP_TYPE])
				{
					trem_time = tap_global / 3.0f / tap_time_coefs[presetData[TREMOLO_TAP]];
					if(trem_time < 2731)
					{
						DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_LO_POS, trem_time >> 8);
						DSP_contr_set_parameter(DSP_ADDRESS_TREMOLO, TREMOLO_TIME_HI_POS, trem_time);
					}

					if(tap_moog_fl)
					{
						moog_time = tap_global / 3.0f;
						DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, moog_time >> 8);
						DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, moog_time);
					}
				}
			}
		break;
//---------------------------------------------------------------------------------------------------
		case Controller::Dst::PresetLevel:
			if(!presetData[vol_contr]) DSP_contr_set_parameter(DSP_ADDRESS_PRESET_VOLUME, val, 0);
		break;
//-------------------------------------------------Compressor--------------------------------------------
		case Controller::Dst::CompressorOnOff:
			presetData[ENABLE_COMPRESSOR] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_COMPRESSOR, presetData[ENABLE_COMPRESSOR]);
		break;

		case Controller::Dst::CompressorThreshold:
			presetData[COMPRESSOR_THRESHOLD] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_THRESHOLD_POS, presetData[COMPRESSOR_THRESHOLD]);
		break;

		case Controller::Dst::CompressorVolume:
			presetData[COMPRESSOR_VOLUME] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_VOLUME_POS, presetData[COMPRESSOR_VOLUME]);
		break;
//-------------------------------------------------Moog filter-------------------------------------------
		case Controller::Dst::RfOnOff:
			presetData[moog] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_RESONANCE_FILTER, presetData[ENABLE_RESONANCE_FILTER]);
		break;

		case Controller::Dst::RfLFOrate:
			presetData[RFILTER_RATE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RATE_POS, presetData[RFILTER_RATE]);
		break;

		case Controller::Dst::RfFreq:
			presetData[RFILTER_EXT] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_EXT_POS, presetData[RFILTER_EXT]);
		break;
//--------------------------------------------------Early------------------------------------------------------------
		case Controller::Dst::EROnOff:
			presetData[ENABLE_EARLY_REFLECTIONS] = (val < 64.0f) ? 0 : 1;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EARLY_REFLECTIONS, presetData[ENABLE_EARLY_REFLECTIONS]);
		break;

		case Controller::Dst::ERVolume:
			presetData[EARLY_MIX] = val * 0.5f;
			DSP_contr_set_parameter(DSP_ADDRESS_EARLY_REFLECTIONS, EARLY_MIX_POS, presetData[EARLY_MIX]);
		break;
//---------------------------------------------------Moog Tap--------------------------------------------------------
		case Controller::Dst::RfLFOTAP:
			if(!tap_del_fl && !tap_trem_fl)
			{
				if(tap_temp_global() && !sys_para[TAP_TYPE])
				{
					moog_time = tap_global / 3.0f;
					DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_LO_POS, moog_time >> 8);
					DSP_contr_set_parameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_TIME_HI_POS, moog_time);
				}
			}
		break;

		case Controller::Dst::VolCtrlOnOff:
				presetData[PRESET_VOLUME_CONTROL] = (val < 64.0f) ? 0 : 1;
		break;

		case Controller::Dst::Cab1Volume:
			presetData[IR_VOLUME1] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, presetData[IR_VOLUME1]);
		break;

		case Controller::Dst::Cab2Volume:
			presetData[IR_VOLUME2] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, presetData[IR_VOLUME2]);
		break;

		case Controller::Dst::GateOnOff:
			presetData[ENABLE_GATE] = (val < 64.0f) ? 0 : 1;;
			DSP_contr_set_parameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_GATE, presetData[ENABLE_GATE]);
		break;

		case Controller::Dst::GateThresh:
			presetData[GATE_THRESHOLD] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_GATE, GATE_THRESHOLD_POS, presetData[GATE_THRESHOLD]);
		break;

		case Controller::Dst::HPFfrequency:
			presetData[EQ_HPF] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_HPF_POS, presetData[EQ_HPF]);
		break;

		case Controller::Dst::LPFfrequency:
			presetData[EQ_LPF] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_LPF_POS, presetData[EQ_LPF]);
		break;

		case Controller::Dst::PresenceVal:
			presetData[EQ_PRESENCE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_PRESENCE_POS, presetData[EQ_PRESENCE]);
		break;
//------------------------------------------------------Preamp parameter--------------------------
		case Controller::Dst::PreampGain:
			presetData[PREAMP_GAIN] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS, presetData[PREAMP_GAIN]);
		break;

		case Controller::Dst::PreampVolume:
			presetData[PREAMP_VOLUME] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS, presetData[PREAMP_VOLUME]);
		break;

		case Controller::Dst::PreampLow:
			presetData[PREAMP_LOW] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS, presetData[PREAMP_LOW]);
		break;

		case Controller::Dst::PreampMid:
			presetData[PREAMP_MID] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_MID_POS, presetData[PREAMP_MID]);
		break;

		case Controller::Dst::PreampHigh:
			presetData[PREAMP_HIGH] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS, presetData[PREAMP_HIGH]);
		break;
//-------------------------------------------------------------Eq band-------------------------------
		case Controller::Dst::EqBand1Lev:
			presetData[EQ_G0] = val * 0.25f;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_G0_POS, presetData[EQ_G0]);
		break;

		case Controller::Dst::EqBand2Lev:
			presetData[EQ_G1] = val * 0.25f;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_G1_POS, presetData[EQ_G1]);
		break;

		case Controller::Dst::EqBand3Lev:
			presetData[EQ_G2] = val * 0.25f;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_G2_POS, presetData[EQ_G2]);
		break;

		case Controller::Dst::EqBand4Lev:
			presetData[EQ_G3] = val * 0.25f;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_G3_POS, presetData[EQ_G3]);
		break;

		case Controller::Dst::EqBand5Lev:
			presetData[EQ_G4] = val * 0.25f;
			DSP_contr_set_parameter(DSP_ADDRESS_EQ, EQ_G4_POS, presetData[EQ_G4]);
		break;
//-------------------------------------------------------------Reverb type---------------------------
		case Controller::Dst::ReverbType:
			if(val < 7) presetData[REVERB_TYPE] = val;
			DSP_contr_set_parameter(DSP_ADDRESS_REVERB, REVERB_TYPE_POS, presetData[REVERB_TYPE]);
		break;
	}

	currentMenu->refresh();
}
//------------------------------------------------------------------------------
TCCTask::TCCTask() :
		TTask()
{
}
extern uint8_t tuner_use;
void TCCTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 8, 0);
	Delay(1000);
	while(1)
	{
		sem->Take(portMAX_DELAY);
		if(mid_fl && !tuner_use && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(currentPreset.controller[i].src)
					if(midi_b[1] == currentPreset.controller[0].src - 5)
						controllerSetData(i, midi_b[2]);
			}
			mid_fl = 0;
		}

		if(ext_fl && !tuner_use && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(currentPreset.controller[i].src == ext_sourc)
					controllerSetData(0, ext_data);
			}
			ext_fl = 0;
		}
	}
}

