#include "appdefs.h"

#include "storage.h"

#include "errno.h"
#include "mmgr.h"

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
