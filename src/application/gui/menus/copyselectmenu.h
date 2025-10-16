#ifndef SRC_APPLICATION_GUI_MENUS_COPYSELECTMENU_H_
#define SRC_APPLICATION_GUI_MENUS_COPYSELECTMENU_H_

#include "abstractmenu.h"

class CopySelectMenu: public AbstractMenu
{
public:
	typedef struct
	{
		uint8_t name;
		uint8_t comment;
		uint8_t controllers;
		uint8_t rf;
		uint8_t gt;
		uint8_t cm;
		uint8_t pr;
		uint8_t pa;
		uint8_t ir;
		uint8_t eq;
		uint8_t fl;
		uint8_t ph;
		uint8_t ch;
		uint8_t dl;
		uint8_t er;
		uint8_t rv;
		uint8_t tr;
		uint8_t pv;
		uint8_t att;
	}TSelectionMask;

	CopySelectMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void setTargetPreset(uint8_t targetPresetNum) {m_targetPresetNum = targetPresetNum;}
	static void copyPreset(const TSelectionMask& selectionMask, uint8_t targetPresetNum);

private:
	typedef enum
	{
		StringLong,
		StringShort,
		ActionOk,
		ActionReturn
	}ElementType;

	typedef struct
	{
		ElementType type;
		uint8_t* checked_ptr;
		const char* name;
	}SelectionElement;

	static constexpr uint8_t ElementsCount = 21;
	static constexpr uint8_t ElementsOnPage = 8;
	SelectionElement element[ElementsCount];

	static TSelectionMask m_selectionMask;

	uint8_t m_elementNum{0};

	uint8_t m_copied;
	uint8_t m_targetPresetNum;

	void printElement(const SelectionElement& element, uint8_t numOnPage, bool highlight);
	uint8_t elementIncrementIndex(const SelectionElement& element);
	void printPage();
};

#endif /* SRC_APPLICATION_GUI_MENUS_COPYSELECTMENU_H_ */
