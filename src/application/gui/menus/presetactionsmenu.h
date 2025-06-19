#ifndef PRESETACTIONSMENU_H_
#define PRESETACTIONSMENU_H_

#include "abstractmenu.h"

class PresetActionsMenu : public AbstractMenu
{
public:
	enum TActionType
	{
		Save,
		Copy
	};

	PresetActionsMenu(AbstractMenu* parent, TActionType actionType);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

private:
	TActionType m_actionType;

	uint8_t targetPresetNum = 0;

	void updatePresetData();
	void copyPreset();
	void savePreset();
};



#endif /* PRESETACTIONSMENU_H_ */
