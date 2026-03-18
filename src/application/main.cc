#define __KLIBC_WRAPS_IMPL__

#include "appdefs.h"
#include "sdk_port.h"

#include "eepr.h"

#include "periphery.h"
#include "AT45DB321.h"
#include "ER_OLEDM023-1B.h"
#include "serial.h"
#include "gpio.h"
#include "hw_timers.h"
#include "sharc.h"
#include "codec.h"

#include "allFonts.h"
#include "par_bitmap.h"

#include "system.h"

#include "filesystem_task.h"
#include "display_task.h"
#include "io_task.h"
#include "ui_task.h"
#include "spectrum_task.h"
#include "controllers_task.h"
#include "sdtest_task.h"
#include "midi_task.h"


EventGroupHandle_t startEventGroup;

int main(void)
{
	sysclock = GetCpuClock();

	oled023_1_disp_init();
	disp_start(0);

	AT45DB321_Init();
	HW_delay(0xfffff);

	EEPROM_start();

	HW_gpio_init();
	HW_exti_init();

	HW_i2s_init();
	HW_uart_init();

	HW_timers_init();

	SHARC_spi_init(TSharcSpiMode::SPI_SLAVE);
	SHARC_startup_load();
	SHARC_spi_init(TSharcSpiMode::SPI_MASTER);

	CODEC_start();

	disp_start(1);
	EEPROM_load_all_ir();

	HW_pin_usb_init();

	startEventGroup = xEventGroupCreate();

	FileSystemTask = new TFileSystemTask();
	FileSystemTask->Create("FS", 40*configMINIMAL_STACK_SIZE, 0);

	DisplayTask = new TDisplayTask();
	DisplayTask->Create("DISP", 60*configMINIMAL_STACK_SIZE, 0);

	IOTask = new TIOTask();
	IOTask->Create("ENC", 20*configMINIMAL_STACK_SIZE, 0);

	UITask = new TUITask();
	UITask->Create("CS", 30*configMINIMAL_STACK_SIZE, 0);

	SpectrumTask = new TSpectrumTask();
	SpectrumTask->Create("STR", 20*configMINIMAL_STACK_SIZE, 0);

	ControllersTask = new TControllersTask();
	ControllersTask->Create("CC", 10*configMINIMAL_STACK_SIZE, 0);

	SDTestTask = new TSDTestTask();
	SDTestTask->Create("SD_TEST", configMINIMAL_STACK_SIZE, 0);

	MidiTask = new TMidiTask();
	MidiTask->Create("MidiSend", 10*configMINIMAL_STACK_SIZE, 1);

	TScheduler::StartScheduler();
}
//---------------------------------------------------------------------------

