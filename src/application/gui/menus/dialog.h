#ifndef DIALOG_H_
#define DIALOG_H_

#include "abstractmenu.h"

class Dialog: public AbstractMenu
{
public:
	enum TDialogType
	{
		SaveChanges,
		ErasePreset,
		RestartDevice
	};

	Dialog(AbstractMenu* parent, TDialogType dialogType = SaveChanges);
	~Dialog();

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void setYesMenu(AbstractMenu* yesMenu);
	void setNoMenu(AbstractMenu* noMenu);

	void keyUp() override {};
	void keyDown() override {};

private:
	AbstractMenu* m_yesMenu{nullptr};
	AbstractMenu* m_noMenu{nullptr};

	TDialogType m_dialogType;

	uint8_t m_paramNum;
	uint8_t m_btnPositions[3];

	char m_questionString1[20]{0};
	char m_questionString2[20]{0};

	char** m_btnNames;
	uint8_t m_btnCount{3};
	uint8_t m_nameLenght{7};

	uint8_t strPos{10};
};

#endif /* DIALOG_H_ */
