#include "console_task.h"

#include "int2str.h"
#include <math.h>

static void undefind_command_handler(TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	msg_console ("undefined command %s\n" , args[0]);
}

// функции враперы синхронизации для readline
extern "C" void console_take()
{
	ConsoleTask->Take();
}
extern "C" void console_give()
{
	ConsoleTask->Give();
}

extern "C" void NoSync()
{
	ConsoleTask->NoSync();
}


TConsoleTask *ConsoleTask;

//-----------------------------------------------------------------------

TConsoleTask::TConsoleTask(/*readline_io_t* readline_io,*/const size_t rx_queue_size)
{
	read_line = new TTranslator();
	read_line->Init(1, 256);

	rx_queue = new TQueue(1024, 1);

	SetSyncMode();

	read_line->SetCommandNotFound(undefind_command_handler);

	// call user defined handler setter
	ConsoleSetCmdHandlers(read_line);

	rx_sem = new TSemaphore(TSemaphore::fstBinary);
	tx_sem = new TSemaphore(TSemaphore::fstBinary);
}

TConsoleTask::~TConsoleTask()
{
	if(read_line)
		delete read_line;
	if(rx_sem)
		delete rx_sem;
	if(tx_sem)
		delete tx_sem;
}

void TConsoleTask::Code()
{
	Suspend();
	while(1)
	{
		read_line->Process();
	}
}
//----------------------------------------------------------------------
TQueue::TQueueSendResult TConsoleTask::RxSend(uint8_t *buf, uint32_t len)
{
	for(uint32_t i = 0; i<len; i++)
	{
		if(rx_queue->SendToBack(buf+i, 0)==TQueue::qsrQueueFull)
			return TQueue::qsrQueueFull;
	}
	return TQueue::qsrPass;
}


size_t TConsoleTask::RxQueueWaiting()
{
	return rx_queue->MessagesWaiting();
}

char TConsoleTask::RxQueueRecv()
{
	char c;
	rx_queue->Receive(&c, portMAX_DELAY);
	return c;
}

//-----------------------------------------------------------------------
void TConsoleTask::SetSyncMode()
{
	read_line->SetGiveSem(console_give);
	read_line->SetTakeSem(console_take);
}

void TConsoleTask::SetNoSyncMode()
{
	read_line->SetGiveSem(NoSync);
	read_line->SetTakeSem(NoSync);
}

void TConsoleTask::SetIo(readline_io_t *readline_io)
{
	// выключение перефении текущего канала связи
	if(read_line->Disable())
		read_line->Disable();

	// установка обработчиков нового канала связи
	read_line->SendChar(readline_io->send_char);
	read_line->RecvChar(readline_io->recv_char);
	read_line->SendString(readline_io->send_string);
	read_line->RecvString(readline_io->recv_string);
	read_line->RecvLine(readline_io->recv_line);
	read_line->SendBuf(readline_io->send_buf);
	read_line->RecvBuf(readline_io->recv_buf);
	read_line->Enable(readline_io->enable);
	read_line->Disable(readline_io->disable);

	// инициализация HW нового канала связи
	if(readline_io->console_task_hw_init)
		readline_io->console_task_hw_init();

	// включение перефении нового канала связи
	if(read_line->Enable())
		read_line->Enable();
}

//----------------------------------------------------------------------------------
void TConsoleTask::PrintF(const char *format, ...)
{
	read_line->PrintF(&format);
}

void TConsoleTask::PrintF(const char **format)
{
	read_line->PrintF(format);
}

void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char *format, ...)
{
	read_line->UnsafePrintF(&format);
}

void __attribute__ ((noinline)) TConsoleTask::UnsafePrintF(const char **format)
{
	read_line->UnsafePrintF(format);
}

