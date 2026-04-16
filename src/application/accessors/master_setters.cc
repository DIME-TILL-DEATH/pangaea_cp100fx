#include "master_setters.h"

#include "console_helpers.h"
#include "eq_setters.h"

#include "eeprom.h"
#include "system.h"
#include "modules.h"

#include "sharc_task.h"
#include "io_task.h"
#include "display_task.h"

static TParamDescriptor attVolParamDesc = {
	.ptr = &sys_para[System::ATTENUATOR],
	.handlerStr = "vl_at",
	.dspAddress = DSP_ADDRESS_PRESET_FULL,
	.dspPosition = NOT_SEND_POS,
	.name = "Attenuator",
	.setterHandler = attenuator_vol_setter
};

static TParamDescriptor attSourceParamDesc = {
	.ptr = &sys_para[System::ATTENUATOR_MODE],
	.handlerStr = "vl_at_mode",
	.dspAddress = DSP_ADDRESS_PRESET_FULL,
	.dspPosition = NOT_SEND_POS,
	.name = "Att. source",
	.setterHandler = attenuator_source_setter
};

TAttenuatorDesc AttenuatorDesc = {
	.volume = attVolParamDesc,
	.source = attSourceParamDesc
};

void attenuator_vol_setter(uint32_t value)
{
	uint8_t* value_ptr = sys_para[System::ATTENUATOR_MODE] ? &currentPreset.paramData.attenuator
															: &sys_para[System::ATTENUATOR];

	*value_ptr = value;
	DisplayTask->potWrite();
	console_printf("%s\r%02x\n", attVolParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void attenuator_source_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)attSourceParamDesc.ptr;
	*value_ptr = value;
	DisplayTask->potWrite();
	console_printf("%s\r%02x\n", attSourceParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

//=======================================Master volume=============================================================
static TParamDescriptor masterVolParamDesc = {
	.ptr = &sys_para[System::MASTER_VOLUME],
	.handlerStr = "vl_ms",
	.dspAddress = DSP_ADDRESS_MASTER,
	.dspPosition = PARAM_EQUAL_POS,
	.name = "Master volume",
	.setterHandler = master_volume_setter
};

static TParamDescriptor phonesVolParamDesc = {
	.ptr = &sys_para[System::PHONES_VOLUME],
	.handlerStr = "vl_ph",
	.dspAddress = DSP_ADDRESS_MASTER,
	.dspPosition = NOT_SEND_POS,
	.name = "Phones volume",
	.setterHandler = phones_volume_setter
};

TMasterVolDesc MasterVolDesc = {
	 .master = masterVolParamDesc,
	 .phones = phonesVolParamDesc
};

void master_volume_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)masterVolParamDesc.ptr;
	*value_ptr = value;
	SharcTask->setParameter(DSP_ADDRESS_MASTER, *value_ptr, 0);
	console_printf("%s\r%02x\n", masterVolParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void phones_volume_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)phonesVolParamDesc.ptr;
	*value_ptr = value;
	DisplayTask->potWrite();
	console_printf("%s\r%02x\n", phonesVolParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

//=====================================Master EQ===================================================================
static TParamDescriptor meqOnParamDesc = {
	.ptr = &sys_para[System::MASTER_EQ_ON],
	.handlerStr = "meq_on",
	.dspAddress = DSP_ADDRESS_MODULES_ENABLE,
	.dspPosition = ENABLE_MASTER_EQ,
	.name = "Master EQ",
	.setterHandler = meq_on_setter
};

static TParamDescriptor meqLgParamDesc = {
	.ptr = &sys_para[System::MASTER_EQ_LOW],
	.handlerStr = "meq_lg",
	.dspAddress = DSP_ADDRESS_EQ,
	.dspPosition = EQ_MASTER_LOW_GAIN_POS,
	.name = "Low",
	.setterHandler = meq_low_setter
};

static TParamDescriptor meqMgParamDesc = {
	.ptr = &sys_para[System::MASTER_EQ_MID],
	.handlerStr = "meq_mg",
	.dspAddress = DSP_ADDRESS_EQ,
	.dspPosition = EQ_MASTER_MID_GAIN_POS,
	.name = "Mid",
	.setterHandler = meq_mid_setter
};

static TParamDescriptor meqHgParamDesc = {
	.ptr = &sys_para[System::MASTER_EQ_HIGH],
	.handlerStr = "meq_hg",
	.dspAddress = DSP_ADDRESS_EQ,
	.dspPosition = EQ_MASTER_HIGH_GAIN_POS,
	.name = "High",
	.setterHandler = meq_high_setter
};

static TParamDescriptor meqMfParamDesc = {
	.ptr = &sys_para[System::MASTER_EQ_FREQ_LO],
	.handlerStr = "meq_mf",
	.dspAddress = DSP_ADDRESS_EQ,
	.dspPosition = EQ_MASTER_MID_FREQ_POS,
	.name = "Mid Freq",
	.setterHandler = meq_mid_freq_setter
};

TMasterEqDesc MasterEqDesc = {
	.on = meqOnParamDesc,
	.low = meqLgParamDesc,
	.mid = meqMgParamDesc,
	.midFreq = meqMfParamDesc,
	.high = meqHgParamDesc
};

void meq_on_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)meqOnParamDesc.ptr;
	*value_ptr = value;
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, ENABLE_MASTER_EQ, *value_ptr);
	console_printf("%s\r%02x\n", meqOnParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void meq_low_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)meqLgParamDesc.ptr;
	*value_ptr = value;
	SharcTask->setParameter(meqLgParamDesc.dspAddress, meqLgParamDesc.dspPosition, *value_ptr + 24);
	console_printf("%s\r%02x\n", meqLgParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void meq_mid_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)meqMgParamDesc.ptr;
	*value_ptr = value;
	SharcTask->setParameter(meqMgParamDesc.dspAddress, meqMgParamDesc.dspPosition, *value_ptr + 24);
	console_printf("%s\r%02x\n", meqMgParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void meq_high_setter(uint32_t value)
{
	uint8_t* value_ptr = (uint8_t*)meqHgParamDesc.ptr;
	*value_ptr = value;
	SharcTask->setParameter(meqHgParamDesc.dspAddress, meqHgParamDesc.dspPosition, *value_ptr + 24);
	console_printf("%s\r%02x\n", meqHgParamDesc.handlerStr, *value_ptr);

	EEPROM_DelayedSaveSystemData();
}

void meq_mid_freq_setter(uint32_t value)
{
	mstEqMidFreq = value;

	sys_para[System::MASTER_EQ_FREQ_LO] = mstEqMidFreq >> 8;
	sys_para[System::MASTER_EQ_FREQ_HI] = mstEqMidFreq & 0xFF;

	SharcTask->setParameter(meqMfParamDesc.dspAddress, meqMfParamDesc.dspPosition, sys_para[System::MASTER_EQ_FREQ_LO]);
	SharcTask->setParameter(meqMfParamDesc.dspAddress, meqMfParamDesc.dspPosition, sys_para[System::MASTER_EQ_FREQ_HI]);

	console_printf("%s\r%04x\n", meqMfParamDesc.handlerStr, mstEqMidFreq);

	EEPROM_DelayedSaveSystemData();
}


void set_master_handlers(TTranslator *translator)
{
	translator->AddAccessorHandler("vl_at", attenuator_vol_setter);
	translator->AddAccessorHandler("vl_at_mode", attenuator_source_setter);

	translator->AddAccessorHandler("vl_ms", master_volume_setter);
	translator->AddAccessorHandler("vl_ph", phones_volume_setter);

	translator->AddAccessorHandler("meq_on", meq_on_setter);
	translator->AddAccessorHandler("meq_lg", meq_low_setter);
	translator->AddAccessorHandler("meq_mg", meq_mid_setter);
	translator->AddAccessorHandler("meq_hg", meq_high_setter);
	translator->AddAccessorHandler("meq_mf", meq_mid_freq_setter);
}
