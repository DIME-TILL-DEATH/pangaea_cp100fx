#ifndef MODULESMENU_H_
#define MODULESMENU_H_


#include "abstractmenu.h"
#include "paramlistmenu.h"
#include "submenuparam.h"

class ModulesMenu : public AbstractMenu
{
public:
	ModulesMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
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

	ParamListMenu* createRfMenu();
	ParamListMenu* createGateMenu();
	ParamListMenu* createCompressorMenu();
	ParamListMenu* createPreampMenu();
	ParamListMenu* createAmpMenu();
	ParamListMenu* createPhaserMenu();
	ParamListMenu* createFlangerMenu();
	ParamListMenu* createChorusMenu();
	static AbstractMenu* createEarlyMenu(AbstractMenu* parentMenu);
	static ParamListMenu* createDelayMenu(AbstractMenu* parentMenu);
	ParamListMenu* createReverbMenu();
	ParamListMenu* createTremoloMenu();

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

	constexpr static uint8_t s_t_c_list    [][7] ={"  No  ","Return","   Yes"};

	//	constexpr static uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};
};


#endif /* MODULESMENU_H_ */
