#ifndef REALPARAM_H_
#define REALPARAM_H_

#include "baseparam.h"

class RealParam: public BaseParam
{
public:
	RealParam(const char* name, void* paramValuePtr);

	enum TIndicatorType{
		None,
		Bar,
		Pan
	};

	void increaseParam();
	void decreaseParam();

	void printParam(uint8_t yPos);

	void setDisplayStep(float step);
	void setDisplayBounds(float m_minDisplayValue, float m_maxDisplayValue);

private:
	float m_displayValue;
	float m_minDisplayValue;
	float m_maxDisplayValue;

	float m_k1;
	float m_k2;

	uint8_t m_precision{0};

	char m_unitsName[4]{0};

	bool m_unitsVisible{false};
	bool m_signVisible{true};

	void calcDisplayValue();

	TIndicatorType m_indicatorType{TIndicatorType::None};

	static constexpr char spaceSymbol = ' ';
	static constexpr char minusSymbol = 45;
	static constexpr char plusSymbol = 43;
};

#endif /* REALPARAM_H_ */
