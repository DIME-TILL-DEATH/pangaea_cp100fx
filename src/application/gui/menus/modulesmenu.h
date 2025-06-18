#ifndef MODULESMENU_H_
#define MODULESMENU_H_


#include "abstractmenu.h"
#include "copymenu.h"


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

	uint8_t numMenu() {return m_numMenu;};

	static void enableCab();

private:
	uint8_t m_numMenu = 0;

	CopyMenu copyMenu{this};

	void icon_refresh(uint8_t num);

	constexpr static uint8_t volum[]="Preset Level";
	constexpr static uint8_t s_to_ch[]="      Save?";

	constexpr static uint8_t cc[]="CC#";
	constexpr static  uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};

	constexpr static uint8_t sd_nr [] = "MicroSD is not ready";
	constexpr static uint8_t imp_dir_n [] = "There is no directory";
	constexpr static uint8_t imp_dir_no [ ]= "IMPULSE";
	constexpr static uint8_t sd_lo [] = "MicroSD is loading..";

	constexpr static uint8_t s_t_c_list    [][7] ={"  No  ","Return","   Yes"};
};

#endif /* MODULESMENU_H_ */
