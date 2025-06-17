#ifndef PARAMLISTMENU_H_
#define GUI_PARAMLISTMENU_H_

#include "abstractmenu.h"
#include "baseparam.h"
#include "stringparam.h"
#include "submenuparam.h"

class ParamListMenu : public AbstractMenu
{
public:
	ParamListMenu(AbstractMenu* parent, gui_menu_type menuType);
	~ParamListMenu();

	void setParams(BaseParam** settlingParamList, uint8_t setlingParamCount, uint8_t pages);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyDown() override;

	static constexpr uint8_t maxParamCount = 16;

private:
	uint8_t currentParamNum = 0;

	BaseParam* paramsList[maxParamCount];
	uint8_t paramsCount{0};

	bool m_canTap{false};

	int8_t currentPageNumber{-1};

	uint8_t pagesCount;

	bool encoderKnobSelected;

	static constexpr uint8_t paramsOnPage = 4;
	static constexpr uint8_t leftPad = 3;

	void printPage();

	void printParam(BaseParam* param, uint8_t yPos);
};


#endif
