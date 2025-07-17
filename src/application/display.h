#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "appdefs.h"
#include "modules.h"
#include "BF706_send.h"

#include "allFonts.h"
#include "icon_bit.h"

#define FILE_NAME_LENGTH 64

class TDisplayTask: public TTask
{
public:
	TDisplayTask();

	typedef enum
	{
		wrdCmd = 0,
		wrdData,
		wrdCount
	}TWriteRegDest;

	typedef struct
	{
		uint8_t x;
		uint8_t y;
	}TPos;

	typedef enum
	{
		dcWriteReg = 0,
		dcClear,
		dcClear_str,
		dcSymbolOut,
		dcStringOut,
		dcNumberOut,
		dcIndicator,
//		dcMenu_init,
		dcProg_ind,
		dcEfIcon,
		dcSetColumn,
		dcSetPagAdr,
		dcEqIndic,
		dcEqInit,
		dcEqLH,
		dcEqPar,
		dcEqResponse,
		dcParamIndic,
		dcParamIndicTransparent,
		dcParamIndicNum,
		dcParamIndicPan,
		dcParamIndicMix,
		dcDelayTimeInd,
		dcStart,
		dcContrast,
		dcTap_ind,
		dcSys_Menu,
		dcLed_Write,
		dcPot_Write,
//		dcMain_scr,
//		dcPresetPreview,
		dcIcStrel,
		dcTunIni,
		dcTunNote,
		dcTunStrel,
		dcInd_foot,
		dcReset,
		dcStrel,
		dcCount
	}TCommand;

	typedef struct
	{
		uint8_t num;
		uint8_t val;
	}TInd_footParam;

	typedef struct
	{
		uint8_t data;
	}TContrastParam;

	typedef struct
	{
		uint8_t data;
	}TStartParam;

	typedef struct
	{
		uint8_t data;
	}TTap_indParam;

	typedef struct
	{
		TPos pos;
		uint32_t data;
	}TDelayTimeIndParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndicPanParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndicMixParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndicParam;

	typedef struct
	{
		TPos pos;
		uint16_t data;
	}TParamIndicNumParam;

	typedef struct
	{
		TWriteRegDest dest;
		uint32_t data;
	}TWriteRegParams;

	typedef struct
	{
		TPos pos;
		Font::TFontStruct font;
		uint8_t count;
	}TClear_strParams;

	typedef struct
	{
		TPos pos;
		Font::TFontStruct font;
		uint8_t symbol;
	}TSymbolOutParams;

	typedef struct
	{
		TPos pos;
		Font::TFontStruct font;
		uint8_t string[FILE_NAME_LENGTH];
	}TStringOutParams;

	typedef struct
	{
		TPos pos;
		Font::TFontStruct font;
		uint32_t val;
	} TNumberOutParams;

	typedef struct
	{
		uint8_t prog;
		uint8_t filled;
	}TProg_indParam;

	typedef struct
	{
		TPos pos;
		uint8_t *adr;
		uint8_t cur;
	}TEfIconParam;

	typedef struct
	{
		uint8_t x;
	}TSetColumnParam;

	typedef struct
	{
		uint8_t x;
	}TSetPagAdrParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
		uint8_t cur;
	}TEqIndicParam;

	typedef struct
	{
		TPos pos;
		int16_t num;
		uint8_t type;
		uint8_t band;
	}TEqParParam;

	typedef struct
	{
		float num;
		uint8_t type;
	}TEqLHParam;

	typedef struct
	{
		icon_t num;
		strelka_t str;
	}TIcStrelParam;

	typedef struct
	{
		uint8_t index;
	}TPresetPreviewParam;

	typedef struct
	{
		TPos pos;
		uint8_t dir;
	}TStrelParam;

	typedef struct
	{
		TCommand cmd;
		union
		{
			TWriteRegParams WriteRegParams;
			TClear_strParams Clear_strParams;
			TSymbolOutParams SymbolOutParams;
			TStringOutParams StringOutParams;
			TNumberOutParams NumberOutParams;
			TProg_indParam Prog_indParam;
			TEfIconParam EfIconParam;
			TSetColumnParam SetColumnParam;
			TSetPagAdrParam SetPagAdrParam;
			TEqIndicParam EqIndicParam;
			TEqParParam EqParParam;
			TEqLHParam EqLHParam;
			TParamIndicParam ParamIndicParam;
			TParamIndicNumParam ParamIndicNumParam;
			TParamIndicPanParam ParamIndicPanParam;
			TParamIndicMixParam ParamIndicMixParam;
			TDelayTimeIndParam DelayTimeIndParam;
			TContrastParam ContrastParam;
			TStartParam StartParam;
			TTap_indParam Tap_indParam;
			TIcStrelParam IcStrelParam;
			TPresetPreviewParam PresetPreviewParam;
			TInd_footParam Ind_footParam;
			TStrelParam StrelParam;
		};
	}TDisplayCmd;

	enum TVolIndicatorType
	{
		VOL_INDICATOR_OFF = 0,
		VOL_INDICATOR_IN,
		VOL_INDICATOR_OUT,
		VOL_INDICATOR_VOLUME
	};

	// send command from code/tasks
	inline TQueue::TQueueSendResult Command(TDisplayCmd *cmd)
	{
		if(cortex_isr_num())
		{
			// send comand from ISR
			BaseType_t HigherPriorityTaskWoken;
			TQueue::TQueueSendResult result;
			result = queue->SendToBackFromISR(cmd, &HigherPriorityTaskWoken);
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
			return result;
		}
		else
		{
			// thread mode
			return queue->SendToBack(cmd, 0);
		}
	}

	// command for perform waiting
	inline UBaseType_t CommandWaiting()
	{
		UBaseType_t result;
		if(cortex_isr_num())
		{
			BaseType_t HigherPriorityTaskWoken;
			result = queue->MessagesWaitingFromISR();
			if(HigherPriorityTaskWoken)
				TScheduler::Yeld();
			return result;
		}
		else
			result = queue->MessagesWaiting();

		return result;
	}

	// read a free amount in comman queue
	inline UBaseType_t FreeSpaceForCommand()
	{
		return queue->SpacesAvailable();
	}

	void WriteReg(TWriteRegDest dest, uint32_t data);
	void Clear();
	void Clear_str(uint8_t x, uint8_t y, Font::TFontName name, uint8_t count);
	void SymbolOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, uint8_t symbol);

	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, uint8_t *string);
	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, const uint8_t *string);
	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, const char *string);

	void NumberOut(uint8_t x, uint8_t y, Font::TFontName name, Font::TFontState state, uint32_t val);

	void SetVolIndicator(TVolIndicatorType indicatorType, dsp_indicator_source_t indicatorSource, uint8_t* indicatorParPtr = nullptr);
	void VolIndicatorTask();

	void Prog_ind(uint8_t prog, bool filled = true);
	void EfIcon(uint8_t x, uint8_t y, uint8_t *adr, uint8_t cur);
	void SetColumn(uint8_t x);
	void SetPagAdr(uint8_t x);
	void EqIndic(uint8_t x, uint8_t y, uint8_t data, uint8_t cur);
	void EqPar(uint8_t col, uint8_t pag, int16_t num, uint8_t type, uint8_t band);
	void EqInit(void);
	void EqLH(float num, uint8_t type);
	void EqResponse();
	void ParamIndic(uint8_t x, uint8_t y, uint8_t data);
	void ParamIndicTransparent(uint8_t x, uint8_t y , uint8_t data);
	void ParamIndicNum(uint8_t x, uint8_t y, uint16_t data);
	void ParamIndicPan(uint8_t x, uint8_t y, uint8_t data);
	void ParamIndicMix(uint8_t x, uint8_t y, uint8_t data);
	void DelayTimeInd(uint8_t x, uint8_t y, uint32_t data);
	void StartScreen(uint8_t data);
	void Contrast(uint8_t val);
	void Tap_ind(uint8_t cur);
	void Sys_Menu(void);
	void Led_Write(void);
	void Pot_Write(void);
//	void Main_scr();
	void Icon_Strel(icon_t num, strelka_t strel);
	void TunIni(void);
	void TunNote(void);
	void TunStrel(void);
	void PresetPreview(uint8_t index);
	void IndFoot(uint8_t num, uint8_t val);
	void Display_Reset(void);
	void Strel(uint8_t x, uint8_t y, uint32_t dir);

private:
	void Code();
	TQueue *queue;

	TVolIndicatorType m_volIndicatorType{VOL_INDICATOR_OFF};
	uint8_t* m_volIndPar_ptr;
	uint16_t m_indRefreshCounter;

	void DrawVolIndicator(uint8_t xPos, uint8_t indLength);
};

extern volatile uint8_t ind_en;

extern TDisplayTask *DisplayTask;

#endif /* __DISPLAY_H__ */
