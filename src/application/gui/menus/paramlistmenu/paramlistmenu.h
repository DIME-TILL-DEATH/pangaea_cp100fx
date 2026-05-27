#ifndef PARAMLISTMENU_H_
#define PARAMLISTMENU_H_

#include "abstractmenu.h"

#include "sharc.h"

#include "system.h"
#include "modules.h"

#include "display_task.h"

#include "bitmaps.h"
#include "icons_bitmap.h"

#include "baseparam.h"
#include "stringlistparam.h"
#include "submenuparam.h"

class ParamListMenu : public AbstractMenu
{
public:
	ParamListMenu(AbstractMenu* parent, gui_menu_type menuType);
	~ParamListMenu() override;

	void setParams(BaseParam** settlingParamList, uint8_t setlingParamCount);
	void setVolumeIndicator(TVolIndicatorType indicatorType,
			dsp_indicator_source_t indicatorSource, uint8_t* indicatorParPtr = nullptr);

	virtual void show(TShowMode showMode = FirstShow) override;
	virtual void task() override;
	virtual void refresh() override;

	virtual void encoderPressed() override;
	virtual void encoderClockwise() override;
	virtual void encoderCounterClockwise() override;

	virtual void keyDown() override;

	static constexpr uint8_t maxParamCount = 16;

	void setIcon(bool drawIcon, icon_t icon);
	void setTapDestination(System::TapDestination tapDst) { m_tapDst = tapDst; }

	static constexpr uint8_t paramsOnPage = 4;
	static constexpr uint8_t leftPad = 3;

//	void (*key1Callback)(AbstractMenu* thisObject);
//	void (*key2Callback)(AbstractMenu* thisObject);
//	void (*key3Callback)(AbstractMenu* thisObject);
//	void (*key4Callback)(AbstractMenu* thisObject);
//	void (*key5Callback)(AbstractMenu* thisObject);

protected:
	uint8_t m_currentParamNum = 0;

	BaseParam* m_paramsList[maxParamCount];
	uint8_t m_paramsCount{0};

	uint8_t m_firstSelectableParam;
	uint8_t m_lastSelectableParam;

	System::TapDestination m_tapDst{System::TapDestination::TAP_OFF};
	bool m_drawIcon{true};

	int8_t m_currentPageNumber{-1};

	uint8_t m_pagesCount;

	icon_t m_icon{ICON_NONE};

	bool m_encoderKnobSelected;

	TVolIndicatorType m_volIndicatorType{TVolIndicatorType::VOL_INDICATOR_OFF};
	dsp_indicator_source_t m_indicatorSource;
	uint8_t* m_indicatorParam_ptr;

	void printPage(bool forceDrawIcon = false);
};


#endif
