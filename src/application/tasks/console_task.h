#ifndef __CONSOLE_TASK_H__
#define __CONSOLE_TASK_H__

#include "appdefs.h"
#include "translator.h"

#include "format.h"

class TConsoleTask: public TTask
{
public:

	typedef void (*console_task_hw_init_t)();

	typedef void (*console_state_t)(int &ret);

	typedef const struct
	{
		console_task_hw_init_t console_task_hw_init;

		TTranslator::state_fnc enable;
		TTranslator::state_fnc disable;

		TTranslator::send_char_fnc send_char;
		TTranslator::recv_char_fnc recv_char;

		TTranslator::send_string_fnc send_string;
		TTranslator::recv_string_fnc recv_string;
		TTranslator::recv_string_fnc recv_line;

		TTranslator::send_buf_fnc send_buf;
		TTranslator::recv_buf_fnc recv_buf;
	} readline_io_t;

	TConsoleTask(/*readline_io_t *io,*/const size_t rx_queue_size);
	virtual ~TConsoleTask();

	// нельзя заинланить изза необходимости формирования стека
	void PrintF(const char *format, ...) __attribute__ ((noinline));
	void __attribute__ ((noinline)) PrintF(const char **format);

	void UnsafePrintF(const char *format, ...) __attribute__ ((noinline));
	void __attribute__ ((noinline)) UnsafePrintF(const char **format);

	inline void Take()
	{
		tx_sem->Take(portMAX_DELAY);
	}
	inline void Give()
	{
		tx_sem->Give();
	}

	TQueue::TQueueSendResult RxSend(uint8_t *buf, uint32_t len);
	size_t RxQueueWaiting();
	char RxQueueRecv();

	static inline void NoSync()
	{
		NOP();
	}

	void SetSyncMode();
	void SetNoSyncMode();

	void SetIo(readline_io_t *io);

	TQueue *rx_queue{nullptr};

private:
	void Code();
	TSemaphore *rx_sem{nullptr};
	TSemaphore *tx_sem{nullptr};
	TTranslator *read_line{nullptr};
};

extern TConsoleTask *ConsoleTask;

void ConsoleSetCmdHandlers(TTranslator *rl);

#define msg_console(...) rl->UnsafePrintF( __VA_ARGS__ )

#endif /*__USB_CONSOLE_H__*/
