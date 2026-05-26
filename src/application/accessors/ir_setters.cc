#include "ir_setters.h"

#include "console_helpers.h"

#include "system.h"
#include "modules.h"
#include "preset.h"

#include "sharc_task.h"
#include "filesystem_task.h"

static TParamDescriptor onParamDesc = {
	.ptr = &currentPreset.paramData.switches.cab,
	.handlerStr = "ir_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_CAB,
	.name = "On",
	.setterHandler = ir_on_setter
};

static TParamDescriptor vol1ParamDesc = {
	.ptr = &currentPreset.paramData.cab1.volume,
	.handlerStr = "ir_v1",
	.dspAddress = DSP_ADDRESS_CAB,
	.dspPosition = IR_VOLUME1_POS,
	.name = "Volume",
	.setterHandler = ir_volume1_setter
};


static TParamDescriptor pan1ParamDesc = {
	.ptr = &currentPreset.paramData.cab1.pan,
	.handlerStr = "ir_p1",
	.dspAddress = DSP_ADDRESS_CAB,
	.dspPosition = IR_PAN1_POS,
	.name = "Pan",
	.setterHandler = ir_pan1_setter
};

static TParamDescriptor vol2ParamDesc = {
	.ptr = &currentPreset.paramData.cab2.volume,
	.handlerStr = "ir_v2",
	.dspAddress = DSP_ADDRESS_CAB,
	.dspPosition = IR_VOLUME2_POS,
	.name = "Volume",
	.setterHandler = ir_volume2_setter
};


static TParamDescriptor pan2ParamDesc = {
	.ptr = &currentPreset.paramData.cab2.pan,
	.handlerStr = "ir_p2",
	.dspAddress = DSP_ADDRESS_CAB,
	.dspPosition = IR_PAN2_POS,
	.name = "Pan",
	.setterHandler = ir_pan2_setter
};

static TParamDescriptor dummyDesc = {
	.ptr = nullptr,
	.handlerStr = "",
	.dspAddress = DSP_ADDRESS_CAB,
	.dspPosition = NOT_SEND_POS,
	.name = "",
	.setterHandler = nullptr
};

TIrDesc IrDesc = {
	.on = onParamDesc,
	.vol1 = vol1ParamDesc,
	.pan1 = pan1ParamDesc,
	.vol2 = vol2ParamDesc,
	.pan2 = pan2ParamDesc,
	.dummy = dummyDesc
};

void ir_on_setter(uint32_t value)
{
	default_param_setter(onParamDesc, value);
}

void ir_volume1_setter(uint32_t value)
{
	default_param_setter(vol1ParamDesc, value);
}

void ir_pan1_setter(uint32_t value)
{
	default_param_setter(pan1ParamDesc, value);
}

void ir_volume2_setter(uint32_t value)
{
	default_param_setter(vol2ParamDesc, value);
}

void ir_pan2_setter(uint32_t value)
{
	default_param_setter(pan2ParamDesc, value);
}

void ir_cab_setter(uint32_t value)
{
	uint8_t cabNum = value;

	// file preselected in browser
	fs_object_t currentObject = FileSystemTask->Object();

	if(cabNum==0)
	{
		kgp_sdk_libc::memcpy(currentPreset.cab1Data, tempCabBuffer, CAB_DATA_SIZE);

		kgp_sdk_libc::memcpy(&currentPreset.cab1Name, currentObject.name.c_str(), CAB_NAME_STRING_SIZE - 1);
		currentPreset.cab1NameSize = kgp_sdk_libc::strlen((const char*)currentPreset.cab1Name);

#ifdef __MONO_MOD__
		if(System::cab_type != CAB_CONFIG_STEREO)
		{
			kgp_sdk_libc::memcpy(currentPreset.cabAuxData, tempCabBuffer + CAB_DATA_SIZE, CAB_DATA_SIZE);
		}
		SharcTask->sendCab1Data(currentPreset.cab1Data, currentPreset.cabAuxData);
#endif

#ifdef __STEREO_MOD__
		SharcTask->sendCab1Data(currentPreset.cab1Data, nullptr);
#endif

		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);
	}
	else
	{
		kgp_sdk_libc::memcpy(currentPreset.cab2Data, tempCabBuffer, CAB_DATA_SIZE);
		kgp_sdk_libc::memcpy(&currentPreset.cab2Name, currentObject.name.c_str(), CAB_NAME_STRING_SIZE - 1);
		currentPreset.cab2NameSize = kgp_sdk_libc::strlen((const char*)currentPreset.cab2Name);

		SharcTask->sendCab2Data(currentPreset.cab2Data);
		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
	}

	console_printf("ir %d set\r%s\n", cabNum, currentObject.name.c_str());
}

void ir_cab_restore(uint32_t value)
{
	uint8_t cabNum = value;

	if(cabNum == 0)
	{
#ifdef __MONO_MOD__
		SharcTask->sendCab1Data(currentPreset.cab1Data, currentPreset.cabAuxData);
#endif
#ifdef __STEREO_MOD__
		SharcTask->sendCab1Data(currentPreset.cab1Data, nullptr);
#endif
		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME1_POS, currentPreset.modulesBuf[IR_VOLUME1]);

		if(currentPreset.cab1NameSize == 0)
		{
			currentPreset.paramData.switches.cab = 0;
			SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_CAB, currentPreset.paramData.switches.cab);
		}
	}
	else
	{
		SharcTask->sendCab2Data(currentPreset.cab2Data);
		SharcTask->setParameter(DSP_ADDRESS_CAB, IR_VOLUME2_POS, currentPreset.modulesBuf[IR_VOLUME2]);
	}

	console_printf("ir %d restore\r\n", cabNum);
}

void set_ir_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler("ir_on", ir_on_setter);

	translator->AddAccessorHandler("ir_v1", ir_volume1_setter);
	translator->AddAccessorHandler("ir_p1", ir_pan1_setter);

	translator->AddAccessorHandler("ir_v2", ir_volume2_setter);
	translator->AddAccessorHandler("ir_p2", ir_pan2_setter);
}
