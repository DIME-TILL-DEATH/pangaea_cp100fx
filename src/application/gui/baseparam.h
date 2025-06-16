#ifndef BASEPARAM_H_
#define BASEPARAM_H_

#include "appdefs.h"
#include "../modules.h"

class BaseParam
{
public:
	enum gui_param_type
	{
		GUI_PARAMETER_DUMMY,
		GUI_PARAMETER_NUM,
		GUI_PARAMETER_LEVEL,
		GUI_PARAMETER_MIX,
		GUI_PARAMETER_PAN,
		GUI_PARAMETER_LIST,
		GUI_PARAMETER_DELAY_TIME,
		GUI_PARAMETER_SUBMENU
	};


	BaseParam(gui_param_type paramType, const char* name, uint8_t* paramValuePtr);
	virtual ~BaseParam() {};

	void setDspAddress(dsp_module_address_t moduleAddress, uint8_t bytePosition);
	void setDisplayPosition(uint8_t xCoord);

	gui_param_type type() const {return m_type;};

	const char* name();
	virtual uint8_t value() const {return *m_valuePtr + m_offset;};
	uint8_t* valuePtr() const {return m_valuePtr;};

	bool disabled() {return m_disabled;};
	void setDisabled(bool disabled) {m_disabled = disabled;};

	dsp_module_address_t moduleAddress() const {return m_moduleAddress;};
	uint8_t bytePosition() const {return m_bytePosition;};

	void setBounds(uint8_t minBound, uint8_t maxBound);
	uint8_t minValue() const {return m_minValue;};
	uint8_t maxValue() const {return m_maxValue;};

	void setScaling(uint8_t stepSize, uint8_t offset);
	uint8_t offset() const {return m_offset;};
	uint8_t stepSize() {return m_stepSize;};

	uint8_t xDisplayPosition() const {return m_xDisplayPosition;};

	uint8_t byteSize() const {return m_byteSize;};

private:
	const char* m_name;

	bool m_disabled{false};

	uint8_t* m_valuePtr;

	dsp_module_address_t m_moduleAddress;
	uint8_t m_bytePosition{0};

	uint8_t m_xDisplayPosition{58};

	uint8_t m_byteSize{1};
	uint8_t m_offset{0};
	uint8_t m_stepSize{1};

protected:
	gui_param_type m_type{GUI_PARAMETER_DUMMY};

	uint8_t m_minValue{0};
	uint8_t m_maxValue{127};
};



#endif /* BASEPARAM_H_ */
