#ifndef MODULESMENU_H_
#define MODULESMENU_H_

#include "abstractmenu.h"
#include "nameeditmenu.h"
#include "presetactionsmenu.h"
#include "dialog.h"

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

	PresetActionsMenu copyMenu{this, PresetActionsMenu::Copy};
	PresetActionsMenu confirmSaveMenu{this, PresetActionsMenu::Save};
	Dialog saveDialog{this, Dialog::SaveChanges};
	Dialog erasePresetDialog{this, Dialog::ErasePreset};
	NameEditMenu nameEditMenu{this};

	void icon_refresh(uint8_t num);
	bool presetEdited{false};

	constexpr static uint8_t volum[]="Preset Level";


	constexpr static uint8_t cc[]="CC#";
	constexpr static  uint8_t contr_list    [][7] ={"Contr","Source","Destin","MinVal","MaxVal","PC Out","SET"};

	constexpr static uint8_t sd_nr [] = "MicroSD is not ready";
	constexpr static uint8_t imp_dir_n [] = "There is no directory";
	constexpr static uint8_t imp_dir_no [ ]= "IMPULSE";
	constexpr static uint8_t sd_lo [] = "MicroSD is loading..";
};

#endif /* MODULESMENU_H_ */
