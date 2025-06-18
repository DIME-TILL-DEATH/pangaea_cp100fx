#ifndef PARAMLISTMENU_H_
#define GUI_PARAMLISTMENU_H_

#include "abstractmenu.h"

#include "../../display.h"

#include "BF706_send.h"
#include "modules.h"

#include "params/baseparam.h"
#include "params/stringparam.h"
#include "params/submenuparam.h"

class ParamListMenu : public AbstractMenu
{
public:
	ParamListMenu(AbstractMenu* parent, gui_menu_type menuType);
	~ParamListMenu();

	void setParams(BaseParam** settlingParamList, uint8_t setlingParamCount, uint8_t pages);
	void setVolumeIndicator(TDisplayTask::TVolIndicatorType volIndicatorType, dsp_indicator_source_t indicatorSource);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyDown() override;

	static constexpr uint8_t maxParamCount = 16;

private:
	uint8_t m_currentParamNum = 0;

	BaseParam* m_paramsList[maxParamCount];
	uint8_t m_paramsCount{0};

	bool m_canTap{false};

	int8_t m_currentPageNumber{-1};

	uint8_t m_pagesCount;

	bool m_encoderKnobSelected;

	TDisplayTask::TVolIndicatorType m_volIndicatorType{TDisplayTask::VOL_INDICATOR_OFF};
	dsp_indicator_source_t m_indicatorSource;

	static constexpr uint8_t paramsOnPage = 4;
	static constexpr uint8_t leftPad = 3;

	void printPage();

//	void printParam(BaseParam* param, uint8_t yPos);
};


#endif
