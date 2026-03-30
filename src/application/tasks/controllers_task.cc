#include "controllers_task.h"

#include "eepr.h"
#include "system.h"
#include "controller.h"
#include "preset.h"
#include "modules.h"

#include "preamp_handlers.h"

#include "ui_task.h"
#include "display_task.h"
#include "io_task.h"
#include "midi_task.h"
#include "sharc_task.h"
#include "console_task.h"

#include "tapmenu.h"
#include "modulesmenu.h"

TControllersTask *ControllersTask = nullptr;

void TControllersTask::controllerSetData(uint8_t adr, uint8_t data)
{
	float val = currentPreset.controller[adr].minVal + (currentPreset.controller[adr].maxVal - currentPreset.controller[adr].minVal) * data / 127.0f;

	switch(currentPreset.controller[adr].dst)
	{
		case Controller::Dst::PreampOnOff:
//			currentPreset.modulesBuf[ENABLE_PREAMP] = (val < 64.0f) ? 0 : 1;
//			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PREAMP, currentPreset.modulesBuf[ENABLE_PREAMP]);
			UITask->setParam(preamp_on_command_handler, (val < 64.0f) ? 0 : 1);
		break;
//-------------------------------------------------PA-------------------------------------------------
		case Controller::Dst::AmpOnOff:
			currentPreset.modulesBuf[ENABLE_AMP] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_AMP, currentPreset.modulesBuf[ENABLE_AMP]);
		break;

		case Controller::Dst::AmpVolume:
			currentPreset.modulesBuf[AMP_MASTER] = val;
			SharcTask->setParameter(DSP_ADDRESS_AMP, AMP_MASTER_POS, currentPreset.modulesBuf[AMP_MASTER]);

		break;

		case Controller::Dst::AmpSlave:
			currentPreset.modulesBuf[AMP_LEVEL] = val;
			SharcTask->setParameter(DSP_ADDRESS_AMP, AMP_LEVEL_POS, currentPreset.modulesBuf[AMP_LEVEL]);
		break;
//-------------------------------------------------CAB SIM--------------------------------------------
		case Controller::Dst::CabSimOnOff:
			currentPreset.modulesBuf[ENABLE_CAB] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CAB, currentPreset.modulesBuf[ENABLE_CAB]);
		break;
//-------------------------------------------------EQ-------------------------------------------------
		case Controller::Dst::EqualOnOff:
			currentPreset.modulesBuf[ENABLE_EQ] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EQ, currentPreset.modulesBuf[ENABLE_EQ]);
		break;
//-------------------------------------------------Delay----------------------------------------------
		case Controller::Dst::DelayOnOff:
			currentPreset.modulesBuf[ENABLE_DELAY] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_DELAY, currentPreset.modulesBuf[ENABLE_DELAY]);
		break;

		case Controller::Dst::DelayVolume:
			currentPreset.modulesBuf[DELAY_MIX] = val;
			SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_MIX_POS, currentPreset.modulesBuf[DELAY_MIX]);
		break;

		case Controller::Dst::DelayFeedback:
			currentPreset.modulesBuf[DELAY_FEEDBACK] = val;
			SharcTask->setParameter(DSP_ADDRESS_DELAY, DELAY_FEEDBACK_POS, currentPreset.modulesBuf[DELAY_FEEDBACK]);
		break;

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
//-------------------------------------------------Phaser---------------------------------------------
		case Controller::Dst::PhaserOnOff:
			currentPreset.modulesBuf[ENABLE_PHASER] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_PHASER, currentPreset.modulesBuf[ENABLE_PHASER]);
		break;

		case Controller::Dst::PhaserVolume:
			currentPreset.modulesBuf[PHASER_MIX] = val;
			SharcTask->setParameter(DSP_ADDRESS_PHASER, PHASER_MIX_POS, currentPreset.modulesBuf[PHASER_MIX]);
		break;

		case Controller::Dst::PhaserRate:
			currentPreset.modulesBuf[PHASER_RATE] = val;
			SharcTask->setParameter(DSP_ADDRESS_PHASER, PHASER_RATE_POS, currentPreset.modulesBuf[PHASER_RATE]);
		break;
//-------------------------------------------------Flanger--------------------------------------------
		case Controller::Dst::FlangerOnOff:
			currentPreset.modulesBuf[ENABLE_FLANGER] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_FLANGER, currentPreset.modulesBuf[ENABLE_FLANGER]);
		break;

		case Controller::Dst::FlangerVolume:
			currentPreset.modulesBuf[FLANGER_MIX] = val;
			SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_MIX_POS, currentPreset.modulesBuf[FLANGER_MIX]);
		break;

		case Controller::Dst::FlangerRate:
			currentPreset.modulesBuf[FLANGER_RATE] = val;
			SharcTask->setParameter(DSP_ADDRESS_FLANGER, FLANGER_RATE_POS, currentPreset.modulesBuf[FLANGER_RATE]);
		break;
//-------------------------------------------------Chorus---------------------------------------------
		case Controller::Dst::ChorusOnOff:
			currentPreset.modulesBuf[ENABLE_CHORUS] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CHORUS, currentPreset.modulesBuf[ENABLE_CHORUS]);
		break;

		case Controller::Dst::ChorusVolume:
			currentPreset.modulesBuf[CHORUS_MIX] = val;
			SharcTask->setParameter(DSP_ADDRESS_CHORUS, CHORUS_MIX_POS, currentPreset.modulesBuf[CHORUS_MIX]);
		break;

		case Controller::Dst::ChorusRate:
			currentPreset.modulesBuf[CHORUS_RATE] = val;
			SharcTask->setParameter(DSP_ADDRESS_CHORUS, CHORUS_RATE_POS, currentPreset.modulesBuf[CHORUS_RATE]);
		break;
//-------------------------------------------------Reverb---------------------------------------------
		case Controller::Dst::ReverbOnOff:
			currentPreset.modulesBuf[ENABLE_REVERB] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_REVERB, currentPreset.modulesBuf[ENABLE_REVERB]);
		break;

		case Controller::Dst::ReverbVolume:
			currentPreset.modulesBuf[REVERB_MIX] = val;
			SharcTask->setParameter(DSP_ADDRESS_REVERB, REVERB_MIX_POS, currentPreset.modulesBuf[REVERB_MIX]);
		break;

		case Controller::Dst::ReverbTime:
			currentPreset.modulesBuf[REVERB_TIME] = val;
			SharcTask->setParameter(DSP_ADDRESS_REVERB, REVERB_TIME_POS, currentPreset.modulesBuf[REVERB_TIME]);
		break;
//-------------------------------------------------Tremolo--------------------------------------------
		case Controller::Dst::TremoloOnOff:
			currentPreset.modulesBuf[ENABLE_TREMOLO] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_TREMOLO, currentPreset.modulesBuf[ENABLE_TREMOLO]);
		break;

		case Controller::Dst::TremoloIntensity:
			currentPreset.modulesBuf[TREMOLO_INTENSITY] = val;
			SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_INTENSITY_POS, currentPreset.modulesBuf[TREMOLO_INTENSITY]);
		break;

		case Controller::Dst::TremoloRate:
			currentPreset.modulesBuf[TREMOLO_RATE] = val;
			SharcTask->setParameter(DSP_ADDRESS_TREMOLO, TREMOLO_RATE_POS, currentPreset.modulesBuf[TREMOLO_RATE]);
		break;

		case Controller::Dst::TremoloTap:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					UITask->showMenu(new TapMenu(currentMenu, System::TapDestination::TAP_TREMOLO));
				else
					currentMenu->keyDown(); // thread safe in MENU_TAP_SCREEN
			}
			else
			{
				System::TapTempo(System::TAP_TREMOLO);
			}
		break;
//---------------------------------------------------------------------------------------------------
		case Controller::Dst::PresetLevel:
			if(!currentPreset.modulesBuf[PRESET_VOLUME]) SharcTask->setParameter(DSP_ADDRESS_PRESET_VOLUME, val, 0);
		break;
//-------------------------------------------------Compressor--------------------------------------------
		case Controller::Dst::CompressorOnOff:
			currentPreset.modulesBuf[ENABLE_COMPRESSOR] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_COMPRESSOR, currentPreset.modulesBuf[ENABLE_COMPRESSOR]);
		break;

		case Controller::Dst::CompressorThreshold:
			currentPreset.modulesBuf[COMPRESSOR_THRESHOLD] = val;
			SharcTask->setParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_THRESHOLD_POS, currentPreset.modulesBuf[COMPRESSOR_THRESHOLD]);
		break;

		case Controller::Dst::CompressorVolume:
			currentPreset.modulesBuf[COMPRESSOR_VOLUME] = val;
			SharcTask->setParameter(DSP_ADDRESS_COMPRESSOR, COMPRESSOR_VOLUME_POS, currentPreset.modulesBuf[COMPRESSOR_VOLUME]);
		break;
//-------------------------------------------------Moog filter-------------------------------------------
		case Controller::Dst::RfOnOff:
			currentPreset.modulesBuf[ENABLE_CAB] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_RESONANCE_FILTER, currentPreset.modulesBuf[ENABLE_RESONANCE_FILTER]);
		break;

		case Controller::Dst::RfLFOrate:
			currentPreset.modulesBuf[RFILTER_RATE] = val;
			SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_RATE_POS, currentPreset.modulesBuf[RFILTER_RATE]);
		break;

		case Controller::Dst::RfFreq:
			currentPreset.modulesBuf[RFILTER_EXT] = val;
			SharcTask->setParameter(DSP_ADDRESS_RESONANCE_FILTER, RFILTER_EXT_POS, currentPreset.modulesBuf[RFILTER_EXT]);
		break;

//---------------------------------------------------Moog Tap--------------------------------------------------------
		case Controller::Dst::RfLFOTAP:
			if(sys_para[System::TAP_SCREEN_POPUP] == System::TAP_SCREEN_ON)
			{
				if(currentMenu->menuType() != MENU_TAP_SCREEN)
					UITask->showMenu(new TapMenu(currentMenu, System::TapDestination::TAP_RFILTER));
				else
					currentMenu->keyDown();	// thread safe in MENU_TAP_SCREEN
			}
			else
			{
				System::TapTempo(System::TAP_RFILTER);
			}
		break;
//--------------------------------------------------Early------------------------------------------------------------
		case Controller::Dst::EROnOff:

			currentPreset.modulesBuf[ENABLE_EARLY_REFLECTIONS] = (val < 64.0f) ? 0 : 1;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_EARLY_REFLECTIONS, currentPreset.modulesBuf[ENABLE_EARLY_REFLECTIONS]);
		break;

		case Controller::Dst::ERVolume:
			currentPreset.modulesBuf[EARLY_MIX] = val * 0.5f;
			SharcTask->setParameter(DSP_ADDRESS_EARLY_REFLECTIONS, EARLY_MIX_POS, currentPreset.modulesBuf[EARLY_MIX]);
		break;

//-------------------------------------------------------------------------------------------------------------------
		case Controller::Dst::VolCtrlOnOff:
			currentPreset.modulesBuf[PRESET_VOLUME_CONTROL] = (val < 64.0f) ? 0 : 1;
		break;

		case Controller::Dst::Cab1Volume:
			currentPreset.modulesBuf[IR_VOLUME1] = val;
			SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
		break;

		case Controller::Dst::Cab2Volume:
			currentPreset.modulesBuf[IR_VOLUME2] = val;
			SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
		break;

		case Controller::Dst::GateOnOff:
			currentPreset.modulesBuf[ENABLE_GATE] = (val < 64.0f) ? 0 : 1;;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_GATE, currentPreset.modulesBuf[ENABLE_GATE]);
		break;

		case Controller::Dst::GateThresh:
			currentPreset.modulesBuf[GATE_THRESHOLD] = val;
			SharcTask->setParameter(DSP_ADDRESS_GATE, GATE_THRESHOLD_POS, currentPreset.modulesBuf[GATE_THRESHOLD]);
		break;

		case Controller::Dst::HPFfrequency:
			currentPreset.modulesBuf[EQ_HPF] = val;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_HPF_POS, currentPreset.modulesBuf[EQ_HPF]);
		break;

		case Controller::Dst::LPFfrequency:
			currentPreset.modulesBuf[EQ_LPF] = val;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_LPF_POS, currentPreset.modulesBuf[EQ_LPF]);
		break;

		case Controller::Dst::PresenceVal:
			currentPreset.modulesBuf[EQ_PRESENCE] = val;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_PRESENCE_POS, currentPreset.modulesBuf[EQ_PRESENCE]);
		break;
//------------------------------------------------------Preamp parameter--------------------------
		case Controller::Dst::PreampGain:
			currentPreset.modulesBuf[PREAMP_GAIN] = val;
			SharcTask->setParameter(DSP_ADDRESS_PREAMP, PREAMP_GAIN_POS, currentPreset.modulesBuf[PREAMP_GAIN]);
		break;

		case Controller::Dst::PreampVolume:
			currentPreset.modulesBuf[PREAMP_VOLUME] = val;
			SharcTask->setParameter(DSP_ADDRESS_PREAMP, PREAMP_VOLUME_POS, currentPreset.modulesBuf[PREAMP_VOLUME]);
		break;

		case Controller::Dst::PreampLow:
			currentPreset.modulesBuf[PREAMP_LOW] = val;
			SharcTask->setParameter(DSP_ADDRESS_PREAMP, PREAMP_LOW_POS, currentPreset.modulesBuf[PREAMP_LOW]);
		break;

		case Controller::Dst::PreampMid:
			currentPreset.modulesBuf[PREAMP_MID] = val;
			SharcTask->setParameter(DSP_ADDRESS_PREAMP, PREAMP_MID_POS, currentPreset.modulesBuf[PREAMP_MID]);
		break;

		case Controller::Dst::PreampHigh:
			currentPreset.modulesBuf[PREAMP_HIGH] = val;
			SharcTask->setParameter(DSP_ADDRESS_PREAMP, PREAMP_HIGH_POS, currentPreset.modulesBuf[PREAMP_HIGH]);
		break;
//-------------------------------------------------------------Eq band-------------------------------
		case Controller::Dst::EqBand1Lev:
			currentPreset.modulesBuf[EQ_G0] = val * 0.25f;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_G0_POS, currentPreset.modulesBuf[EQ_G0]);
		break;

		case Controller::Dst::EqBand2Lev:
			currentPreset.modulesBuf[EQ_G1] = val * 0.25f;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_G1_POS, currentPreset.modulesBuf[EQ_G1]);
		break;

		case Controller::Dst::EqBand3Lev:
			currentPreset.modulesBuf[EQ_G2] = val * 0.25f;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_G2_POS, currentPreset.modulesBuf[EQ_G2]);
		break;

		case Controller::Dst::EqBand4Lev:
			currentPreset.modulesBuf[EQ_G3] = val * 0.25f;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_G3_POS, currentPreset.modulesBuf[EQ_G3]);
		break;

		case Controller::Dst::EqBand5Lev:
			currentPreset.modulesBuf[EQ_G4] = val * 0.25f;
			SharcTask->setParameter(DSP_ADDRESS_EQ, EQ_G4_POS, currentPreset.modulesBuf[EQ_G4]);
		break;
//-------------------------------------------------------------Reverb type---------------------------
		case Controller::Dst::ReverbType:
			if(val < 7) currentPreset.modulesBuf[REVERB_TYPE] = val;
			SharcTask->setParameter(DSP_ADDRESS_REVERB, REVERB_TYPE_POS, currentPreset.modulesBuf[REVERB_TYPE]);
		break;
	}
}
//------------------------------------------------------------------------------
TControllersTask::TControllersTask() :
		TTask()
{
}

void TControllersTask::midiCommand(uint8_t source, uint8_t data)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_MIDI;
	cmd.midiCmd.source = source;
	cmd.midiCmd.data = data;
	Command(&cmd);
}

void TControllersTask::extCommand(uint8_t source, uint8_t data)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_EXTERNAL;
	cmd.extCmd.source = source;
	cmd.extCmd.data = data;
	Command(&cmd);
}

void TControllersTask::midiCalcTempo(uint16_t* data, uint8_t size)
{
	TControllerCmd cmd;
	cmd.type = CONTROLLER_TEMPO;
	cmd.tempoCmd.data = data;
	cmd.tempoCmd.size = size;
	Command(&cmd);
}

void TControllersTask::Code()
{
	queue = new TQueue(64, sizeof(TControllerCmd));
	if(!queue) Suspend();
	if(!queue->IsCreated()) Suspend();

	TControllerCmd cmd;

	for(int i =0; i < maBufSize; i++) maMidiClockBuf[i] = 500;

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_CCTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.type)
		{
			case CONTROLLER_MIDI:
			{
				for(uint8_t i=0; i < Controller::controllersCount; i++)
				{
					if(cmd.midiCmd.source == currentPreset.controller[i].src)
						controllerSetData(i, cmd.midiCmd.data);
				}
				break;
			}

			case CONTROLLER_EXTERNAL:
			{
				for(uint8_t i=0; i < Controller::controllersCount; i++)
				{
					if(currentPreset.controller[i].src == cmd.extCmd.source)
						controllerSetData(i, cmd.extCmd.data);
				}
				break;
			}

			case CONTROLLER_TEMPO:
			{
				uint16_t buf[32];
				kgp_sdk_libc::memcpy(buf, cmd.tempoCmd.data, cmd.tempoCmd.size * sizeof(uint16_t));

				float roundedInterval = 0;
				for(int i=0; i<cmd.tempoCmd.size; i++)
				{
					roundedInterval += buf[i];
				}
				roundedInterval = roundedInterval/cmd.tempoCmd.size;

				if(maMidiClockBufPos == maBufSize) maMidiClockBufPos = 0;

				float result = 0;
				for(int i =0; i < maBufSize; i++) result += maMidiClockBuf[i];
				result = result / maBufSize;

				System::setMoogTime(result);
				System::setDelayTime(result);
				System::setTremoloTime(result);
				UITask->refreshMenu();
				break;
			}
		}

		if(currentMenu->menuType() != MENU_MAIN) // block blinking on EXPR pedal
			UITask->refreshMenu();
	}
}

