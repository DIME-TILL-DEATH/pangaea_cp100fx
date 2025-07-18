#ifndef BASEPARAM_H_
#define BASEPARAM_H_

#include "appdefs.h"
#include "modules.h"
#include "BF706_send.h"

class BaseParam
{
public:
	enum gui_param_type
	{
		GUI_PARAMETER_DUMMY,
		GUI_PARAMETER_NUM,
		GUI_PARAMETER_LEVEL,
		GUI_PARAMETER_VOLUME,
		GUI_PARAMETER_MIX,
		GUI_PARAMETER_PAN,
		GUI_PARAMETER_LIST,
//		GUI_PARAMETER_DELAY_TIME,
		GUI_PARAMETER_SUBMENU_DELAY_TIME,
		GUI_PARAMETER_SUBMENU,
		GUI_PARAMETER_CUSTOM,
		GUI_PARAMETER_REAL,
		GUI_PARAMETER_STRING_OUT
	};

	// Сделан только для RealParam
	enum TIndicatorType{
		IndNone,
		IndBarFilled,
		IndBarTransparent,
		IndMix,
		IndPan
	};

	BaseParam(gui_param_type paramType, const char* name, void* paramValuePtr);
	virtual ~BaseParam() {};

	gui_param_type type() const {return m_type;};
	virtual const char* name();
	uint8_t* valuePtr() const {return m_valuePtr;};

	virtual uint32_t value() const;
	virtual void increaseParam();
	virtual void decreaseParam();

	virtual void printParam(uint8_t yPos);

	virtual void setToDsp();

	void setDspAddress(dsp_module_address_t moduleAddress, uint8_t bytePosition);
	dsp_module_address_t moduleAddress() const {return m_moduleAddress;};
	uint8_t bytePosition() const {return m_bytePosition;};

	bool disabled() {return m_disabled;};
	void setDisabled(bool disabled) {m_disabled = disabled;};

	void setBounds(int32_t minBound, int32_t maxBound);
	int32_t minValue() const {return m_minValue;};
	int32_t maxValue() const {return m_maxValue;};

	void setScaling(uint8_t stepSize, int32_t offset);
	int32_t offset() const {return m_offset;};
	uint8_t stepSize() {return m_stepSize;};

	void setInverse(bool isInverse);
	bool inverse() {return m_inverse;}

	void setDisplayPosition(uint8_t xCoord);
	uint8_t xDisplayPosition() const {return m_xDisplayPosition;};

	void setByteSize(uint8_t size);
	uint8_t byteSize() const {return m_byteSize;};

	void setIndicatorType(TIndicatorType indicatorType);

	static int16_t encSpeedInc(int16_t data, int16_t max, uint8_t stepSize = 1);
	static int16_t encSpeedDec(int16_t data, int16_t min, uint8_t stepSize = 1);

protected:
	const char* m_name;

	TIndicatorType m_indicatorType{IndNone};

	bool m_disabled{false};
	bool m_inverse{false};

	dsp_module_address_t m_moduleAddress;
	uint8_t m_bytePosition{NOT_SEND_POS};

	uint8_t m_xDisplayPosition{58};

	uint8_t m_byteSize{1};
	int32_t m_offset{0};
	uint8_t m_stepSize{1};

	gui_param_type m_type{GUI_PARAMETER_DUMMY};

	uint8_t* m_valuePtr;

	int32_t m_minValue{0};
	int32_t m_maxValue{127};

	void encoderSpeedIncrease();
	void encoderSpeedDecrease();
};



#endif /* BASEPARAM_H_ */
