#include "controller.h"

#include "preset.h"
#include "modules.h"
#include "system.h"

#include "preset_accessors.h"

#include "ui_task.h"
#include "sharc_task.h"

#include "tapmenu.h"


void Controller::SetData(const TController& controller, uint8_t data)
{
	float val = controller.minVal + (controller.maxVal - controller.minVal) * data / 127.0f;

	switch(controller.dst)
	{
		//---------------------------------Resonance filter-------------------------------------------
		case Controller::Dst::RfOnOff: UITask->setParam(rf_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::RfLFOrate: UITask->setParam(rf_rate_setter, val); break;

		case Controller::Dst::RfFreq: // wah-wah, not presented in gui
			currentPreset.modulesBuf[RFILTER_EXT] = val;
			SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_EXT_POS, currentPreset.modulesBuf[RFILTER_EXT]);
		break;

		case Controller::Dst::RfLFOTAP:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN) UITask->showMenu(new TapMenu(currentMenu, System::TapDestination::TAP_RFILTER));
				else currentMenu->keyDown();	// thread safe in MENU_TAP_SCREEN
			}
			else
			{
				System::TapTempo(System::TAP_RFILTER);
			}
		break;

//-----------------------------------------------Noise gate---------------------------------------------
		case Controller::Dst::GateOnOff: UITask->setParam(gate_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::GateThresh: UITask->setParam(gate_threshold_setter, val); break;

//----------------------------------------------Compressor--------------------------------------------
		case Controller::Dst::CompressorOnOff: UITask->setParam(compressor_on_setter ,(val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::CompressorThreshold: UITask->setParam(compressor_threshold_setter, val); break;
		case Controller::Dst::CompressorVolume: UITask->setParam(compressor_volume_setter, val); break;

//------------------------------------------------Preamp--------------------------
		case Controller::Dst::PreampOnOff: UITask->setParam(preamp_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::PreampGain: 	UITask->setParam(preamp_gain_setter, val); break;
		case Controller::Dst::PreampVolume: UITask->setParam(preamp_volume_setter, val); break;
		case Controller::Dst::PreampLow: 	UITask->setParam(preamp_low_setter, val); break;
		case Controller::Dst::PreampMid: 	UITask->setParam(preamp_mid_setter, val); break;
		case Controller::Dst::PreampHigh: 	UITask->setParam(preamp_high_setter, val); break;

		//------------------------------------Power amp--------------------------------------
		case Controller::Dst::AmpOnOff: UITask->setParam(amp_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::AmpVolume: UITask->setParam(amp_master_setter, val); break;
		case Controller::Dst::AmpSlave: UITask->setParam(amp_level_setter, val); break;
		case Controller::Dst::PresenceVal: UITask->setParam(amp_presence_setter, val); break;

//---------------------------------------------CAB SIM--------------------------------------------
		case Controller::Dst::CabSimOnOff: UITask->setParam(ir_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::Cab1Volume: UITask->setParam(ir_volume1_setter, val); break;
		case Controller::Dst::Cab2Volume: UITask->setParam(ir_volume2_setter, val); break;

//----------------------------------------------EQ-------------------------------
		case Controller::Dst::EqualOnOff: UITask->setParam(eq_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::EqBand1Lev:
		{
			uint8_t res = val * 0.25f;
			UITask->setParam(eq_gain_setter, 0 << 16 | res);
			break;
		}
		case Controller::Dst::EqBand2Lev:
		{
			uint8_t res = val * 0.25f;
			UITask->setParam(eq_gain_setter, 1 << 16 | res);
			break;
		}
		case Controller::Dst::EqBand3Lev:
		{
			uint8_t res = val * 0.25f;
			UITask->setParam(eq_gain_setter, 2 << 16 | res);
			break;
		}
		case Controller::Dst::EqBand4Lev:
		{
			uint8_t res = val * 0.25f;
			UITask->setParam(eq_gain_setter, 3 << 16 | res);
			break;
		}
		case Controller::Dst::EqBand5Lev:
		{
			uint8_t res = val * 0.25f;
			UITask->setParam(eq_gain_setter, 4 << 16 | res);
			break;
		}
		case Controller::Dst::HPFfrequency: UITask->setParam(hpf_freq_setter, val); break;
		case Controller::Dst::LPFfrequency: UITask->setParam(lpf_freq_setter, val); break;

//-------------------------------------------------Phaser---------------------------------------------
		case Controller::Dst::PhaserOnOff: UITask->setParam(phaser_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::PhaserMix: UITask->setParam(phaser_mix_setter, val); break;
		case Controller::Dst::PhaserRate: UITask->setParam(phaser_rate_setter, val); break;

//-------------------------------------------------Flanger--------------------------------------------
		case Controller::Dst::FlangerOnOff: UITask->setParam(flanger_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::FlangerMix: UITask->setParam(flanger_mix_setter, val); break;
		case Controller::Dst::FlangerRate: UITask->setParam(flanger_rate_setter, val); break;

//-------------------------------------------------Chorus---------------------------------------------
		case Controller::Dst::ChorusOnOff: UITask->setParam(chorus_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::ChorusMix: UITask->setParam(chorus_mix_setter, val); break;
		case Controller::Dst::ChorusRate: UITask->setParam(chorus_rate_setter, val); break;

//-------------------------------------------------Delay----------------------------------------------
		case Controller::Dst::DelayOnOff: UITask->setParam(delay_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::DelayMix: UITask->setParam(delay_mix_setter, val);break;
		case Controller::Dst::DelayFeedback: UITask->setParam(delay_feedback_setter, val);break;
		case Controller::Dst::DelayTap:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					UITask->showMenu(new TapMenu(currentMenu, System::TapDestination::TAP_DELAY));
				else
					currentMenu->keyDown(); // thread safe in MENU_TAP_SCREEN
			}
			else
			{
				System::TapTempo(System::TAP_DELAY);
			}
		break;

//--------------------------------------------------Early------------------------------------------------------------
		case Controller::Dst::EROnOff: UITask->setParam(early_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::ERMix: UITask->setParam(early_mix_setter, val); break;

//-------------------------------------------------Reverb---------------------------------------------
		case Controller::Dst::ReverbOnOff: UITask->setParam(reverb_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::ReverbMix: UITask->setParam(reverb_mix_setter, val); break;
		case Controller::Dst::ReverbTime: UITask->setParam(reverb_time_setter, val); break;
		case Controller::Dst::ReverbType: UITask->setParam(reverb_type_setter, val); break;

//-------------------------------------------------Tremolo--------------------------------------------
		case Controller::Dst::TremoloOnOff: UITask->setParam(tremolo_on_setter, (val < 64.0f) ? 0 : 1); break;
		case Controller::Dst::TremoloIntensity: UITask->setParam(tremolo_intensity_setter, val); break;
		case Controller::Dst::TremoloRate: UITask->setParam(tremolo_rate_setter, val); break;
		case Controller::Dst::TremoloTap:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN) UITask->showMenu(new TapMenu(currentMenu, System::TapDestination::TAP_TREMOLO));
				else currentMenu->keyDown(); // thread safe in MENU_TAP_SCREEN
			}
			else
			{
				System::TapTempo(System::TAP_TREMOLO);
			}
		break;

//---------------------------------------------------------------------------------------------------
		case Controller::Dst::PresetLevel: UITask->setParam(preset_volume_setter, val); break;
		case Controller::Dst::VolCtrlOnOff: UITask->setParam(preset_volume_control_setter, (val < 64.0f) ? 0 : 1); break;
	}
}
