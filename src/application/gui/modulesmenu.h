#ifndef MODULESMENU_H_
#define MODULESMENU_H_


#include "abstractmenu.h"
#include "paramlistmenu.h"

class ModulesMenu : public AbstractMenu
{
public:
	ModulesMenu(AbstractMenu* parent);

	void show() override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

private:
	uint8_t num_men = 0;
	uint8_t num_men_temp = 0;

	ParamListMenu gateMenu{this, MENU_GATE};

	void icon_refresh(uint8_t num);

	constexpr static uint8_t cab_list      [][8] ={"Pan","Browser","Volume"};
	constexpr static uint8_t cab_out_list  [][6] ={"1 L+R","1R AP","2 L+R","1R A ","1R P "," 1 R "};
	constexpr static uint8_t cab_list_menu [][9] ={"Cabinet1","Cabinet2"};

	constexpr static uint8_t volum[]="Preset Level";
	constexpr static uint8_t s_to_ch[]="      Save?";

	constexpr static uint8_t cc[]="CC#";
	constexpr static  uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};

	constexpr static uint8_t sd_nr [] = "MicroSD is not ready";
	constexpr static uint8_t imp_dir_n [] = "There is no directory";
	constexpr static uint8_t imp_dir_no [ ]= "IMPULSE";
	constexpr static uint8_t sd_lo [] = "MicroSD is loading..";

	constexpr static uint8_t moog_list     [][8] ={"Mix","F Type","F Mod","LFO r","Lo freq","Hi freq","Res","Dyn th","Dyn att","Dyn rel","Volume","LFO_Typ"};
	constexpr static uint8_t moog_typ_list [][4] ={"LPF","HPF","BPF"};
	constexpr static uint8_t moog_mod_list [][4] ={"LFO","Dyn","Ext"};
	constexpr static uint8_t moog_gen_type [][7] ={"Tri   ","Rand  ","Rand/2","Rand/3","Rand/4","Rand/6","Rand/8"};
	constexpr static uint8_t phas_list     [][8] ={"Mix","Rate","Center","Width","F_Back","Stage","HPF","Pos"};
	constexpr static uint8_t chor_list     [][8] ={"Mix","Rate","Width","Delay","Type","HPF"};
	constexpr static uint8_t chor_list1    [][8] ={"Mix","Detune"," --"," --","Type","HPF"};
	constexpr static uint8_t chor_list2    [][8] ={"Mix"," --"," --","Delay","Type","HPF"};
	constexpr static uint8_t fl_list       [][8] ={"Mix","LFO","Rate","Width","Delay","F_Back","HPF","Pos"};
	constexpr static uint8_t fl_t          [][9] ={"Triangle","Sinus   "};
	constexpr static uint8_t ch_list_t     [][12]={"  Chorus   ","  Chorus S "," Chorus x3 ","Chorus x3 S","  Detune   ","MidSide Dub"};
	constexpr static uint8_t del_list      [][8] ={"Mix","Time","F_Back","LPF","HPF","D_Pan","D2 Vol","D2 Pan","D->D2","D_Mod","M_Rate","Direct"};
	constexpr static uint8_t del_tim_l     [][5] ={"Time","TAP","Tail"};
	constexpr static uint8_t tap_tim       [][6] ={"1/1  ","1/1.5","1/2  ","1/3  ","1/4  ","2/1  "};
	constexpr static float   tap_tim_v     [6]   ={1.0f,1.5f,2.0f,3.0f,4.0f,0.5f};
	constexpr static uint8_t rev_list      [][7] ={"Mix","Type","Time","Size","Damp","LPF","HPF","Detune","Diffus","PreD","Tail"};
	constexpr static uint8_t rev_type_list [][8] ={"Default","Hall   ","Room   ","Plate  ","Spring ","Gate   ","Reverse"};
	constexpr static uint8_t ear_list      [][5] ={"Mix","Size"};
	constexpr static uint8_t s_t_c_list    [][7] ={"  No  ","Return","   Yes"};
	constexpr static uint8_t tr_list       [][8] ={"Intens","Rate","LFOtype","LFO mod","Type","TAP"};
	constexpr static uint8_t tr_t_list     [][7] ={"Sin   ","Square","Mono  ","Stereo"};
	constexpr static uint8_t tr_lfo_t_list [][9] ={"Sin     ","Square  ","Sawtooth"};
	//	constexpr static uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};
};


#endif /* MODULESMENU_H_ */
