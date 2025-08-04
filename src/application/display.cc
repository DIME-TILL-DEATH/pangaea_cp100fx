#include "display.h"
#include "allFonts.h"
#include "icon_bit.h"
#include "eepr.h"

#include "BF706_send.h"

volatile uint8_t ind_en;

TDisplayTask* DisplayTask;

TDisplayTask::TDisplayTask () :TTask()
{
	m_volIndPar_ptr = nullptr;
	m_indRefreshCounter = 0;
}

void TDisplayTask::Code()
{
	TDisplayCmd cmd ;

	queue = new TQueue(40,sizeof(TDisplayCmd));
	if (!queue) Suspend();

	if (!queue->IsCreated()) Suspend();

	while(1)
	{
		queue->Receive(&cmd, portMAX_DELAY) ;

		switch(cmd.cmd)
		{
			case dcWriteReg:
				if (cmd.WriteRegParams.dest == wrdCmd)
					oled023_1_write_instruction(cmd.WriteRegParams.data);
				else
					oled023_1_write_data(cmd.WriteRegParams.data);
			break;

			case dcClear:
				oled023_1_disp_clear();
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

			case dcNumberOut:
				switch(cmd.StringOutParams.font.name)
				{
					case Font::fnt12x13:
						//t12x13_line(cmd.StringOutParams.pos.x , cmd.StringOutParams.pos.y, cmd.StringOutParams.string );
					break;
					case Font::fnt33x30:
						//t33x30_line(cmd.NumberOutParams.pos.x , cmd.NumberOutParams.pos.y , cmd.NumberOutParams.val);
					break;
					case Font::fntSystem:
						//Arsys_line(cmd.StringOutParams.pos.x , cmd.StringOutParams.pos.y , cmd.StringOutParams.string,cmd.StringOutParams.font.state);
					break;
					default: break;
             }
            break ;

			case dcIndicator:
				if(m_volIndicatorType == VOL_INDICATOR_VOLUME) DrawVolIndicator(64, 64);
				else DrawVolIndicator(58, 50);
            break;

			case dcProg_ind:
			{
				prog_ind(cmd.Prog_indParam.prog, cmd.Prog_indParam.filled);
				break;
			}

			case dcClear_str:
				clear_str(cmd.Clear_strParams.pos.x, cmd.Clear_strParams.pos.y,
						cmd.Clear_strParams.font.name, cmd.Clear_strParams.count);
			break;

			case dcEfIcon:
				Arsys_ef(cmd.EfIconParam.pos.x , cmd.EfIconParam.pos.y,
						cmd.EfIconParam.adr , cmd.EfIconParam.cur);
			break;

			case dcCheckBox:
				checkbox(cmd.CheckBoxparam.pos.x, cmd.CheckBoxparam.pos.y, cmd.CheckBoxparam.checked);
			break;

			case dcSetColumn:
				Set_Column_Address(cmd.SetColumnParam.x);
			break;

			case dcSetPagAdr:
				Set_Page_Address(cmd.SetPagAdrParam.x);
			break;

			case dcEqIndic:
				eq_ind(cmd.EqIndicParam.pos.x, cmd.EqIndicParam.pos.y,
						cmd.EqIndicParam.data, cmd.EqIndicParam.cur);
			break;

			case dcEqPar:
				eq_par(cmd.EqParParam.pos.x, cmd.EqParParam.pos.y,
						cmd.EqParParam.num, cmd.EqParParam.type, cmd.EqParParam.band);
			break;

			case dcEqInit:
				eq_init();
			break;

			case dcEqLH:
				lo_hi_par(cmd.EqLHParam.num , cmd.EqLHParam.type);
			break;

			case dcEqResponse:
				eq_response();
			break;

			case dcParamIndic:
				par_ind(cmd.ParamIndicParam.pos.x, cmd.ParamIndicParam.pos.y,
						cmd.ParamIndicParam.data);
			break;

			case dcParamIndicTransparent:
				p_ind(cmd.ParamIndicParam.pos.x, cmd.ParamIndicParam.pos.y,
						cmd.ParamIndicParam.data);
			break;

			case dcParamIndicNum:
				par_ind_num_(cmd.ParamIndicNumParam.pos.x, cmd.ParamIndicNumParam.pos.y,
						cmd.ParamIndicNumParam.data);
			break;

			case dcParamIndicPan:
				par_ind_pan(cmd.ParamIndicPanParam.pos.x, cmd.ParamIndicPanParam.pos.y,
						cmd.ParamIndicPanParam.data);
			break;

			case dcParamIndicMix:
				par_ind_mix(cmd.ParamIndicPanParam.pos.x, cmd.ParamIndicPanParam.pos.y,
						cmd.ParamIndicPanParam.data);
			break;
			case dcDelayTimeInd:
				del_tim_ind(cmd.DelayTimeIndParam.pos.x, cmd.DelayTimeIndParam.pos.y,
						cmd.DelayTimeIndParam.data);
			break;

			case dcStart:
				disp_start(cmd.StartParam.data);
			break;

			case dcContrast:
				disp_contr(cmd.ContrastParam.data);
			break;

			case dcTap_ind:
				tap_ind(cmd.Tap_indParam.data);
			break;

			case dcLed_Write:
				led_disp_write();
			break;

			case dcPot_Write:
				write_pot();
			break;

			case dcIcStrel:
				icon_print(cmd.IcStrelParam.num, cmd.IcStrelParam.str);
			break;

			case dcTunIni:
				tun_ini();
				m_tunerInitiated = true;
			break;

			case dcTunStrel:
				strel_tun();
			break;

			case dcInd_foot:
				ind_foot(cmd.Ind_footParam.num , cmd.Ind_footParam.val);
			break;

			case dcReset:
				oled023_1_disp_reset();
			break;

			case dcStrel:
				strel_print(cmd.StrelParam.pos.x, cmd.StrelParam.pos.y, cmd.StrelParam.dir);
			break;

			default:
			break;
      }
    }
}

void TDisplayTask::IndFoot(uint8_t num , uint8_t val)
{
	TDisplayCmd cmd ;
	cmd.cmd=dcInd_foot;
	cmd.Ind_footParam.num = num;
	cmd.Ind_footParam.val = val;
	Command(&cmd);
}
void TDisplayTask::WriteReg(TWriteRegDest dest , uint32_t data )
{
	TDisplayCmd cmd;
	cmd.cmd=dcWriteReg;
	cmd.WriteRegParams.dest = dest;
	cmd.WriteRegParams.data = data;
	Command(&cmd);
}
void TDisplayTask::Clear()
{
	TDisplayCmd cmd;
	cmd.cmd=dcClear;
	Command( &cmd );
}

void TDisplayTask::Tap_ind(uint8_t cur)
{
	TDisplayCmd cmd;
	cmd.cmd=dcTap_ind;
	cmd.Tap_indParam.data = cur;
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

	DSP_GuiSendParameter(DSP_ADDRESS_IND_SRC, indicatorSource, 0);

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
	cmd.cmd=dcIndicator;
	Command(&cmd);
}

void TDisplayTask::Prog_ind(uint8_t prog, bool filled)
{
	TDisplayCmd cmd;
	cmd.cmd=dcProg_ind;
	cmd.Prog_indParam.prog = prog;
	cmd.Prog_indParam.filled = filled;
	Command(&cmd);
}
void TDisplayTask::Clear_str(uint8_t x, uint8_t y, Font::TFontName name, uint8_t count)
{
	TDisplayCmd cmd;
	cmd.cmd=dcClear_str;
	cmd.Clear_strParams.pos = {x,y};
	cmd.Clear_strParams.font.name = name;
	cmd.Clear_strParams.count = count;
	Command(&cmd);
}
void TDisplayTask::EfIcon(uint8_t x, uint8_t y , uint8_t* adr , uint8_t cur)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEfIcon;
	cmd.EfIconParam.pos = {x,y};
	cmd.EfIconParam.adr = adr;
	cmd.EfIconParam.cur = cur;
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

void TDisplayTask::SetColumn(uint8_t x)
{
	TDisplayCmd cmd;
	cmd.cmd=dcSetColumn;
	cmd.SetColumnParam.x = x;
	Command(&cmd);
}
void TDisplayTask::SetPagAdr(uint8_t x)
{
	TDisplayCmd cmd;
	cmd.cmd=dcSetPagAdr;
	cmd.SetPagAdrParam.x = x;
	Command(&cmd);
}
void TDisplayTask::EqInit()
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqInit;
	Command(&cmd);
}
void TDisplayTask::EqIndic(uint8_t x, uint8_t y , uint8_t data , uint8_t cur )
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqIndic;
	cmd.EqIndicParam.pos = {x,y};
	cmd.EqIndicParam.data = data;
	cmd.EqIndicParam.cur = cur;
	Command(&cmd);
}
void TDisplayTask::EqPar(uint8_t col , uint8_t pag , int16_t num, uint8_t type , uint8_t band)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqPar;
	cmd.EqParParam.pos.x = col;
	cmd.EqParParam.pos.y = pag;
	cmd.EqParParam.num = num;
	cmd.EqParParam.type = type;
	cmd.EqParParam.band = band;
	Command(&cmd);
}

void TDisplayTask::EqLH(float num , uint8_t type)
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqLH;
	cmd.EqLHParam.num = num;
	cmd.EqLHParam.type = type;
	Command(&cmd);
}

void TDisplayTask::EqResponse()
{
	TDisplayCmd cmd;
	cmd.cmd=dcEqResponse;
	Command(&cmd);
}

void TDisplayTask::ParamIndic(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndic;
	cmd.ParamIndicParam.pos = {x,y};
	cmd.ParamIndicParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndicTransparent(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndicTransparent;
	cmd.ParamIndicParam.pos = {x,y};
	cmd.ParamIndicParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndicNum(uint8_t x, uint8_t y , uint16_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndicNum;
	cmd.ParamIndicNumParam.pos = {x,y};
	cmd.ParamIndicNumParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndicPan(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndicPan;
	cmd.ParamIndicPanParam.pos = {x,y};
	cmd.ParamIndicPanParam.data = data;
	Command(&cmd);
}
void TDisplayTask::ParamIndicMix(uint8_t x, uint8_t y , uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcParamIndicMix;
	cmd.ParamIndicMixParam.pos = {x,y};
	cmd.ParamIndicMixParam.data = data;
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
void TDisplayTask::StartScreen(uint8_t data)
{
	TDisplayCmd cmd;
	cmd.cmd=dcStart;
	cmd.StartParam.data = data;
	Command(&cmd);
}
void TDisplayTask::Led_Write()
{
	TDisplayCmd cmd;
	cmd.cmd=dcLed_Write;
	Command(&cmd);
}
void TDisplayTask::Pot_Write()
{
	TDisplayCmd cmd;
	cmd.cmd=dcPot_Write;
	Command(&cmd);
}

void TDisplayTask::Contrast(uint8_t val)
{
	TDisplayCmd cmd;
	cmd.cmd=dcContrast;
	cmd.ContrastParam.data = val;
	Command(&cmd);
}
void TDisplayTask::Icon_Strel(icon_t num , strelka_t strel)
{
	TDisplayCmd cmd;
	cmd.cmd=dcIcStrel;
	cmd.IcStrelParam.num = num;
	cmd.IcStrelParam.str = strel;
	Command(&cmd);
}
void TDisplayTask::TunerInit(void)
{
	TDisplayCmd cmd;
	cmd.cmd=dcTunIni;
	Command(&cmd);
}
void TDisplayTask::TunerDeinit()
{
	m_tunerInitiated = false;
	TDisplayCmd cmd;
	cmd.cmd=dcClear;
	Command(&cmd);
}

void TDisplayTask::TunStrel()
{
	TDisplayCmd cmd;
	cmd.cmd=dcTunStrel;
	Command(&cmd);
}
void TDisplayTask::Display_Reset(void)
{
	TDisplayCmd cmd;
	cmd.cmd=dcReset;
	Command(&cmd);
}
void TDisplayTask::Strel(uint8_t x, uint8_t y , uint32_t dir)
{
	if(m_tunerInitiated)
	{
		TDisplayCmd cmd;
		cmd.cmd=dcStrel;
		cmd.StrelParam.pos = {x,y};
		cmd.StrelParam.dir = dir;
		Command(&cmd);
	}
}

void TDisplayTask::DrawVolIndicator(uint8_t xPos, uint8_t indLength)
{
	uint8_t volIndPosition = 32;

	if(m_volIndPar_ptr) volIndPosition = (*m_volIndPar_ptr >> 1) + 1;
	else volIndPosition = 0;

	uint8_t outLevel;

	if(ind_out_l[1] < 8388000)
		outLevel = /*vsqrt*/(ind_out_l[1]) * ((float)(indLength - 1) / (8384000.0f));
	else
		outLevel = indLength - 1;

	ind_in_p[1] = ind_out_l[1] = 0;

	Set_Column_Address(xPos);
	Set_Page_Address(3);
	GPIO_ResetBits(GPIOB, CS);

	for(uint8_t i = 0; i < indLength; i++)
	{
		if(m_volIndicatorType == TVolIndicatorType::VOL_INDICATOR_VOLUME && i >= volIndPosition - 2 && i <= volIndPosition)
		{
				if(i == volIndPosition - 1) oled023_1_send_data(0x42);
				else oled023_1_send_data(0x3c);
		}
		else
		{
			if((i == indLength - 1) || (i == 0))
			{
				oled023_1_send_data(0x7e); //3c
			}
			else
			{
				if(i < outLevel) oled023_1_send_data(0x3c);
				else oled023_1_send_data(0x0);
			}
		}
	}
	GPIO_SetBits(GPIOB, CS);
}

