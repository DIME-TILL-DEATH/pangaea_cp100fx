
#define __KLIBC_WRAPS_IMPL__

#include "appdefs.h"
#include "sdk_port.h"

#include "eepr.h"

#include "periphery.h"
#include "AT45DB321.h"
#include "lcd.h"
#include "serial.h"
#include "gpio.h"
#include "hw_timers.h"
#include "sharc.h"
#include "codec.h"

#include "bitmaps.h"
#include "param_bitmap.h"

#include "system.h"

#include "filesystem_task.h"
#include "display_task.h"
#include "io_task.h"
#include "ui_task.h"
#include <tuner_task.h>
#include "controllers_task.h"
#include "sdtest_task.h"
#include "midi_task.h"
#include "sharc_task.h"


EventGroupHandle_t startEventGroup;

int main(void)
{
	sysclock = GetCpuClock();

	LCD_Init();
	display_start(0);

	AT45DB321_Init();
	HW_Delay(0xfffff);

	EEPROM_Start();

	HW_GpioInit();
	HW_ExtiInit();

	HW_I2sInit();
	HW_UartInit();

	HW_TimersInit();

	SHARC_SpiInit(TSharcSpiMode::SPI_SLAVE);
	SHARC_StartupLoad();
	SHARC_SpiInit(TSharcSpiMode::SPI_MASTER);

	CODEC_Start();

	display_start(1);
	SHARC_LoadAllData();

	HW_PinUsbInit();

	System::setStartupValues();

	startEventGroup = xEventGroupCreate();

	FileSystemTask = new TFileSystemTask();
	FileSystemTask->Create("FS", 40*configMINIMAL_STACK_SIZE, 0);

	DisplayTask = new TDisplayTask();
	DisplayTask->Create("DISP", 60*configMINIMAL_STACK_SIZE, 2);

	IOTask = new TIOTask();
	IOTask->Create("ENC", 20*configMINIMAL_STACK_SIZE, 3);

	UITask = new TUITask();
	UITask->Create("CS", 30*configMINIMAL_STACK_SIZE, 3);

	TunerTask = new TTunerTask();
	TunerTask->Create("STR", 20*configMINIMAL_STACK_SIZE, 3);

	//ControllerTask lower priority than UITask, becourse in UITask preset switching. No one can interrupt it
	ControllersTask = new TControllersTask();
	ControllersTask->Create("CC", 10*configMINIMAL_STACK_SIZE, 0);

	SDTestTask = new TSDTestTask();
	SDTestTask->Create("SD_TEST", configMINIMAL_STACK_SIZE, 0);

	MidiTask = new TMidiTask();
	MidiTask->Create("MidiSend", 10*configMINIMAL_STACK_SIZE, 5);

	SharcTask = new TSharcTask();
	SharcTask->Create("ShaarcSendData", 10*configMINIMAL_STACK_SIZE, 0);

	TScheduler::StartScheduler();
}
//---------------------------------------------------------------------------

