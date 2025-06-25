#ifndef REALPARAM_H_
#define REALPARAM_H_

#include "baseparam.h"

class RealParam: public BaseParam
{
public:
	RealParam(const char* name, void* paramValuePtr);


	void printParam(uint8_t yPos);

	void setUnits(const char* units, uint8_t strSize);

	void setDisplayBounds(float minDisplayValue, float maxDisplayValue);
	void setDisplayPrecision(uint8_t precision);

private:
	float m_displayValue;
	float m_minDisplayValue;
	float m_maxDisplayValue;

	float m_k1;
	float m_k2;

	uint8_t m_precision{1};

	char m_unitsName[4]{0};

	bool m_unitsVisible{false};
	bool m_signVisible{true};

	void calcDisplayValue();

	static constexpr char spaceSymbol = ' ';
	static constexpr char minusSymbol = 45;
	static constexpr char plusSymbol = 43;
};

#endif /* REALPARAM_H_ */
