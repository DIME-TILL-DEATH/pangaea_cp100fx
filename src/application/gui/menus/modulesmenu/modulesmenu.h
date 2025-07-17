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
	void refresh() override;
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

	static void enableCab(ModulesMenu* parent);

	static constexpr uint8_t modulesCount = 14;

private:
	uint8_t m_numMenu = 0;
	bool presetEdited{false};

	PresetActionsMenu confirmSaveMenu{this, PresetActionsMenu::Save};
	Dialog saveDialog{this, Dialog::SaveChanges};

	void iconRefresh(uint8_t num);
};

#endif /* MODULESMENU_H_ */
