#include "console_cmd_handlers.h"

#include "translator.h"

#include "system.h"
#include "controller.h"
#include "modules.h"
#include "eeprom.h"

#include "console_helpers.h"
#include "hardware_handlers.h"
#include "preset_accessors.h"
#include "syssettings_handlers.h"

#include "filesystem_task.h"
#include "midi_task.h"
#include "sharc_task.h"
#include "ui_task.h"
#include "tuner_task.h"


#include "amt.h"
#include "phaser_setters.h"
#include "resonance_filter_setters.h"
#include "reverb_setters.h"

bool consoleBusy = false;

uint16_t getDataPartFromStream(TTranslator *rl, char *buf, int maxSize)
{
	kgp_sdk_libc::memset(buf, 0, maxSize);
	int streamPos = 0;
	do
	{
		int c;
		rl->RecvChar(c);
		if(c == '\r' || c == '\n')
		{
			return streamPos;
		}
		buf[streamPos++] = c;
	} while(streamPos < maxSize);

	return streamPos;
}

static void amtdev_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r%02x\n", args[0], AMT_DEV_ID);
}

static void amtver_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r%s\n", args[0], amt_ver);
}

static void ls_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	FileSystemTask->Suspend();
	console_printf("%s\r", args[0]);

	for(auto it = fileBrowser->List().begin(); it != fileBrowser->List().end(); ++it)
	{
		console_printf("%d:%s|", (*it).type, (*it).name.c_str());
		vTaskDelay(pdMS_TO_TICKS(1)); // protect console overflow
	}
	console_printf("\n");
	FileSystemTask->Resume();
}

static void cd_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	FileSystemTask->Suspend();

	consoleBusy = true;
	char folderName[256];
	getDataPartFromStream(rl, folderName, 256);
	consoleBusy = false;

	fs_object_t fsObject;
	fsObject.name = folderName;
	fileBrowser->SelectDir(fsObject);

	msg_console("%s\r%s\n", args[0], fileBrowser->CurrDir(false).c_str());
	FileSystemTask->Resume();
}

uint16_t bufPos;
static void ir_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	uint8_t cabNum = 0;

	if(count > 1)
	{
		char *end;
		cabNum = kgp_sdk_libc::strtol(args[1], &end, 16);
//		msg_console("%d", cabNum);
	}

	if(count == 2)
	{
		if(cabNum==0)
		{
			msg_console("%s %d\r%s\n", args[0], cabNum, currentPreset.cab1Name);
		}
		else
		{
			msg_console("%s %d\r%s\n", args[0], cabNum, currentPreset.cab2Name);
		}
	}

	if(count == 3 || count == 4)
	{
		std::emb_string command = args[2];

		if(command == "set")
		{
			consoleBusy = true;
			char fileName[CAB_NAME_STRING_SIZE - 1];
			kgp_sdk_libc::memset(fileName, 0, CAB_NAME_STRING_SIZE - 1);

			getDataPartFromStream(rl, fileName, CAB_NAME_STRING_SIZE - 1);
			consoleBusy = false;

			fs_object_t fsObject;
			fsObject.name = fileName;
			fileBrowser->SelectFile(fsObject); //not thread safe
			FileSystemTask->SendCommand(TFsBrowser::bcAction);

			TFileSystemTask::TResponse browserResponse;
			browserResponse = FileSystemTask->GetResponseBlocking();

			if(browserResponse.responseType == TFileSystemTask::TResponseType::rpFileSelected)
				UITask->setParam(ir_cab_setter, cabNum);
			else
				msg_console("%s error\r\n", args[0]);
		}

		if(command == "preview_start")
		{
			bufPos = 0;
			kgp_sdk_libc::memset(tempCabBuffer, 0, CAB_DATA_SIZE * 2);
			msg_console("%s %d request_part\r\n", args[0], cabNum);
		}

		if(command == "preview_part")
		{
			if (count > 3)
			{
				char buffer[512];
				char *pEnd;
				int32_t streamPos = 0;
				int32_t bytesToRecieve = kgp_sdk_libc::strtol(args[3], &pEnd, 10);
				int c;
				do
				{
					rl->RecvChar(c);
					buffer[streamPos++] = c;
				} while (streamPos < bytesToRecieve);
				rl->RecvChar(c); // get last \n

				kgp_sdk_libc::memcpy(tempCabBuffer + bufPos, buffer, bytesToRecieve);
				bufPos += bytesToRecieve;
				msg_console("%s %d request_part\r\n", args[0], cabNum);
			}
			else
			{
				msg_console("error\rPART_SIZE_INCORRECT\n");
			}
		}

		if(command == "preview_end")
		{
			if(cabNum==0)
			{
				SharcTask->sendCab1Data(&tempCabBuffer[0], &tempCabBuffer[CAB_DATA_SIZE]);
				SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
			}
			else
			{
				SharcTask->sendCab2Data(tempCabBuffer);
				SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
			}
			msg_console("%s %d preview_end\r\n", args[0], cabNum);
		}

		if(command == "restore")
		{
			UITask->setParam(ir_cab_restore, cabNum);
		}
	}
}

static void mkdir_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	FileSystemTask->Suspend();
	msg_console("%s\r", args[0]);

	consoleBusy = true;
	char dirName[64];
	kgp_sdk_libc::memset(dirName, 0, 64);

	getDataPartFromStream(rl, dirName, 64);
	consoleBusy = false;
	msg_console("%s\n", dirName);

	fs_object_t fsObject;
	fsObject.name = dirName;
	fileBrowser->CreateDir(fsObject);

	FileSystemTask->Resume();
}

static void remove_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	FileSystemTask->Suspend();
	msg_console("%s\r", args[0]);

	consoleBusy = true;
	char objName[64];
	kgp_sdk_libc::memset(objName, 0, 64);

	getDataPartFromStream(rl, objName, 64);
	consoleBusy = false;
	msg_console("%s\n", objName);

	fs_object_t fsObject;
	fsObject.name = objName;

	fileBrowser->RemoveObject(fsObject);
	FileSystemTask->Resume();
}

static void rename_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	FileSystemTask->Suspend();
	msg_console("%s\r", args[0]);

	consoleBusy = true;
	char srcName[64];
	kgp_sdk_libc::memset(srcName, 0, 64);
	getDataPartFromStream(rl, srcName, 64);

	fs_object_t srcObject;
	srcObject.name = srcName;

	char dstName[64];
	kgp_sdk_libc::memset(dstName, 0, 64);
	getDataPartFromStream(rl, dstName, 64);
	consoleBusy = false;

	fs_object_t dstObject;
	dstObject.name = dstName;

	fileBrowser->RenameObject(srcObject, dstObject);

	msg_console("%s\r%s\n", srcName, dstName);
	FileSystemTask->Resume();
}

static void copyto_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	char *end;
	uint8_t presetNum = kgp_sdk_libc::strtol(args[1], &end, 16);

	consoleBusy = true;
	char selectionMaskArray[sizeof(Preset::TSelectionMask)];
	getDataPartFromStream(rl, selectionMaskArray, sizeof(Preset::TSelectionMask));
	consoleBusy = false;

	msg_console("%s\r\n", args[0]);

	for(uint8_t i=0; i < sizeof(Preset::TSelectionMask); i++)
	{
		selectionMaskArray[i] -= '0';
	}
	Preset::TSelectionMask selectionMask;
	kgp_sdk_libc::memcpy(&selectionMask, selectionMaskArray, sizeof(Preset::TSelectionMask));

	Preset::Copy(presetNum, selectionMask);
}

static void upload_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s ", args[0]);
	if(count < 2)
	{
		msg_console("\r\n");
		return;
	}

	std::emb_string command = args[1];

	if(command == "start")
	{
		FileSystemTask->Suspend();

		char buffer[128];
		getDataPartFromStream(rl, buffer, 128);
		fs_object_t fsObject;
		fsObject.name = buffer;

		if (fileBrowser->CreateFile(fsObject))
		{
			msg_console("request_part\r\n");
		} else {
			msg_console("error\rCANNOT_CREATE_FILE\n");
		}

		FileSystemTask->Resume();
	}

	if (command == "part")
	{
		if (count > 2)
		{
			FileSystemTask->Suspend();

			char buffer[512];
			char *pEnd;
			int32_t streamPos = 0;
			int32_t bytesToRecieve = kgp_sdk_libc::strtol(args[2], &pEnd, 10);
			int c;
			do
			{
				rl->RecvChar(c);
				buffer[streamPos++] = c;
			} while (streamPos < bytesToRecieve);
			rl->RecvChar(c); // get last \n

			if (fileBrowser->AppendDataToFile(buffer, bytesToRecieve))
			{
				msg_console("request_part\r\n");
			}

			FileSystemTask->Resume();
		}
		else
		{
			msg_console("error\rPART_SIZE_INCORRECT\n");
		}
	}
}

static void plist_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s", args[0]);
	for (int p = 0; p < 99; p++)
	{
		Preset::TPresetBrief presetData;
		EEPROM_LoadPresetBrief(p, &presetData);

		char *cab1NameSrc = presetData.cab1Name + 1;
		char *cab2NameSrc = presetData.cab2Name + 1;
		char cab1NameDst[64];
		char cab2NameDst[64];

		kgp_sdk_libc::memset(cab1NameDst, 0, 64);
		kgp_sdk_libc::memset(cab2NameDst, 0, 64);

		uint8_t pos=0;

		while(*cab1NameSrc)
		{
			if(*cab1NameSrc != '|' && *cab1NameSrc != '\r' && *cab1NameSrc != '\n')
			{
				cab1NameDst[pos] = *cab1NameSrc;
				pos++;
			}
			cab1NameSrc++;
		}

		pos=0;
		while(*cab2NameSrc)
		{
			if(*cab2NameSrc != '|' && *cab2NameSrc != '\r' && *cab2NameSrc != '\n')
			{
				cab2NameDst[pos] = *cab1NameSrc;
				pos++;
			}
			cab2NameSrc++;
		}

		msg_console("\r%d|%s|%s|%s|%s|", p, presetData.name, presetData.comment, cab1NameDst, cab2NameDst);

		uint8_t enabled[14];
		kgp_sdk_libc::memcpy(enabled, &presetData.paramData.switches, 14);
		for(uint8_t i=0; i<14; i++) msg_console("%d", enabled[i]);
	}
	msg_console("\n");
}

static void state_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	currentPreset.paramData.delay_time = currentPreset.delayTime;

	msg_console("%s\r", args[0]);
	for(size_t i = 0; i < 512; i++)
	{
		msg_console("%02x", currentPreset.modulesBuf[i]);
	}
	msg_console("\n");
}

static void cntrls_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	for(size_t i = 0; i < Controller::controllersCount; i++)
	{
		msg_console("%02x%02x%02x%02x", currentPreset.controller[i].src, currentPreset.controller[i].dst,
				currentPreset.controller[i].minVal, currentPreset.controller[i].maxVal);
	}
	msg_console("\n");
}

static void pname_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if(count > 1)
	{
		std::emb_string command = args[1];

		if(command == "set")
		{
			consoleBusy = true;
			getDataPartFromStream(rl, (char*)currentPreset.name, 15);
			consoleBusy = false;
		}
	}
	msg_console("%s\n", currentPreset.name);
}

static void pcomment_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if(count > 1)
	{
		std::emb_string command = args[1];

		if(command == "set")
		{
			consoleBusy = true;
			getDataPartFromStream(rl, (char*)currentPreset.comment, 15);
			consoleBusy = false;
		}
	}
	msg_console("%s\n", currentPreset.comment);
}

static void controller_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\r", args[0]);
	if (count > 3)
	{
		char *end;
		uint8_t cntrlNum = kgp_sdk_libc::strtol(args[1], &end, 16);
		std::emb_string command = args[2];
		uint8_t value = kgp_sdk_libc::strtol(args[3], &end, 16);

		msg_console("%d\r%s\r%d", cntrlNum, args[2], value);

		if(command == "dst")
		{
			currentPreset.controller[cntrlNum].dst = value;
			goto ending;
		}

		if(command == "src")
		{
			currentPreset.controller[cntrlNum].src = value;
			goto ending;
		}

		if(command == "min")
		{
			currentPreset.controller[cntrlNum].minVal = value;
			goto ending;
		}

		if(command == "max")
		{
			currentPreset.controller[cntrlNum].maxVal = value;
			goto ending;
		}
	}
	else
	{
		msg_console("INCORRECT_ARGS");
	}

ending:
	msg_console("\n");
}

static void controller_pc_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	default_param_handler(&currentPreset.pcOut, args, count);
}

static void controller_set_command_handler(TTranslator* rl, TTranslator::const_symbol_type_ptr_t* args, const size_t count)
{
	uint8_t answer;

	if (count > 0)
	{
		if(count > 1)
		{
			char *end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			currentPreset.set = val;
			currentPreset.set |= 0x80;
			answer = val;
		}
		else
		{
			if(currentPreset.set & 0x80) answer = currentPreset.set & 0x7F;
			else answer = currentPresetNumber;
		}
		msg_console("%s\r%02x\n", args[0], answer);
	}
}

static void tuner_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console("%s ", args[0]);
	if(count == 2)
	{
		msg_console("%s\r", args[1]);

		std::emb_string command = args[1];

		if(command == "ref")
		{
			uint16_t refFreq = TunerTask->refFreq;
			msg_console("%d", refFreq);
		}
	}

	if(count == 3)
	{
		msg_console("%s\r", args[1]);
		std::emb_string command = args[1];
		if(command == "ref")
		{
			char *end;
			uint16_t refFreq = kgp_sdk_libc::strtol(args[2], &end, 10);
			TunerTask->refFreq = refFreq;
			msg_console("%d", refFreq);
		}

		if(command == "enable")
		{
			char *end;
			uint8_t on = kgp_sdk_libc::strtol(args[2], &end, 16);

			if(on)
			{
				TunerTask->Enable(TTunerTask::TUNER_CONSOLE);
			}
			else
			{
				TunerTask->Disable();
			}
			msg_console("%d", on);
		}

		if(command == "get")
		{
			char *end;
			uint16_t count = kgp_sdk_libc::strtol(args[2], &end, 16);
			TunerTask->Enable(TTunerTask::TUNER_CONSOLE, count);

		}
	}
	msg_console("\n");
}


//------------------------------------------------------------------------------
void ConsoleSetCmdHandlers(TTranslator *translator)
{
	set_hardware_handlers(translator);

	translator->AddCommandHandler("amtdev", amtdev_command_handler);
	translator->AddCommandHandler("amtver", amtver_command_handler);

	translator->AddCommandHandler("cd", cd_command_handler);
	translator->AddCommandHandler("ls", ls_command_handler);
	translator->AddCommandHandler("ir", ir_command_handler);

	translator->AddCommandHandler("upload", upload_command_handler);

	translator->AddCommandHandler("mkdir", mkdir_command_handler);
	translator->AddCommandHandler("rename", rename_command_handler);
	translator->AddCommandHandler("remove", remove_command_handler);

	translator->AddCommandHandler("copy_to", copyto_command_handler);


	translator->AddCommandHandler("state", state_command_handler);
	translator->AddCommandHandler("cntrls", cntrls_command_handler);

	translator->AddCommandHandler("plist", plist_command_handler);

	translator->AddCommandHandler("pname", pname_command_handler);
	translator->AddCommandHandler("pcomment", pcomment_command_handler);

	translator->AddCommandHandler("cntrl", controller_command_handler);
	translator->AddCommandHandler("cntrl_pc", controller_pc_command_handler);
	translator->AddCommandHandler("cntrl_set", controller_set_command_handler);

	translator->AddCommandHandler("tn", tuner_command_handler);

	set_syssettings_handlers(translator);

	set_preset_handlers(translator);

	set_resonance_filter_handlers(translator);
	set_gate_handlers(translator);
	set_compressor_handlers(translator);
	set_preamp_handlers(translator);
	set_amp_handlers(translator);
	set_ir_handlers(translator);
	set_eq_handlers(translator);
	set_phaser_handlers(translator);
	set_flanger_handlers(translator);
	set_chorus_handlers(translator);
	set_delay_handlers(translator);
	set_early_handlers(translator);
	set_reverb_handlers(translator);
	set_tremolo_handlers(translator);
}

//------------------------------------------------------------------------------
