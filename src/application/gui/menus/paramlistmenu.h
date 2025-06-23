#ifndef PARAMLISTMENU_H_
#define PARAMLISTMENU_H_

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

	void setParams(BaseParam** settlingParamList, uint8_t setlingParamCount);
	void setVolumeIndicator(TDisplayTask::TVolIndicatorType volIndicatorType, dsp_indicator_source_t indicatorSource);

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	virtual void encoderPressed() override;
	virtual void encoderClockwise() override;
	virtual void encoderCounterClockwise() override;

	virtual void keyDown() override;

	static constexpr uint8_t maxParamCount = 16;

	void setIcon(bool drawIcon, uint8_t iconNumber);

//	void (*key1Callback)(AbstractMenu* thisObject);
//	void (*key2Callback)(AbstractMenu* thisObject);
//	void (*key3Callback)(AbstractMenu* thisObject);
//	void (*key4Callback)(AbstractMenu* thisObject);
//	void (*key5Callback)(AbstractMenu* thisObject);

protected:
	uint8_t m_currentParamNum = 0;

	BaseParam* m_paramsList[maxParamCount];
	uint8_t m_paramsCount{0};

	bool m_canTap{false};
	bool m_drawIcon{true};

	int8_t m_currentPageNumber{-1};

	uint8_t m_pagesCount;

	bool m_encoderKnobSelected;

	TDisplayTask::TVolIndicatorType m_volIndicatorType{TDisplayTask::VOL_INDICATOR_OFF};
	dsp_indicator_source_t m_indicatorSource;

	static constexpr uint8_t paramsOnPage = 4;
	static constexpr uint8_t leftPad = 3;

	void printPage(bool forceDrawIcon = false);
};


#endif
