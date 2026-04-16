#ifndef _EXPRESSIONMENU_H_
#define _EXPRESSIONMENU_H_

#include "paramlistmenu.h"

class ExpressionMenu: public ParamListMenu
{
public:
	ExpressionMenu(AbstractMenu* parent);

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	static AbstractMenu* create(AbstractMenu* parent);

private:
	enum TCalibrationState
	{
		CalibrationIdle,
		CalibrationMin,
		CalibrationMax
	};
	
	static TCalibrationState m_calibrationState;

	static void calibratePrintCallback(void* parameter);

	static void exprTypePrintCallback(void* parameter);
	static void exprTypeIncreaseCallback(void* parameter);
	static void exprTypeDecreaseCallback(void* parameter);

	static void ccParamPrintCallback(void* parameter);
};

#endif /* _EXPRESSIONMENU_H_ */
