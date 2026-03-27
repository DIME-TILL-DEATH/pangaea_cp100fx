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

	// ----------  readline IO stream interface ------------------

	// Note! befor use TConsoleTask

	inline void Enable(TTranslator::state_fnc enable)
	{
		read_line->Enable(enable);
	}
	inline TTranslator::state_fnc Enable()
	{
		return read_line->Enable();
	}
	inline void Enable(int &ret)
	{
		return read_line->Enable(ret);
	}

	inline void Disable(TTranslator::state_fnc disable)
	{
		read_line->Disable(disable);
	}
	inline TTranslator::state_fnc Disable()
	{
		return read_line->Disable();
	}
	inline void Disable(int &ret)
	{
		return read_line->Disable(ret);
	}

	inline void SendChar(TTranslator::send_char_fnc send_char)
	{
		read_line->SendChar(send_char);
	}
	inline TTranslator::send_char_fnc SendChar()
	{
		return read_line->SendChar();
	}
	inline int SendChar(const int c)
	{
		return read_line->SendChar(c);
	}

	inline void RecvChar(TTranslator::recv_char_fnc recv_char)
	{
		read_line->RecvChar(recv_char);
	}
	inline TTranslator::recv_char_fnc RecvChar()
	{
		return read_line->RecvChar();
	}
	inline int RecvChar(int &c)
	{
		return read_line->RecvChar(c);
	}

	inline void RecvString(TTranslator::recv_string_fnc recv_string)
	{
		read_line->RecvString(recv_string);
	}
	inline TTranslator::recv_string_fnc RecvString()
	{
		return read_line->RecvString();
	}
	inline size_t RecvString(emb_string &dest)
	{
		return read_line->RecvString(dest);
	}
	;

	inline void RecvLine(TTranslator::recv_string_fnc recv_string)
	{
		read_line->RecvLine(recv_string);
	}
	inline TTranslator::recv_string_fnc RecvLine()
	{
		return read_line->RecvLine();
	}
	inline size_t RecvLine(emb_string &dest)
	{
		return read_line->RecvLine(dest);
	}
	;

	void inline SendString(TTranslator::send_string_fnc send_string)
	{
		read_line->SendString(send_string);
	}
	inline TTranslator::send_string_fnc SendString()
	{
		return read_line->SendString();
	}
	inline TTranslator::symbol_type_ptr_t SendString(TTranslator::const_symbol_type_ptr_t dest)
	{
		return read_line->SendString(dest);
	}
	;
	inline TTranslator::symbol_type_ptr_t SendString(emb_string &dest)
	{
		return read_line->SendString(dest.c_str());
	}
	;

	inline void SendBuf(TTranslator::send_buf_fnc send_buf)
	{
		read_line->SendBuf(send_buf);
	}
	inline TTranslator::send_buf_fnc SendBuf()
	{
		return read_line->SendBuf();
	}
	inline int SendBuf(TTranslator::const_symbol_type_ptr_t buf, size_t size)
	{
		return read_line->SendBuf(buf, size);
	}

	inline void RecvBuf(TTranslator::recv_buf_fnc recv_buf)
	{
		read_line->RecvBuf(recv_buf);
	}
	inline TTranslator::recv_buf_fnc RecvBuf()
	{
		return read_line->RecvBuf();
	}
	inline int RecvBuf(TTranslator::symbol_type_ptr_t buf, size_t size)
	{
		return read_line->RecvBuf(buf, size);
	}

	//------------------------------------------------------------------------------------

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
	;
	static inline void NoSync()
	{
		NOP();
	}
	;

	void SetSyncMode();
	void SetNoSyncMode();

	inline void Echo(bool state)
	{
		read_line->Echo(state);
	}
	inline bool Echo()
	{
		return read_line->Echo();
	}

	inline TQueue::TQueueSendResult WriteToInputBuffFromISR(const char *symbol, BaseType_t *HigherPriorityTaskWoken)
	{
		return rx_queue->SendToBackFromISR(symbol, HigherPriorityTaskWoken);
	}
	inline TQueue::TQueueSendResult WriteToInputBuff(const char *symbol)
	{
		return rx_queue->SendToBack(symbol, 0);
	}

	inline TQueue::TQueueReceiveResult ReadFromInputBuff(char *symbol)
	{
		return rx_queue->Receive(symbol, portMAX_DELAY);
	}

	void SetIo(readline_io_t *io);

private:
	void Code();
	TSemaphore *rx_sem
	{nullptr};
	TSemaphore *tx_sem
	{nullptr};
	TTranslator *read_line
	{nullptr};

	TQueue *rx_queue
	{nullptr};
};

extern TConsoleTask *ConsoleTask;

void ConsoleSetCmdHandlers(TTranslator *rl);

// handlers execute in UITask
#define msg_console(...) rl->UnsafePrintF( __VA_ARGS__ )

#endif /*__USB_CONSOLE_H__*/
