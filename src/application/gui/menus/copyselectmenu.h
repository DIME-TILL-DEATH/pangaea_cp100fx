#ifndef COPYSELECTMENU_H_
#define COPYSELECTMENU_H_

#include "abstractmenu.h"
#include "preset.h"

class CopySelectMenu: public AbstractMenu
{
public:
	CopySelectMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void setTargetPreset(uint8_t targetPresetNum) {m_targetPresetNum = targetPresetNum;}

private:
	typedef enum
	{
		StringLong,
		StringShort,
		ActionOk,
		ActionReturn,
		ActionSelectAll,
		ActionDeselectAll
	}ElementType;

	typedef struct
	{
		ElementType type;
		uint8_t* checked_ptr;
		const char* name;
	}SelectionElement;

	static constexpr uint8_t ElementsCount = 23;
	static constexpr uint8_t ElementsOnPage = 8;
	SelectionElement element[ElementsCount];

	static Preset::TSelectionMask m_selectionMask;

	uint8_t m_elementNum{0};

	uint8_t m_copied;
	uint8_t m_targetPresetNum;

	void printElement(const SelectionElement& element, uint8_t numOnPage, bool highlight);
	uint8_t elementIncrementIndex(const SelectionElement& element);
	void printPage();
};

#endif /* SRC_APPLICATION_GUI_MENUS_COPYSELECTMENU_H_ */
