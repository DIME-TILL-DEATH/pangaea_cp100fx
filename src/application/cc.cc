#include "appdefs.h"
#include "eepr.h"
#include "display.h"
#include "cc.h"
#include "cs.h"
#include "enc.h"
#include "display.h"

#include "BF706_send.h"
#include "midi_send.h"

#include "system.h"

#include "allFonts.h"
#include "gui_task.h"
#include "modulesmenu.h"

#include "controllers.h"
#include "preset.h"
#include "modules.h"

#include "tapmenu.h"

TCCTask *CCTask;

volatile uint8_t midi_b[3];
uint8_t mid_fl;
uint8_t ext_fl;
uint8_t ext_f_fl;
uint8_t ext_data;
uint8_t ext_sourc = 255;

void controllerSetData(uint8_t adr, uint8_t data)
{
	float val = currentPreset.controller[adr].minVal + (currentPreset.controller[adr].maxVal - currentPreset.controller[adr].minVal) * data / 127.0f;

	switch(currentPreset.controller[adr].dst)
	{
		case Controller::Dst::PreampOnOff:
			currentPreset.modules.rawData[ENABLE_PREAMP] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PREAMP, currentPreset.modules.rawData[ENABLE_PREAMP]);
		break;
//-------------------------------------------------PA-------------------------------------------------
		case Controller::Dst::AmpOnOff:
			currentPreset.modules.rawData[ENABLE_AMP] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_AMP, currentPreset.modules.rawData[ENABLE_AMP]);
		break;

		case Controller::Dst::AmpVolume:
			currentPreset.modules.rawData[AMP_MASTER] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_AMP, AMP_MASTER_POS, currentPreset.modules.rawData[AMP_MASTER]);

//			ConsoleTask->WriteToInputBuff("pa_ms 5\r\n");
		break;

		case Controller::Dst::AmpSlave:
			currentPreset.modules.rawData[AMP_LEVEL] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_AMP, AMP_LEVEL_POS, currentPreset.modules.rawData[AMP_LEVEL]);
		break;
//-------------------------------------------------CAB SIM--------------------------------------------
		case Controller::Dst::CabSimOnOff:
			currentPreset.modules.rawData[ENABLE_CAB] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CAB, currentPreset.modules.rawData[ENABLE_CAB]);
		break;
//-------------------------------------------------EQ-------------------------------------------------
		case Controller::Dst::EqualOnOff:
			currentPreset.modules.rawData[ENABLE_EQ] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EQ, currentPreset.modules.rawData[ENABLE_EQ]);
		break;
//-------------------------------------------------Delay----------------------------------------------
		case Controller::Dst::DelayOnOff:
			currentPreset.modules.rawData[ENABLE_DELAY] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_DELAY, currentPreset.modules.rawData[ENABLE_DELAY]);
		break;

		case Controller::Dst::DelayVolume:
			currentPreset.modules.rawData[DELAY_MIX] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_DELAY, DELAY_MIX_POS, currentPreset.modules.rawData[DELAY_MIX]);
		break;

		case Controller::Dst::DelayFeedback:
			currentPreset.modules.rawData[DELAY_FEEDBACK] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS, currentPreset.modules.rawData[DELAY_FEEDBACK]);
		break;

		case Controller::Dst::DelayTap:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					currentMenu->showChild(new TapMenu(currentMenu, System::TapDestination::TAP_DELAY));
				else
					currentMenu->keyDown();
			}
			else
			{
				System::TapTempo(System::TAP_DELAY);
			}
		break;
//-------------------------------------------------Phaser---------------------------------------------
		case Controller::Dst::PhaserOnOff:
			currentPreset.modules.rawData[ENABLE_PHASER] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PHASER, currentPreset.modules.rawData[ENABLE_PHASER]);
		break;

		case Controller::Dst::PhaserVolume:
			currentPreset.modules.rawData[PHASER_MIX] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PHASER, PHASER_MIX_POS, currentPreset.modules.rawData[PHASER_MIX]);
		break;

		case Controller::Dst::PhaserRate:
			currentPreset.modules.rawData[PHASER_RATE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PHASER, PHASER_RATE_POS, currentPreset.modules.rawData[PHASER_RATE]);
		break;
//-------------------------------------------------Flanger--------------------------------------------
		case Controller::Dst::FlangerOnOff:
			currentPreset.modules.rawData[ENABLE_FLANGER] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_FLANGER, currentPreset.modules.rawData[ENABLE_FLANGER]);
		break;

		case Controller::Dst::FlangerVolume:
			currentPreset.modules.rawData[FLANGER_MIX] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_FLANGER, FLANGER_MIX_POS, currentPreset.modules.rawData[FLANGER_MIX]);
		break;

		case Controller::Dst::FlangerRate:
			currentPreset.modules.rawData[FLANGER_RATE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_FLANGER, FLANGER_RATE_POS, currentPreset.modules.rawData[FLANGER_RATE]);
		break;
//-------------------------------------------------Chorus---------------------------------------------
		case Controller::Dst::ChorusOnOff:
			currentPreset.modules.rawData[ENABLE_CHORUS] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CHORUS, currentPreset.modules.rawData[ENABLE_CHORUS]);
		break;

		case Controller::Dst::ChorusVolume:
			currentPreset.modules.rawData[CHORUS_MIX] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_CHORUS, CHORUS_MIX_POS, currentPreset.modules.rawData[CHORUS_MIX]);
		break;

		case Controller::Dst::ChorusRate:
			currentPreset.modules.rawData[CHORUS_RATE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_CHORUS, CHORUS_RATE_POS, currentPreset.modules.rawData[CHORUS_RATE]);
		break;
//-------------------------------------------------Reverb---------------------------------------------
		case Controller::Dst::ReverbOnOff:
			currentPreset.modules.rawData[ENABLE_REVERB] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_REVERB, currentPreset.modules.rawData[ENABLE_REVERB]);
		break;

		case Controller::Dst::ReverbVolume:
			currentPreset.modules.rawData[REVERB_MIX] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_MIX_POS, currentPreset.modules.rawData[REVERB_MIX]);
		break;

		case Controller::Dst::ReverbTime:
			currentPreset.modules.rawData[REVERB_TIME] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_TIME_POS, currentPreset.modules.rawData[REVERB_TIME]);
		break;
//-------------------------------------------------Tremolo--------------------------------------------
		case Controller::Dst::TremoloOnOff:
			currentPreset.modules.rawData[ENABLE_TREMOLO] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_TREMOLO, currentPreset.modules.rawData[ENABLE_TREMOLO]);
		break;

		case Controller::Dst::TremoloIntensity:
			currentPreset.modules.rawData[TREMOLO_INTENSITY] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS, currentPreset.modules.rawData[TREMOLO_INTENSITY]);
		break;

		case Controller::Dst::TremoloRate:
			currentPreset.modules.rawData[TREMOLO_RATE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS, currentPreset.modules.rawData[TREMOLO_RATE]);
		break;

		case Controller::Dst::TremoloTap:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					currentMenu->showChild(new TapMenu(currentMenu, System::TapDestination::TAP_TREMOLO));
				else
					currentMenu->keyDown();
			}
			else
			{
				System::TapTempo(System::TAP_TREMOLO);
			}
		break;
//---------------------------------------------------------------------------------------------------
		case Controller::Dst::PresetLevel:
			if(!currentPreset.modules.rawData[vol_contr]) DSP_ContrSendParameter(DSP_ADDRESS_PRESET_VOLUME, val, 0);
		break;
//-------------------------------------------------Compressor--------------------------------------------
		case Controller::Dst::CompressorOnOff:
			currentPreset.modules.rawData[ENABLE_COMPRESSOR] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_COMPRESSOR, currentPreset.modules.rawData[ENABLE_COMPRESSOR]);
		break;

		case Controller::Dst::CompressorThreshold:
			currentPreset.modules.rawData[COMPRESSOR_THRESHOLD] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_THRESHOLD_POS, currentPreset.modules.rawData[COMPRESSOR_THRESHOLD]);
		break;

		case Controller::Dst::CompressorVolume:
			currentPreset.modules.rawData[COMPRESSOR_VOLUME] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_VOLUME_POS, currentPreset.modules.rawData[COMPRESSOR_VOLUME]);
		break;
//-------------------------------------------------Moog filter-------------------------------------------
		case Controller::Dst::RfOnOff:
			currentPreset.modules.rawData[moog] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_RESONANCE_FILTER, currentPreset.modules.rawData[ENABLE_RESONANCE_FILTER]);
		break;

		case Controller::Dst::RfLFOrate:
			currentPreset.modules.rawData[RFILTER_RATE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RATE_POS, currentPreset.modules.rawData[RFILTER_RATE]);
		break;

		case Controller::Dst::RfFreq:
			currentPreset.modules.rawData[RFILTER_EXT] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_EXT_POS, currentPreset.modules.rawData[RFILTER_EXT]);
		break;

//---------------------------------------------------Moog Tap--------------------------------------------------------
		case Controller::Dst::RfLFOTAP:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					currentMenu->showChild(new TapMenu(currentMenu, System::TapDestination::TAP_RFILTER));
				else
					currentMenu->keyDown();
			}
			else
			{
				System::TapTempo(System::TAP_RFILTER);
			}
		break;
//--------------------------------------------------Early------------------------------------------------------------
		case Controller::Dst::EROnOff:

			currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS] = (val < 64.0f) ? 0 : 1;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EARLY_REFLECTIONS, currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS]);
		break;

		case Controller::Dst::ERVolume:
			currentPreset.modules.rawData[EARLY_MIX] = val * 0.5f;
			DSP_ContrSendParameter(DSP_ADDRESS_EARLY_REFLECTIONS, EARLY_MIX_POS, currentPreset.modules.rawData[EARLY_MIX]);
		break;

//-------------------------------------------------------------------------------------------------------------------
		case Controller::Dst::VolCtrlOnOff:
				currentPreset.modules.rawData[PRESET_VOLUME_CONTROL] = (val < 64.0f) ? 0 : 1;
		break;

		case Controller::Dst::Cab1Volume:
			currentPreset.modules.rawData[IR_VOLUME1] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modules.rawData[IR_VOLUME1]);
		break;

		case Controller::Dst::Cab2Volume:
			currentPreset.modules.rawData[IR_VOLUME2] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modules.rawData[IR_VOLUME2]);
		break;

		case Controller::Dst::GateOnOff:
			currentPreset.modules.rawData[ENABLE_GATE] = (val < 64.0f) ? 0 : 1;;
			DSP_ContrSendParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_GATE, currentPreset.modules.rawData[ENABLE_GATE]);
		break;

		case Controller::Dst::GateThresh:
			currentPreset.modules.rawData[GATE_THRESHOLD] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_GATE, GATE_THRESHOLD_POS, currentPreset.modules.rawData[GATE_THRESHOLD]);
		break;

		case Controller::Dst::HPFfrequency:
			currentPreset.modules.rawData[EQ_HPF] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_HPF_POS, currentPreset.modules.rawData[EQ_HPF]);
		break;

		case Controller::Dst::LPFfrequency:
			currentPreset.modules.rawData[EQ_LPF] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_LPF_POS, currentPreset.modules.rawData[EQ_LPF]);
		break;

		case Controller::Dst::PresenceVal:
			currentPreset.modules.rawData[EQ_PRESENCE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_PRESENCE_POS, currentPreset.modules.rawData[EQ_PRESENCE]);
		break;
//------------------------------------------------------Preamp parameter--------------------------
		case Controller::Dst::PreampGain:
			currentPreset.modules.rawData[PREAMP_GAIN] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS, currentPreset.modules.rawData[PREAMP_GAIN]);
		break;

		case Controller::Dst::PreampVolume:
			currentPreset.modules.rawData[PREAMP_VOLUME] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS, currentPreset.modules.rawData[PREAMP_VOLUME]);
		break;

		case Controller::Dst::PreampLow:
			currentPreset.modules.rawData[PREAMP_LOW] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS, currentPreset.modules.rawData[PREAMP_LOW]);
		break;

		case Controller::Dst::PreampMid:
			currentPreset.modules.rawData[PREAMP_MID] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PREAMP, PREAMP_MID_POS, currentPreset.modules.rawData[PREAMP_MID]);
		break;

		case Controller::Dst::PreampHigh:
			currentPreset.modules.rawData[PREAMP_HIGH] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS, currentPreset.modules.rawData[PREAMP_HIGH]);
		break;
//-------------------------------------------------------------Eq band-------------------------------
		case Controller::Dst::EqBand1Lev:
			currentPreset.modules.rawData[EQ_G0] = val * 0.25f;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_G0_POS, currentPreset.modules.rawData[EQ_G0]);
		break;

		case Controller::Dst::EqBand2Lev:
			currentPreset.modules.rawData[EQ_G1] = val * 0.25f;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_G1_POS, currentPreset.modules.rawData[EQ_G1]);
		break;

		case Controller::Dst::EqBand3Lev:
			currentPreset.modules.rawData[EQ_G2] = val * 0.25f;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_G2_POS, currentPreset.modules.rawData[EQ_G2]);
		break;

		case Controller::Dst::EqBand4Lev:
			currentPreset.modules.rawData[EQ_G3] = val * 0.25f;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_G3_POS, currentPreset.modules.rawData[EQ_G3]);
		break;

		case Controller::Dst::EqBand5Lev:
			currentPreset.modules.rawData[EQ_G4] = val * 0.25f;
			DSP_ContrSendParameter(DSP_ADDRESS_EQ, EQ_G4_POS, currentPreset.modules.rawData[EQ_G4]);
		break;
//-------------------------------------------------------------Reverb type---------------------------
		case Controller::Dst::ReverbType:
			if(val < 7) currentPreset.modules.rawData[REVERB_TYPE] = val;
			DSP_ContrSendParameter(DSP_ADDRESS_REVERB, REVERB_TYPE_POS, currentPreset.modules.rawData[REVERB_TYPE]);
		break;
	}
}
//------------------------------------------------------------------------------
TCCTask::TCCTask() :
		TTask()
{
}

void TCCTask::Code()
{
	sem = new TSemaphore(TSemaphore::fstCounting, 8, 0);
	Delay(1000);
	while(1)
	{
		if(!currentMenu) this->Give();

		sem->Take(portMAX_DELAY);
		if(mid_fl && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(midi_b[1] == currentPreset.controller[i].src - 5)
					controllerSetData(i, midi_b[2]);
			}
			mid_fl = 0;

			if(currentMenu->menuType() != MENU_MAIN) // block blinking and loadBrief request
				currentMenu->refresh();
		}

		if(ext_fl && pc_mute_fl)
		{
			for(uint8_t i=0; i < controllersCount; i++)
			{
				if(currentPreset.controller[i].src == ext_sourc)
					controllerSetData(i, ext_data);
			}
			ext_fl = 0;

			if(currentMenu->menuType() != MENU_MAIN) // block blinking and loadBrief request
				currentMenu->refresh();
		}
	}
}

