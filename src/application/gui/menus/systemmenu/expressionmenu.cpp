#include "expressionmenu.h"

#include "adc.h"
#include "gpio.h"
#include "periphery.h"

#include "system.h"
#include "footswitch.h"

#include "icons_bitmap.h"

#include "sharc_task.h"
#include "display_task.h"

#include "baseparam.h"
#include "customparam.h"
#include "stringlistparam.h"

#include "syssettings_handlers.h"

ExpressionMenu::TCalibrationState ExpressionMenu::m_calibrationState = CalibrationIdle;

ExpressionMenu::ExpressionMenu(AbstractMenu* parent)
	:ParamListMenu(parent, MENU_EXPRESSION)
{

}

void ExpressionMenu::encoderPressed()
{
	if(m_currentParamNum == 1)
	{
		switch(m_calibrationState)
		{
			case CalibrationIdle:
			{
				m_calibrationState = CalibrationMin;
				DisplayTask->StringOut(66, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Set to min");
				break;
			}
			
			case CalibrationMin:
			{
				sys_para[System::EXPR_CAL_MIN_HI] = adc_bu;
				sys_para[System::EXPR_CAL_MIN_LO] = adc_bu>>8;
				ADC_LoadCal();
				
				DisplayTask->ClearString(66, 1, Font::fntSystem, 10);
				DisplayTask->StringOut(92, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Ok");
				HW_Delay(0x7fffff);
				
				m_calibrationState = CalibrationMax;
				DisplayTask->StringOut(66, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Set to max");
				break;
			}
			
			case CalibrationMax:
			{
				sys_para[System::EXPR_CAL_MAX_HI] = adc_bu;
				sys_para[System::EXPR_CAL_MAX_LO] = adc_bu>>8;
				ADC_LoadCal();
				
				DisplayTask->ClearString(66, 1, Font::fntSystem, 10);
				DisplayTask->StringOut(92, 1, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Ok");
				HW_Delay(0x7fffff);
				
				m_calibrationState = CalibrationIdle;
				DisplayTask->ClearString(92, 1, Font::fntSystem, 2);
				calibratePrintCallback((uint8_t*)&adc_bu2);
				break;
			}
		}
		
		restartBlinking(0);
	}
	else
	{
		ParamListMenu::encoderPressed();
	}
}

void ExpressionMenu::encoderClockwise()
{
	if(m_calibrationState == CalibrationIdle)
		ParamListMenu::encoderClockwise();
}

void ExpressionMenu::encoderCounterClockwise()
{
	if(m_calibrationState == CalibrationIdle)
		ParamListMenu::encoderCounterClockwise();
}

AbstractMenu* ExpressionMenu::create(AbstractMenu* parent)
{
	ExpressionMenu* expressionMenu = new ExpressionMenu(parent);

	const uint8_t paramNum = 4;
	BaseParam* params[paramNum];

	CustomParam* exprTypeParam = new CustomParam(CustomParam::TDisplayType::Custom, &SysSettingsDesc.exprType);
	exprTypeParam->increaseCallback = exprTypeIncreaseCallback;
	exprTypeParam->decreaseCallback = exprTypeDecreaseCallback;
	exprTypeParam->printCallback = exprTypePrintCallback;
	exprTypeParam->setDisplayPosition(60);
	params[0] = exprTypeParam;

	CustomParam* calibrateMinParam = new CustomParam(CustomParam::TDisplayType::Custom, &SysSettingsDesc.exprCalibrate);
	calibrateMinParam->printCallback = calibratePrintCallback;
	params[1] = calibrateMinParam;

	CustomParam* ccParam = new CustomParam(CustomParam::TDisplayType::Custom, &SysSettingsDesc.exprCc);
	ccParam->printCallback = ccParamPrintCallback;
	ccParam->setDisplayPosition(84);
	params[2] = ccParam;

	StringListParam* levelParam = new StringListParam(&SysSettingsDesc.exprLev, {"Off", "On "}, 4);
	levelParam->setDisplayPosition(84);
	params[3] = levelParam;

	expressionMenu->setParams(params, paramNum);
	expressionMenu->setIcon(false, ICON_NONE);
	return expressionMenu;
}

void ExpressionMenu::exprTypePrintCallback(void* parameter)
{
	static constexpr uint8_t strExprType[][12] = {"   Off     ", "Std. Volume", "Alt. Volume", "Std. CC    ", "Alt. CC    "};
	uint8_t* ptr = static_cast<uint8_t*>(parameter);
	DisplayTask->StringOut(60, 0, Font::fntSystem, Font::fnsNormal, (uint8_t*)&strExprType[*ptr & 0x7f]);
}

void ExpressionMenu::exprTypeIncreaseCallback(void* parameter)
{
	if((sys_para[System::EXPR_TYPE]&0x7f) < EXPR_TYPE_CC_ALT)
		sys_para[System::EXPR_TYPE]++;
}

void ExpressionMenu::exprTypeDecreaseCallback(void* parameter)
{
	if((sys_para[System::EXPR_TYPE]&0x7f) > EXPR_TYPE_VOL_STD)
		sys_para[System::EXPR_TYPE]--;
}

void ExpressionMenu::ccParamPrintCallback(void* parameter)
{
	uint8_t* ptr = static_cast<uint8_t*>(parameter);
	if(!*ptr)
		DisplayTask->StringOut(84, 2, Font::fntSystem, Font::fnsNormal, (uint8_t*)"Off");
	else
		DisplayTask->ParamIndNum(84, 2, *ptr-1);
}

void ExpressionMenu::calibratePrintCallback(void* parameter)
{
	if(m_calibrationState == CalibrationIdle)
		DisplayTask->ParamInd(66, 1, adc_bu2);
}
