
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
#include "tuner_task.h"
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
	FileSystemTask->Create("FileSystem", 30*configMINIMAL_STACK_SIZE, 0);

	DisplayTask = new TDisplayTask();
	DisplayTask->Create("Display", 40*configMINIMAL_STACK_SIZE, 0);

	IOTask = new TIOTask();
	IOTask->Create("InOut", 20*configMINIMAL_STACK_SIZE, 0);

	UITask = new TUITask();
	UITask->Create("UserInterface", 20*configMINIMAL_STACK_SIZE, 0);

	TunerTask = new TTunerTask();
	TunerTask->Create("Tuner", 20*configMINIMAL_STACK_SIZE, 0);

	ControllersTask = new TControllersTask();
	ControllersTask->Create("Controllers", 10*configMINIMAL_STACK_SIZE, 0);

	SDTestTask = new TSDTestTask();
	SDTestTask->Create("SD_TEST", configMINIMAL_STACK_SIZE, 0);

	MidiTask = new TMidiTask();
	MidiTask->Create("MidiSend", 10*configMINIMAL_STACK_SIZE, 1);

	SharcTask = new TSharcTask();
	SharcTask->Create("SharcSendData", 5*configMINIMAL_STACK_SIZE, 0);

	ConsoleTask = new TConsoleTask(256);
	ConsoleTask->Create("CONS", 20*configMINIMAL_STACK_SIZE, 0);

	TScheduler::StartScheduler();
}
//---------------------------------------------------------------------------

