#define __KLIBC_WRAPS_IMPL__

#include "appdefs.h"
#include "debug_led.h"
#include "mmgr.h"
#include "usb.h"
#include "display.h"
#include "storage.h"
#include "fs.h"
#include "cs.h"
#include "cc.h"
#include "sd_test.h"
#include "enc.h"
#include "errno.h"
#include "spectrum.h"
#include "midi_send.h"

#include "usb.h"

extern void (*__preinit_array_start__[])(void);
extern void (*__preinit_array_end__[])(void);
extern void (*__init_array_start__[])(void);
extern void (*__init_array_end__[])(void);

extern void (*__fini_array_start__[])(void);
extern void (*__fini_array_end__[])(void);

extern "C" void _init(void);
extern "C" void _fini(void);
extern "C" void _premain(void);

extern "C" void __libc_init_array(void)
{
	size_t count;
	size_t i;

	count = __preinit_array_end__-__preinit_array_start__;
	for(i = 0; i<count; i++)
		if(__preinit_array_start__[i])
			__preinit_array_start__[i]();

	_init();

	count = __init_array_end__-__init_array_start__;
	for(i = 0; i<count; i++)
		if(__init_array_start__[i])
			__init_array_start__[i]();

}

/* Run all the cleanup routines.  */
extern "C" void __libc_fini_array(void)
{
	size_t count;
	size_t i;

	count = __fini_array_end__-__fini_array_start__;
	for(i = count; i>0; i--)
		__fini_array_start__[i-1]();

	_fini();
}

volatile uint32_t sysclock;
extern "C" unsigned long GetCpuClock(void)
{
	RCC_ClocksTypeDef rrc;
	RCC_GetClocksFreq(&rrc);
	return rrc.SYSCLK_Frequency;
}

extern "C" void _init(void)
{
	heap_init();
	sysclock = GetCpuClock();
	*(__errno()) = 0;
}
volatile char *task_name;
extern "C" void vApplicationStackOverflowHook(TaskHandle_t *pxTask, char *pcTaskName)
{
	task_name = (volatile char*)pcTaskName;
	//err ( "task stack overflow: %s 0x%x\n" , pcTaskName , pxTask ) ;
	while(1)
		NOP();
}

extern "C" void vApplicationTickHook()
{
	NOP();
	(void)task_name;
}

volatile uint32_t stack;
extern "C" void vApplicationIdleHook()
{
	NOP();
}

void init(void);

void pin_usb_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

int main(void)
{
	init();
	pin_usb_init();
	sysclock = GetCpuClock();

	FSTask = new TFSTask();
	FSTask->Create("FS", 40*configMINIMAL_STACK_SIZE, 0);

	DisplayTask = new TDisplayTask();
	DisplayTask->Create("DISP", 60*configMINIMAL_STACK_SIZE, 0);

	ENCTask = new TENCTask();
	ENCTask->Create("ENC", 20*configMINIMAL_STACK_SIZE, 0);

	CSTask = new TCSTask();
	CSTask->Create("CS", 40*configMINIMAL_STACK_SIZE, 0);

	SpectrumTask = new TSpectrumTask();
	SpectrumTask->Create("STR", 20*configMINIMAL_STACK_SIZE, 0);


	CCTask = new TCCTask();
	CCTask->Create("CC", 10*configMINIMAL_STACK_SIZE, 0);

	SD_TESTTask = new TSD_TESTTask();
	SD_TESTTask->Create("SD_TEST", configMINIMAL_STACK_SIZE, 0);

	MidiSendTask = new TMidiSendTask();
	MidiSendTask->Create("MidiSend", 10*configMINIMAL_STACK_SIZE, 1);

//	ConsoleTask = new TConsoleTask(256);
//	ConsoleTask->Create("CONS", 20*configMINIMAL_STACK_SIZE, 0);

	TScheduler::StartScheduler();
}
//---------------------------------------------------------------------------

