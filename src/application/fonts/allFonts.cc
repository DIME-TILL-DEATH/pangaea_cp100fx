#include "appdefs.h"
#include "allFonts.h"

#include "../gui/gui_task.h"
#include "fs.h"
#include "eepr.h"
#include "cs.h"
#include "amt.h"

const uint8_t cs[] = "IR";
const uint8_t ph[] = "PH";
const uint8_t ch[] = "CH";
const uint8_t dl[] = "DL";
const uint8_t rm[] = "FL";
const uint8_t er[] = "ER";
const uint8_t rv[] = "RV";
const uint8_t equ[] = "EQ";
const uint8_t tr[] = "TR";
const uint8_t syst[] = "CC";
const uint8_t volu[] = "Vol";
const uint8_t in[] = "In";
const uint8_t out[] = "OUT";
const uint8_t cm[] = "CM";
const uint8_t am[] = "PA";
const uint8_t vl[] = "PL";
const uint8_t na[] = "NM";
const uint8_t pr[] = "PR";
const uint8_t gt[] = "GT";
const uint8_t rf[] = "RF";

const uint8_t sys[] = "System";
const uint8_t cc_of[] = "Off      ";
const uint8_t sys_menu_list[][12] = {"Mode", "MIDI ch", "Cab num", "Expression", "Footswitch", "S/PDIF", "MIDI PC Map", "Tempo", "Tuner contr", "Time",
		"Swap UpConf", "Speed tun"};
const uint8_t expr_menu[][10] = {"Type", "Calibrate", "CC#", "Store Lev"};
const uint8_t expr_type[][12] = {"   Off     ", "Standard V ", "Alternat V ", "Standard CC", "Alternat CC"};
const uint8_t footsw_menu[][12] = {"FSW-DOWN", "FSW-CONFIRM", "FSW-UP", "Speed FS"};
const uint8_t ext_switch[][12] = {"Off        ", "Expression ", "Foot switch"};
const uint8_t int_sw_list[][12] = {"True bypass", "Controllers", "Presets sw "};
const uint8_t contr_ext_l[][12] = {"Expression ", " FSW DOWN  ", "FSW CONFIRM", "  FSW UP   "};
const uint8_t fsw_t[][12] = {"Default    ", "Controller ", "Tuner      ", "Preset Map1", "Preset Map2", "Preset Map3", "Preset Map4"};
const uint8_t tempo_type[][10] = {"Preset   ", "Global   ", "Glob+MIDI"};


enum TControllerSrc
{
	OFF,
	EXPRESSION,
	FSW_DOWN, FSW_CONFIRM, FSW_UP,
	CC1, CC2, CC3, CC4
};

enum TControllerDst
{
	Preamp_On_Off,
	Amp_On_Off, Amp_Volume, Amp_Slave,
	CabSim_On_Off, Eq_On_Off,
	Delay_On_Off, Delay_Volume, Delay_Fedback, Delay_Tap,
//	/*10*/"Phaser On Off", "Phaser Volume", "Phaser Rate  ",
//	/*13*/"Flanger OnOff", "Flang  Volume", "Flang  Rate  ",
//	/*16*/"Chorus On Off", "Chorus Volume", "Chorus Rate  ",
//	/*19*/"Reverb On Off", "Reverb Volume", "Reverb Time  ",
//	/*22*/"Tremolo OnOff", "Tremolo Inten", "Tremolo Rate ",
//	/*25*/"Preset Level ",
//	/*26*/"Tremolo TAP  ",
//	/*27*/"Compr On Off ", "Compr Thresh ", "Compr Volume ",
//	/*30*/"Filt  On Off ", "Filt LFOrate ", "Filt freq    ",
//	/*33*/"ER On Off    ", "ER Volume    ",
//	/*35*/"Filt LFO TAP ",
//	/*36*/"Vol Ct On Off",
//	/*37*/"Cab1 Volume  ", "Cab2 Volume  ",
//	/*39*/"Gate On Off  ", "Gate Thresh  ",
//	/*41*/"HPF frequency", "LPF frequency", "Presence val ",
//	/*44*/"Preamp Gain  ", "Preamp Volume", "Preamp Low   ", "Preamp Mid   ", "Preamp High  ",
//	/*49*/"Eq Band1 Lev ", "Eq Band2 Lev ", "Eq Band3 Lev ", "Eq Band4 Lev ", "Eq Band5 Lev ",
//	/*54*/"Reverb Type
};

typedef struct
{
	TControllerSrc controllerSrc;
	TControllerDst controllerDst;
	uint8_t minValue;
	uint8_t maxValue;
}TControllerData;

uint32_t ind_in_p[2];
uint32_t ind_out_l[2];
uint16_t ind_poin;
uint8_t vol_fl;
uint8_t vol_vol;
//uint8_t edit_modules_fl = 0;
uint8_t imya_temp;

uint8_t t_po;
uint8_t t_po1;
volatile uint8_t t_no;
//volatile uint32_t p_no1 = 0x1ffff;

void disp_start(uint8_t num)
{
	uint16_t a = 0;
	if(!num)
	{
		for(uint8_t j = 0; j<4; j++)
		{
			Set_Page_Address(j);
			Set_Column_Address(27);
			GPIO_ResetBits(GPIOB, CS);
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);
			GPIO_SetBits(GPIOB, RS);
			for(uint8_t i = 0; i<75; i++)
			{
				oled023_1_send_data(amt_logo[a++]);
				if(a>299)
					break;
			}
			GPIO_SetBits(GPIOB, CS);
		}
		//for(uint32_t d = 0 ; d < 0x1ffffff ; d++)NOP();
	}
	else
	{
		for(uint8_t i = 0; i<128; i++)
		{
			Set_Column_Address(i);
			for(uint8_t j = 0; j<4; j++)
			{
				Set_Page_Address(j);
				oled023_1_write_data(0);
			}
			for(uint32_t d = 0; d<0xffff; d++)
				NOP();
		}
		Arsys_line(50, 2, (uint8_t*)amt_ver, 0);
		for(uint32_t d = 0; d<0x7fffff; d++)
			NOP();
		for(uint8_t i = 127; i>0; i--)
		{
			Set_Column_Address(i);
			for(uint8_t j = 0; j<4; j++)
			{
				Set_Page_Address(j);
				oled023_1_write_data(0);
			}
			for(uint32_t d = 0; d<0xffff; d++)
				NOP();
		}
		GPIO_SetBits(GPIOB, CS);
	}
	extern volatile uint8_t ind_en;
	ind_en = 1;
}

void vol_ind(uint8_t xPos, uint8_t indLength)
{
	uint8_t vvol = (vol_vol>>1)+1;
	uint8_t outl;
	ind_poin = 0;
	if(ind_out_l[1]<8388000)
		outl = /*vsqrt*/(ind_out_l[1])*((float)(indLength-1)/(8384000.0f));
	else
		outl = indLength-1;
	ind_in_p[1] = ind_out_l[1] = 0;

//  if(currentMenu->menuType() == MENU_MODULES) //if(edit_modules_fl)
//  {
	Set_Column_Address(xPos);
	Set_Page_Address(3);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i<indLength; i++)
	{
		if((vol_fl==1)&&(i>=vvol-2)&&(i<=vvol))
		{
			if(i==vvol-1)
				oled023_1_send_data(0x42);
			else
				oled023_1_send_data(0x3c);
		}
		else
		{
			if(outl>i)
				oled023_1_send_data(0x3c);
			else
			{
				if((i==indLength-1)||(i==0))
					oled023_1_send_data(0x7e); //3c
				else
					oled023_1_send_data(0x0);
			}
		}
	}
	GPIO_SetBits(GPIOB, CS);
//  }
//  else {
//  	  Set_Column_Address(24);
//  	  Set_Page_Address(3);
//  	  GPIO_ResetBits(GPIOB,CS);
//  	  for(uint8_t i = 0 ; i < 64 ; i++)
//  	  {
//    	if(outl > i)oled023_1_send_data(0x78);
//      	else
//        	{
//          	  if((i == 63) || (i == 0))oled023_1_send_data(0x78);
//          	  else oled023_1_send_data(0x0);
//        	}
//      	  }
//  	  }
//	GPIO_SetBits(GPIOB,CS);
}
void tap_ind(uint8_t cur)
{
	Set_Page_Address(3);
	Set_Column_Address(90);
	for(uint8_t i = 0; i<19; i++)
	{
		if(cur==1)
			oled023_1_write_data(0xc0);
		else
			oled023_1_write_data(0);
	}
	Set_Page_Address(4);
	Set_Column_Address(90);
	if(cur==1)
	{
		oled023_1_write_data(0xff);
		oled023_1_write_data(0xff);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Page_Address(5);
	Set_Column_Address(90);
	if(cur==1)
	{
		oled023_1_write_data(7);
		oled023_1_write_data(7);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Column_Address(107);
	if(cur==1)
	{
		oled023_1_write_data(7);
		oled023_1_write_data(7);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
	Set_Page_Address(4);
	Set_Column_Address(107);
	if(cur==1)
	{
		oled023_1_write_data(0xff);
		oled023_1_write_data(0xff);
	}
	else
	{
		oled023_1_write_data(0);
		oled023_1_write_data(0);
	}
}
void mode_ind(uint8_t val)
{
	uint8_t a1, a2;
	Set_Page_Address(0);
	switch(val)
	{
		case 0:
			a1 = a2 = 0x7;
		break;
		case 1:
			a1 = 0;
			a2 = 0x7;
		break;
		case 2:
			a1 = a2 = 0;
		break;
	}
	Set_Column_Address(2);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i<15; i++)
		oled023_1_send_data(a1);
	GPIO_SetBits(GPIOB, CS);
	Set_Column_Address(38);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i<15; i++)
		oled023_1_send_data(a2);
	GPIO_SetBits(GPIOB, CS);
	Set_Column_Address(74);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i<15; i++)
		oled023_1_send_data(0x7);
	GPIO_SetBits(GPIOB, CS);
}

extern uint8_t fsw1_in_fl1;
extern uint8_t fsw2_in_fl1;
extern uint8_t fsw3_in_fl1;
extern uint8_t contr_kn[];
extern uint8_t contr_kn1[];
inline void ind_foot_proc(uint8_t col, uint8_t val)
{
	Set_Page_Address(3);
	Set_Column_Address(col);
	GPIO_ResetBits(GPIOB, CS);
	for(uint8_t i = 0; i<7; i++)
	{
		if(!i||(i==6))
			oled023_1_send_data(0x7c);
		else
			oled023_1_send_data((val*0xfe)|0x82);
	}
	GPIO_SetBits(GPIOB, CS);
}
void ind_foot(uint8_t num, uint8_t val)
{
	switch(num)
	{
		case 0:
			if(!fsw1_in_fl1&&(sys_para[fs1]==1))
				ind_foot_proc(0, contr_kn[0]);
			if(sys_para[fsm1]&&(sys_para[fs11]==1))
				ind_foot_proc(7, contr_kn1[0]);
		break;
		case 1:
			if(!fsw2_in_fl1&&(sys_para[fs2]==1))
			{
				if(sys_para[fsm2])
					ind_foot_proc(57, contr_kn[1]);
				else
					ind_foot_proc(60, contr_kn[1]);
			}
			if(sys_para[fsm2]&&(sys_para[fs21]==1))
				ind_foot_proc(64, contr_kn1[1]);
		break;
		case 2:
			if(!fsw3_in_fl1&&(sys_para[fs3]==1))
			{
				if(sys_para[fsm3])
					ind_foot_proc(113, contr_kn[2]);
				else
					ind_foot_proc(120, contr_kn[2]);
			}
			if(sys_para[fsm3]&&(sys_para[fs31]==1))
				ind_foot_proc(120, contr_kn1[2]);
		break;
	}
}

const uint8_t inpp[] = "In";
const uint8_t outt[] = "Out";
void main_screen(void)
{
	oled023_1_disp_clear();
	ind_poin = 0;
	Arsys_clean(2, 0, (uint8_t*)presetName);
	Arsys_clean(2, 1, (uint8_t*)presetName);
	if(!imya_temp)
	{
		Arsys_line(2, 0, (uint8_t*)presetName, 0);
		Arsys_line(2, 1, (uint8_t*)presetComment, 0);
	}
	else
	{
		Arsys_line(2, 0, (uint8_t*)imya_t, 0);
		Arsys_line(2, 1, (uint8_t*)imya1_t, 0);
		imya_temp = 0;
	}
}
void menu_init(void)
{
	oled023_1_disp_clear();
	Arsys_ef(2, 0, (uint8_t*)rf, presetData[moog]);
	Arsys_ef(20, 0, (uint8_t*)gt, presetData[gate]);
	Arsys_ef(38, 0, (uint8_t*)cm, presetData[compr]);
	Arsys_ef(56, 0, (uint8_t*)pr, presetData[pream]);
	Arsys_ef(74, 0, (uint8_t*)am, presetData[amp]);                         //   2,20,38,56,74,92,110
	Arsys_ef(92, 0, (uint8_t*)cs, presetData[cab]);
	Arsys_ef(110, 0, (uint8_t*)equ, presetData[eq]);
	Arsys_ef(2, 2, (uint8_t*)ph, presetData[phas]);
	Arsys_ef(20, 2, (uint8_t*)rm, presetData[fl]);
	Arsys_ef(38, 2, (uint8_t*)ch, presetData[chor]);
	Arsys_ef(56, 2, (uint8_t*)dl, presetData[delay]);
	Arsys_ef(74, 2, (uint8_t*)er, presetData[early]);
	Arsys_ef(92, 2, (uint8_t*)rv, presetData[reve]);
	Arsys_ef(110, 2, (uint8_t*)tr, presetData[trem]);
}

void prog_ind(uint32_t val)
{
	val++;
	uint8_t col = 87;
	uint8_t prog_num[2];
	Set_Page_Address(0);
	t33x30_clear(col, 0, 39);
	prog_num[0] = val/10;
	prog_num[1] = val%10;
	for(uint8_t i = 0; i<2; i++)
		col += t33x30_sym(col, 0, prog_num[i]);
}
void clear_str(uint8_t col, uint8_t pag, uint8_t font, uint8_t count)
{
	switch(font)
	{
		case 0:
			t12x13_clear(col, pag, count);
		break;
		case 1:
			t33x30_clear(col, pag, count);
		break;
		case 2:
			Arsys_clean_(col, pag, count);
		break;
	}
}
void Arsys_ef(uint8_t col, uint8_t pag, uint8_t *adr, uint8_t curs)
{
	uint8_t fl = 0;
	uint32_t sym;
	sym = adr[0];
	sym = (sym-32)*6;
	for(uint8_t i = 0; i<8; i++)
	{
		Set_Column_Address(col+i);
		Set_Page_Address(pag);
		if(i>1)
		{
			if((SystemFont5x7[sym+i-2]&0xc0)==0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					oled023_1_write_data((SystemFont5x7[sym+i-2]<<2)|1);
				break;
				case 1:
					oled023_1_write_data((~(SystemFont5x7[sym+i-2]<<2))|3);
					if(fl==1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					oled023_1_write_data(1);
					fl = 0;
				break;
			}
		}
		if((i==1)&&(curs!=1))
			oled023_1_write_data(1);
		else
			oled023_1_write_data(0xff);
		Set_Column_Address(col+i);
		Set_Page_Address(pag+1);
		if(i!=0)
		{
			if(i==1)
			{
				if(curs!=1)
					oled023_1_write_data(4);
				else
					oled023_1_write_data(7);
			}
			else
			{
				if(curs!=1)
					oled023_1_write_data(4+fl);
				else
					oled023_1_write_data((4+fl)|2);
			}
		}
		else
			oled023_1_write_data(7);
	}
	sym = adr[1];
	sym = (sym-32)*6;
	for(uint8_t i = 0; i<7; i++)
	{
		Set_Column_Address(col+i+8);
		Set_Page_Address(pag);
		if(i<6)
		{
			if((SystemFont5x7[sym+i]&0xc0)==0)
				fl = 0;
			else
				fl = 1;
			switch(curs)
			{
				case 0:
					oled023_1_write_data((SystemFont5x7[sym+i]<<2)|1);
				break;
				case 1:
					oled023_1_write_data((~(SystemFont5x7[sym+i]<<2))|3);
					if(fl==1)
						fl = 0;
					else
						fl = 1;
				break;
				case 2:
					oled023_1_write_data(1);
					fl = 0;
				break;
			}
		}
		if(i==6)
			oled023_1_write_data(0xff);
		Set_Column_Address(col+i+8);
		Set_Page_Address(pag+1);
		if(i!=6)
		{
			if(curs!=1)
				oled023_1_write_data(4+fl);
			else
				oled023_1_write_data((4+fl)|2);
		}
		else
			oled023_1_write_data(7);
	}
}
void disp_contr(uint8_t val)
{
	//erc12864_Set_Contrast_Control_Register(val * (22.0/31.0) + 14.0);
}

void sys_menu_init(void)
{

}
void tun_ini(void)
{
	oled023_1_disp_clear();
	scal_tun();
}
void tun_ind(void)
{
	strel_tun();
}
