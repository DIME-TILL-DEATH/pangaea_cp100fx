#ifndef PARAMLISTMENU_H_
#define GUI_PARAMLISTMENU_H_

#include "abstractmenu.h"
#include "baseparam.h"

class ParamListMenu : public AbstractMenu
{
public:
	ParamListMenu(AbstractMenu* parent, gui_menu_type menuType);
	~ParamListMenu();

	void setParams(BaseParam** settlingParamList, uint8_t setlingParamCount, uint8_t pages);

	void show() override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

private:
	uint8_t currentParamNum = 0;

	BaseParam* paramsList[64];
	uint8_t paramsCount{0};

	int8_t currentPageNumber{-1};

	uint8_t pagesCount;

	bool encoderKnobSelected;

	static constexpr uint8_t paramsOnPage = 4;

	void printPage();
};


#endif
