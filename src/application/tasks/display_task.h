#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <bitmaps.h>
#include "appdefs.h"

#include "sharc.h"


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
		dcClear = 0,
		dcClearString,
		dcSymbolOut,
		dcStringOut,
		dcEffectIcon,
		dcCheckBox,
		dcEqInit,
		dcEqFilter,
		dcEqPar,
		dcEqResponse,
		dcEqInd,
		dcParamInd,
		dcParamIndTransparent,
		dcParamIndNum,
		dcParamIndNote,
		dcParamIndPan,
		dcParamIndMix,
		dcDelayTimeInd,
		dcVolInd,
		dcFswInd,
		dcPresetInd,
		dcTunerInit,
		dcTunerRefFreq,
		dcTunerDraw,
		dcIconAndArrows,
		dcArrow,
		dcCount,
	}TCommand;

	typedef struct
	{
		uint8_t num;
		uint8_t pressState;
		uint8_t holdState;
	}TFswIndParam;

	typedef struct
	{
		TPos pos;
		uint32_t data;
	}TDelayTimeIndParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndPanParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndMixParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
	}TParamIndParam;

	typedef struct
	{
		TPos pos;
		uint16_t data;
	}TParamIndNumParam;

	typedef struct
	{
		TPos pos;
		uint16_t data;
	}TParamIndNoteParam;

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
	}TClearStringParams;

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
		uint8_t prog;
		uint8_t filled;
	}TPresetIndParam;

	typedef struct
	{
		TPos pos;
		uint8_t *adr;
		uint8_t cur;
	}TEffectIconParam;

	typedef struct
	{
		TPos pos;
		uint8_t checked;
	}TCheckBoxParam;

	typedef struct
	{
		TPos pos;
		uint8_t data;
		uint8_t cur;
	}TEqIndParam;

	typedef struct
	{
		TPos pos;
		int16_t num;
		uint8_t type;
		uint8_t band;
	}TEqQParam;

	typedef struct
	{
		float num;
		uint8_t type;
	}TEqFilterParam;

	typedef struct
	{
		icon_t num;
		strelka_t str;
	}TIconAndArrowParam;

	typedef struct
	{
		TPos pos;
		uint8_t dir;
	}TArrowParam;

	typedef struct
	{
		float refFreq;
		uint8_t arrowPos;
		const char* noteName;
	}TTunerParam;

	typedef struct
	{
		TCommand cmd;
		union
		{
			TClearStringParams ClearStringParams;
			TSymbolOutParams SymbolOutParams;
			TStringOutParams StringOutParams;
			TPresetIndParam PresetIndParam;
			TEffectIconParam EffectIconParam;
			TCheckBoxParam CheckBoxparam;
			TEqIndParam EqIndParam;
			TEqQParam EqQParam;
			TEqFilterParam EqFilterParam;
			TParamIndParam ParamIndParam;
			TParamIndNumParam ParamIndNumParam;
			TParamIndNoteParam ParamIndNoteParam;
			TParamIndPanParam ParamIndPanParam;
			TParamIndMixParam ParamIndMixParam;
			TDelayTimeIndParam DelayTimeIndParam;
			TFswIndParam FswIndParam;
			TArrowParam ArrowParam;
			TIconAndArrowParam IconAndArrowParam;
			TTunerParam TunerParam;
		};
	}TDisplayCmd;

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

	void Clear();
	void ClearString(uint8_t x, uint8_t y, Font::TFontName name, uint8_t count);

	void SymbolOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, uint8_t symbol);

	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, uint8_t *string);
	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, const uint8_t *string);
	void StringOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, const char *string);

	void SetVolIndicator(TVolIndicatorType indicatorType, dsp_indicator_source_t indicatorSource, uint8_t* indicatorParPtr = nullptr);
	void VolIndicatorTask();

	void ModuleIcon(uint8_t x, uint8_t y, uint8_t *adr, uint8_t cur);

	void EqInit(void);
	void EqInd(uint8_t x, uint8_t y, uint8_t data, uint8_t cur);
	void EqFreq(uint8_t x, uint8_t y, int16_t val, uint8_t band);
	void EqQ(uint8_t x, uint8_t y, int16_t val, uint8_t band);
	void EqFilter(float num, uint8_t type);
	void EqResponse();

	void CheckBox(uint8_t x, uint8_t y, uint8_t checked);

	void ParamInd(uint8_t x, uint8_t y, uint8_t data);
	void ParamIndTransparent(uint8_t x, uint8_t y , uint8_t data);
	void ParamIndNum(uint8_t x, uint8_t y, uint16_t data);
	void ParamIndNote(uint8_t x, uint8_t y, uint16_t data);
	void ParamIndPan(uint8_t x, uint8_t y, uint8_t data);
	void ParamIndMix(uint8_t x, uint8_t y, uint8_t data);

	void PresetInd(uint8_t prog, bool filled = true);
	void FswInd(uint8_t num, uint8_t pressState, uint8_t holdDualState);
	void DelayTimeInd(uint8_t x, uint8_t y, uint32_t data);

	void IconAndArrows(icon_t num, strelka_t strel);

	void TunerInit();
	void TunerRefFreq(float refFreq);
	void TunerDraw(uint8_t arrowPos, const char* noteName);

	void Arrow(uint8_t x, uint8_t y, uint32_t dir);

private:
	void Code();
	TQueue *queue;

	TVolIndicatorType m_volIndicatorType{VOL_INDICATOR_OFF};
	uint8_t* m_volIndPar_ptr{nullptr};
	uint16_t m_indRefreshCounter;
};

extern volatile uint8_t ind_en;

extern TDisplayTask *DisplayTask;

#endif /* __DISPLAY_H__ */
