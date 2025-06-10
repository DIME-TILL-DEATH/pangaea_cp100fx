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
		GUI_PARAMETER_LEVEL,
		GUI_PARAMETER_PAN,
		GUI_PARAMETER_LIST,
		GUI_PARAMETER_SUBMENU
	};


	BaseParam(gui_param_type paramType, const char* name, uint8_t* paramValuePtr);

	void setDspAddress(dsp_module_address_t moduleAddress, uint8_t bytePosition);
	void setDisplayPosition(uint8_t xCoord);

	gui_param_type type() const {return m_type;};

	const char* name() const {return m_name;};
	uint8_t* valuePtr() const {return m_valuePtr;};

	dsp_module_address_t moduleAddress() const {return m_moduleAddress;};
	uint8_t bytePosition() const {return m_bytePosition;};

	uint8_t minValue() const {return m_minValue;};
	uint8_t maxValue() const {return m_maxValue;};

	uint8_t xDisplayPosition() const {return m_xDisplayPosition;};

	uint8_t byteSize() const {return m_byteSize;};

private:
	gui_param_type m_type{GUI_PARAMETER_DUMMY};

	const char* m_name;
	uint8_t* m_valuePtr;

	dsp_module_address_t m_moduleAddress;
	uint8_t m_bytePosition{0};

	uint8_t m_minValue{0};
	uint8_t m_maxValue{127};

	uint8_t m_xDisplayPosition{58};

	uint8_t m_byteSize{1};
};



#endif /* BASEPARAM_H_ */
