#define __KLIBC_WRAPS_IMPL__

#include "appdefs.h"
#include "sdk_port.h"

#include "tasks/usb_task.h"
#include "tasks/display_task.h"
#include "tasks/filesystem_task.h"
#include "tasks/ui_task.h"
#include "tasks/controllers_task.h"
#include "tasks/sdtest_task.h"
#include "tasks/io_task.h"

#include "tasks/spectrum_task.h"
#include "tasks/midi_task.h"

#include "init.h"

EventGroupHandle_t startEventGroup;

int main(void)
{
	sysclock = GetCpuClock();

	init();

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

