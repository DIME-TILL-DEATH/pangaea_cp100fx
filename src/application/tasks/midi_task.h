#ifndef __MIDI_TASK_H__
#define __MIDI_TASK_H__

#include "appdefs.h"

namespace Midi{
	enum status
	{
	  MIDI_STATUS_NOTE_OFF   = 0x80,
	  MIDI_STATUS_NOTE_ON    = 0x90,
	  MIDI_STATUS_NOTE_AT    = 0xA0, // after touch
	  MIDI_STATUS_CC         = 0xB0, // control change
	  MIDI_STATUS_PC 		 = 0xC0,
	  MIDI_STATUS_CHANNEL_AT = 0xD0, // after touch
	  MIDI_STATUS_PITCH_BEND = 0xE0,
	  MIDI_STATUS_TIMING_CLOCK = 0xF8,
	  MIDI_STATUS_SYSEX_START = 0xF0,
	  MIDI_STATUS_SYSEX_STOP = 0xF7
	};
};

class TMidiTask: public TTask
{
public:

	TMidiTask();

	typedef enum{
		PC_INTERNAL,
		PC_EXTERNAL
	}TPcType;

	void uartRcv(uint8_t data);
	void uartSend(uint8_t data);

	void pcSend(TPcType pcType, uint8_t presetNumber);
	void exprSend(uint8_t data);
	void fswPressed(uint8_t data, uint8_t state);

private:


	enum MidiState
	{
		PROCESS_STATUS_BYTE,
		WAIT_BYTE1,
		WAIT_BYTE2
	};

	typedef enum{
		UART_IN,
		PROGRAMM_CHANGE,
		EXPRESSION,
		FOOTSWITCH
	}TCmd;

	typedef struct{
		uint8_t rcvData;
	}TUartInCmd;

	typedef struct{
		TPcType pcType;
		uint8_t presetNumber;
	}TProgChangeCmd;

	typedef struct{
		uint8_t data;
	}TExpressionCmd;

	typedef struct{
		uint8_t fswType;
		uint8_t state;
	}TFootswitchCmd;

	typedef struct{
		TCmd type;

		union
		{
			TUartInCmd uartCmd;
			TProgChangeCmd pcCmd;
			TExpressionCmd exprCmd;
			TFootswitchCmd fswCmd;
		};
	}TMidiCmd;

	TQueue *queue;

	MidiState midiState = PROCESS_STATUS_BYTE;
	uint8_t statusByte;
	uint8_t dataByte[2];
	uint8_t rcvChannel;

	void sendBank();
	void Code() override;
	TQueue::TQueueSendResult Command(TMidiCmd *cmd)
	{
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			TQueue::TQueueSendResult result;
			result = queue->SendToBackFromISR(cmd, &HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
			return result;
		}
		else
		{
			return queue->SendToBack(cmd, 0);
		}
	}
};

extern TMidiTask *MidiTask;

void ISR_midi_recieve();

#endif /* __MIDI_TASK_H__ */
