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
	}TSelectionMask;

	CopySelectMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	typedef enum
	{
		StringLong,
		StringShort,
		ActionOk,
		ActionReturn
	}ElementType;

	typedef enum
	{
		Unchecked,
		Checked
	}ElementState;

	typedef struct
	{
		ElementType type;
		ElementState state;
		uint8_t* value_ptr;
		const char* name;
	}SelectionElement;

	static constexpr uint8_t ElementsCount = 19;
	static constexpr uint8_t ElementsOnPage = 8;
	SelectionElement element[ElementsCount];

	TSelectionMask m_selectionMask;

	uint8_t m_elementNum{0};

	void printElement(const SelectionElement& element, uint8_t numOnPage, bool highlight);
	uint8_t elementIncrementIndex(const SelectionElement& element);
	void printPage();
};

#endif /* SRC_APPLICATION_GUI_MENUS_COPYSELECTMENU_H_ */
