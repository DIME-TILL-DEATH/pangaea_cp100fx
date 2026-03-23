#include "display_task.h"

#include "serial.h"

#include "sharc_task.h"

#include "icons_bitmap.h"

TDisplayTask* DisplayTask = nullptr;

TDisplayTask::TDisplayTask () :TTask()
{
	m_volIndPar_ptr = nullptr;
	m_indRefreshCounter = 0;
}

void TDisplayTask::Code()
{
	TDisplayCmd cmd ;

	queue = new TQueue(40, sizeof(TDisplayCmd));
	if (!queue) Suspend();

	if (!queue->IsCreated()) Suspend();

	extern EventGroupHandle_t startEventGroup;
	xEventGroupSync(startEventGroup, EVENT_BIT_DSTASK_STARTED, EVENT_ALL_TASK_STARTED, portMAX_DELAY);

	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY);

		switch(cmd.cmd)
		{
			case dcClear:
				LCD_Clear();
			break;

			case dcSymbolOut:
				switch(cmd.SymbolOutParams.font.name)
				{
					case Font::fnt12x13 :
						t12x13_sym(cmd.SymbolOutParams.pos.x, cmd.SymbolOutParams.pos.y,
								cmd.SymbolOutParams.symbol, cmd.SymbolOutParams.font.curs);
					break;
					case Font::fnt33x30:
					{
						t33x30_sym(cmd.SymbolOutParams.pos.x, cmd.SymbolOutParams.pos.y,
								cmd.SymbolOutParams.symbol, true);
					break;
					}
					case Font::fntSystem:
						Arsys_sym(cmd.SymbolOutParams.pos.x, cmd.SymbolOutParams.pos.y,
								cmd.SymbolOutParams.symbol, cmd.SymbolOutParams.font.curs);
					break;
					default: break;
				}
			break;

			case dcStringOut:
				switch(cmd.StringOutParams.font.name)
				{
					case Font::fnt12x13:
						t12x13_line(cmd.StringOutParams.pos.x , cmd.StringOutParams.pos.y,
								cmd.StringOutParams.string , cmd.StringOutParams.font.curs);
					break;
					case Font::fnt33x30:

					break;
					case Font::fntSystem:
						Arsys_line(cmd.StringOutParams.pos.x , cmd.StringOutParams.pos.y ,
								cmd.StringOutParams.string,cmd.StringOutParams.font.curs);
					break;
					default: break;
				}
            break;

			case dcVolInd:
				if(m_volIndicatorType == VOL_INDICATOR_VOLUME) vol_indicator(64, 64, m_volIndicatorType, m_volIndPar_ptr);
				else vol_indicator(58, 50, m_volIndicatorType, m_volIndPar_ptr);
            break;

			case dcPresetInd:
			{
				preset_ind(cmd.PresetIndParam.prog, cmd.PresetIndParam.filled);
				break;
			}

			case dcClearString:
				clear_string(cmd.ClearStringParams.pos.x, cmd.ClearStringParams.pos.y,
						cmd.ClearStringParams.font.name, cmd.ClearStringParams.count);
			break;

			case dcEffectIcon:
				effect_icon(cmd.EffectIconParam.pos.x , cmd.EffectIconParam.pos.y,
						cmd.EffectIconParam.adr , cmd.EffectIconParam.cur);
			break;

			case dcCheckBox:
				checkbox(cmd.CheckBoxparam.pos.x, cmd.CheckBoxparam.pos.y, cmd.CheckBoxparam.checked);
			break;

			case dcEqInd:
				eq_ind(cmd.EqIndParam.pos.x, cmd.EqIndParam.pos.y,
						cmd.EqIndParam.data, cmd.EqIndParam.cur);
			break;

			case dcEqPar:
				eq_par(cmd.EqQParam.pos.x, cmd.EqQParam.pos.y,
						cmd.EqQParam.num, cmd.EqQParam.type, cmd.EqQParam.band);
			break;

			case dcEqInit:
				eq_init();
			break;

			case dcEqFilter:
				lo_hi_par(cmd.EqFilterParam.num , cmd.EqFilterParam.type);
			break;

			case dcEqResponse:
				eq_response();
			break;

			case dcParamInd:
				param_ind(cmd.ParamIndParam.pos.x, cmd.ParamIndParam.pos.y,
						cmd.ParamIndParam.data);
			break;

			case dcParamIndTransparent:
				param_transparent_ind(cmd.ParamIndParam.pos.x, cmd.ParamIndParam.pos.y,
						cmd.ParamIndParam.data);
			break;

			case dcParamIndNum:
				param_ind_num_(cmd.ParamIndNumParam.pos.x, cmd.ParamIndNumParam.pos.y,
						cmd.ParamIndNumParam.data);
			break;

			case dcParamIndNote:
				param_ind_note(cmd.ParamIndNumParam.pos.x, cmd.ParamIndNumParam.pos.y,
						cmd.ParamIndNumParam.data);
			break;

			case dcParamIndPan:
				param_ind_pan(cmd.ParamIndPanParam.pos.x, cmd.ParamIndPanParam.pos.y,
						cmd.ParamIndPanParam.data);
			break;

			case dcParamIndMix:
				param_ind_mix(cmd.ParamIndPanParam.pos.x, cmd.ParamIndPanParam.pos.y,
						cmd.ParamIndPanParam.data);
			break;
			case dcDelayTimeInd:
				delay_time_ind(cmd.DelayTimeIndParam.pos.x, cmd.DelayTimeIndParam.pos.y,
						cmd.DelayTimeIndParam.data);
			break;

			case dcIconAndArrows:
				icon_print(cmd.IconAndArrowParam.num, cmd.IconAndArrowParam.str);
			break;

			case dcTunerInit:
				tuner_init();
			break;

			case dcTunerDraw:
				tuner_arrow(cmd.TunerParam.arrowPos);
				if(cmd.TunerParam.noteDefined)
					tuner_note();
			break;

			case dcTunerRefFreq:
				tuner_ref_freq((uint16_t)cmd.TunerParam.refFreq);
			break;

			case dcFswInd:
				fsw_ind(cmd.FswIndParam.num, cmd.FswIndParam.pressState, cmd.FswIndParam.holdState);
			break;

			case dcArrow:
				arrow_print(cmd.ArrowParam.pos.x, cmd.ArrowParam.pos.y, cmd.ArrowParam.dir);
			break;

			default:
			break;
      }
    }
}

void TDisplayTask::FswInd(uint8_t num, uint8_t pressState, uint8_t holdState)
{
	TDisplayCmd cmd ;
	cmd.cmd=dcFswInd;
	cmd.FswIndParam.num = num;
	cmd.FswIndParam.pressState = pressState;
	cmd.FswIndParam.holdState = holdState;
	Command(&cmd);
}

void TDisplayTask::Clear()
{
	TDisplayCmd cmd;
	cmd.cmd=dcClear;
	Command( &cmd );
}

void TDisplayTask::SymbolOut(uint8_t x, uint8_t y, Font::TFontName name, uint8_t curs, uint8_t symbol)
{
	TDisplayCmd cmd;
	cmd.cmd=dcSymbolOut;
	cmd.SymbolOutParams.pos = {x,y};
	cmd.SymbolOutParams.font.name = name;
	cmd.SymbolOutParams.font.curs = curs;
	cmd.SymbolOutParams.symbol = symbol;
	Command(&cmd);
}

void TDisplayTask::StringOut(uint8_t x, uint8_t y , Font::TFontName name , uint8_t curs , const char* string)
{
	StringOut(x, y, name, curs, (uint8_t*)string);
}

void TDisplayTask::StringOut(uint8_t x, uint8_t y , Font::TFontName name , uint8_t curs , const uint8_t* string)
{
	StringOut(x, y, name, curs, (uint8_t*)string);
}

void TDisplayTask::StringOut(uint8_t x, uint8_t y , Font::TFontName name , uint8_t curs , uint8_t* string)
{
	TDisplayCmd cmd ;
	cmd.cmd=dcStringOut;
	cmd.StringOutParams.pos = {x,y};
	cmd.StringOutParams.font.name = name;
	cmd.StringOutParams.font.curs = curs;
	kgp_sdk_libc::strncpy ( (char*)cmd.StringOutParams.string, (const char*)string, FILE_NAME_LENGTH) ;
	Command(&cmd);
}

void TDisplayTask::SetVolIndicator(TVolIndicatorType volIndicatorType, dsp_indicator_source_t indicatorSource, uint8_t* indicatorParPtr)
{
	m_volIndicatorType = volIndicatorType;
	m_volIndPar_ptr = indicatorParPtr;

	SharcTask->setParameter(DSP_ADDRESS_IND_SRC, indicatorSource, 0);

	m_indRefreshCounter = 500;
}

void TDisplayTask::VolIndicatorTask()
{
	if(m_indRefreshCounter < 4000)
	{
		m_indRefreshCounter++;
		return;
	}

	m_indRefreshCounter = 0;

	switch(m_volIndicatorType)
	{
		case VOL_INDICATOR_OFF: return;
		case VOL_INDICATOR_IN: DisplayTask->StringOut(3, 3, Font::fntSystem, 0, (uint8_t*)"Input"); break;
		case VOL_INDICATOR_OUT: DisplayTask->StringOut(3, 3, Font::fntSystem, 0, (uint8_t*)"Output"); break;
		case VOL_INDICATOR_VOLUME: break;
	}

	TDisplayCmd cmd;
	cmd.cmd=dcVolInd;
	Command(&cmd);
}

void TDisplayTask::PresetInd(uint8_t prog, bool filled)
{
	TDisplayCmd cmd;
	cmd.cmd=dcPresetInd;
	cmd.PresetIndParam.prog = prog;
	cmd.PresetIndParam.filled = filled;
	Command(&cmd);
}
void TDisplayTask::ClearString(uint8_t x, uint8_t y, Font::TFontName name, uint8_t count)
{
	TDisplayCmd cmd;
	cmd.cmd=dcClearString;
	cmd.ClearStringParams.pos = {x,y};
	cmd.ClearStringParams.font.name = name;
	cmd.ClearStringParams.count = count;
	Command(&cmd);
}
void TDisplayTask::ModuleIcon(uint8_t x, uint8_t y , uint8_t* adr , uint8_t cur)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEffectIcon;
	cmd.EffectIconParam.pos = {x,y};
	cmd.EffectIconParam.adr = adr;
	cmd.EffectIconParam.cur = cur;
	Command(&cmd);
}

void TDisplayTask::CheckBox(uint8_t x, uint8_t y, uint8_t checked)
{
	TDisplayCmd cmd;
	cmd.cmd=dcCheckBox;
	cmd.CheckBoxparam.pos = {x,y};
	cmd.CheckBoxparam.checked = checked;
	Command(&cmd);
}

void TDisplayTask::EqInit()
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqInit;
	Command(&cmd);
}
void TDisplayTask::EqInd(uint8_t x, uint8_t y , uint8_t data , uint8_t cur )
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqInd;
	cmd.EqIndParam.pos = {x,y};
	cmd.EqIndParam.data = data;
	cmd.EqIndParam.cur = cur;
	Command(&cmd);
}
void TDisplayTask::EqFreq(uint8_t col , uint8_t pag , int16_t num, uint8_t band)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqPar;
	cmd.EqQParam.pos.x = col;
	cmd.EqQParam.pos.y = pag;
	cmd.EqQParam.num = num;
	cmd.EqQParam.type = 0;
	cmd.EqQParam.band = band;
	Command(&cmd);
}

void TDisplayTask::EqQ(uint8_t col , uint8_t pag , int16_t num, uint8_t band)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqPar;
	cmd.EqQParam.pos.x = col;
	cmd.EqQParam.pos.y = pag;
	cmd.EqQParam.num = num;
	cmd.EqQParam.type = 1;
	cmd.EqQParam.band = band;
	Command(&cmd);
}
void TDisplayTask::EqFilter(float num , uint8_t type)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqFilter;
	cmd.EqFilterParam.num = num;
	cmd.EqFilterParam.type = type;
	Command(&cmd);
}

void TDisplayTask::EqResponse()
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqResponse;
	Command(&cmd);
}

void TDisplayTask::ParamInd(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamInd;
	cmd.ParamIndParam.pos = {x,y};
	cmd.ParamIndParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndTransparent(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndTransparent;
	cmd.ParamIndParam.pos = {x,y};
	cmd.ParamIndParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndNum(uint8_t x, uint8_t y , uint16_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndNum;
	cmd.ParamIndNumParam.pos = {x,y};
	cmd.ParamIndNumParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndNote(uint8_t x, uint8_t y , uint16_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndNote;
	cmd.ParamIndNoteParam.pos = {x,y};
	cmd.ParamIndNoteParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndPan(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndPan;
	cmd.ParamIndPanParam.pos = {x,y};
	cmd.ParamIndPanParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndMix(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndMix;
	cmd.ParamIndMixParam.pos = {x,y};
	cmd.ParamIndMixParam.data = data;
	Command(&cmd);
}
void TDisplayTask::DelayTimeInd(uint8_t x, uint8_t y , uint32_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcDelayTimeInd;
	cmd.DelayTimeIndParam.pos = {x,y};
	cmd.DelayTimeIndParam.data = data;
	Command(&cmd);
}

void TDisplayTask::IconAndArrows(icon_t num , strelka_t strel)
{
	TDisplayCmd cmd;
	cmd.cmd=dcIconAndArrows;
	cmd.IconAndArrowParam.num = num;
	cmd.IconAndArrowParam.str = strel;
	Command(&cmd);
}
void TDisplayTask::TunerInit(void)
{
	TDisplayCmd cmd;
	cmd.cmd=dcTunerInit;
	Command(&cmd);
}

void TDisplayTask::TunerRefFreq(float refFreq)
{
	TDisplayCmd cmd;
	cmd.cmd=dcTunerRefFreq;
	cmd.TunerParam.refFreq = refFreq;
	Command(&cmd);
}

void TDisplayTask::TunerDraw(bool noteDefined, uint8_t arrowPos)
{
	TDisplayCmd cmd;
	cmd.cmd=dcTunerDraw;
	cmd.TunerParam.noteDefined = noteDefined;
	cmd.TunerParam.arrowPos = arrowPos;
	Command(&cmd);
}

void TDisplayTask::Arrow(uint8_t x, uint8_t y , uint32_t dir)
{
	TDisplayCmd cmd;
	cmd.cmd=dcArrow;
	cmd.ArrowParam.pos = {x,y};
	cmd.ArrowParam.dir = dir;
	Command(&cmd);
}
