#include "expressionmenu.h"

#include "cs.h"
#include "allFonts.h"
#include "icon_bit.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

#include "system.h"

#include "BF706_send.h"

#include "midi_send.h"


const uint8_t ExpressionMenu::strOk[];
const uint8_t ExpressionMenu::strSetMin[];
const uint8_t ExpressionMenu::strSetMmax[];
const uint8_t ExpressionMenu::strExprType[][12];


ExpressionMenu::ExpressionMenu(AbstractMenu *parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_EXPRESSION;
}

void ExpressionMenu::show(TShowMode swhoMode)
{
	currentMenu = this;
	m_menuState = ParamChoice;

	DisplayTask->Clear();
	for(uint8_t i = 0; i<4; i++)
	{
		DisplayTask->StringOut(3, i, Font::fntSystem, 0, (uint8_t*)&expr_menu[i]);
	}

	DisplayTask->StringOut(60, 0, Font::fntSystem, 0, (uint8_t*)&strExprType[sys_para[System::EXPR_TYPE] & 0x7f]);

	if(!sys_para[System::EXPR_CCN])
		DisplayTask->StringOut(84, 2, Font::fntSystem, 0, (uint8_t*)"Off");
	else
		DisplayTask->ParamIndicNum(84, 2, sys_para[System::EXPR_CCN]-1);

	if(!sys_para[System::EXPR_STORE_LEVEL])
		DisplayTask->StringOut(84, 3, Font::fntSystem, 0, (uint8_t*)"Off");
	else
		DisplayTask->StringOut(84, 3, Font::fntSystem, 0, (uint8_t*)"On ");
}

void ExpressionMenu::task()
{
	switch(m_menuState)
	{
		case ParamChoice:
			DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 2*blinkFlag_fl, (uint8_t*)&expr_menu[m_parNum]);
		break;

		case ParamTuning: break;

		case CalMin:
			DisplayTask->StringOut(67, 1, Font::fntSystem, 2*blinkFlag_fl, (uint8_t*)strSetMin);
		break;

		case CalMax:
			DisplayTask->StringOut(67, 1, Font::fntSystem, 2*blinkFlag_fl, (uint8_t*)strSetMmax);
		break;
	}
}

void ExpressionMenu::encoderPressed()
{
	switch(m_menuState)
	{
		case ParamChoice:
			if(m_parNum == 1)
			{
				m_menuState = CalMin;
				DisplayTask->StringOut(67, 1, Font::fntSystem, 3, (uint8_t*)strSetMin);
			}
			else
			{
				m_menuState = ParamTuning;
			}
		break;

		case ParamTuning:
			m_menuState = ParamChoice;
		break;

		case CalMin:
			sys_para[System::EXPR_CAL_MIN_HI] = adc_bu;
			sys_para[System::EXPR_CAL_MIN_LO] = adc_bu>>8;

			DisplayTask->Clear_str(67, 1, Font::fntSystem, 10);
			DisplayTask->StringOut(92, 1, Font::fntSystem, 0, (uint8_t*)strOk);
			dela(0x7fffff);
			DisplayTask->StringOut(67, 1, Font::fntSystem, 2, (uint8_t*)strSetMmax);
			adc_calib();

			m_menuState = CalMax;
		break;

		case CalMax:
			sys_para[System::EXPR_CAL_MAX_HI] = adc_bu;
			sys_para[System::EXPR_CAL_MAX_LO] = adc_bu>>8;

			DisplayTask->Clear_str(67, 1, Font::fntSystem, 10);
			DisplayTask->StringOut(92, 1, Font::fntSystem, 0, (uint8_t*)strOk);
			dela(0x7fffff);
			adc_calib();
			m_menuState = ParamChoice;
		break;
	}

	tim5_start(0);
}

void ExpressionMenu::encoderClockwise()
{
	switch(m_menuState)
	{
		case ParamChoice:
			if(m_parNum<3)
			{
				DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 0, (uint8_t*)&expr_menu[m_parNum++]);
				DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
				tim5_start(0);
			}
		break;

		case ParamTuning:
			switch(m_parNum)
			{
				case 0:
					if((sys_para[System::EXPR_TYPE]&0x7f)<4)
					{
						DisplayTask->StringOut(60, 0, Font::fntSystem, 0,
								(uint8_t*)strExprType[++sys_para[System::EXPR_TYPE] & 0x7f]);
						adc_pin_init();
						if((sys_para[System::EXPR_TYPE] & 0x7f) > 2)
							ext_send(127);
					}
				break;
				case 2:
					if(sys_para[System::EXPR_CCN] < 128)
					{
						sys_para[System::EXPR_CCN] = BaseParam::encSpeedInc(sys_para[System::EXPR_CCN], 100);
						if(!sys_para[System::EXPR_CCN])
							DisplayTask->StringOut(84, 2, Font::fntSystem, 0, (uint8_t*)"Off");
						else
							DisplayTask->ParamIndicNum(84, 2, sys_para[System::EXPR_CCN]-1);
					}
				break;
				case 3:
					if(!sys_para[System::EXPR_STORE_LEVEL])
					{
						sys_para[System::EXPR_STORE_LEVEL]++;
						DisplayTask->StringOut(84, 3, Font::fntSystem, 0, (uint8_t*)"On ");
					}
				break;
			}
		break;

		case CalMin:
		break;
		case CalMax:
		break;
	}
}

void ExpressionMenu::encoderCounterClockwise()
{
	switch(m_menuState)
	{
		case ParamChoice:
			if(m_parNum>0)
			{
				DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 0, (uint8_t*)&expr_menu[m_parNum--]);
				DisplayTask->StringOut(3, m_parNum, Font::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
				tim5_start(0);
			}
		break;

		case ParamTuning:
			switch(m_parNum)
			{
				case 0:
					if((sys_para[System::EXPR_TYPE]&0x7f)>1)
					{
						DisplayTask->StringOut(60, 0, Font::fntSystem, 0,
								(uint8_t*)&strExprType[--sys_para[System::EXPR_TYPE] & 0x7f]);
						adc_pin_init();
						if((sys_para[System::EXPR_TYPE] & 0x7f) > 2)
							ext_send(127);
					}
				break;
				case 2:
					if(sys_para[System::EXPR_CCN]>0)
					{
						sys_para[System::EXPR_CCN] = BaseParam::encSpeedDec(sys_para[System::EXPR_CCN], 0);
						if(!sys_para[System::EXPR_CCN])
							DisplayTask->StringOut(84, 2, Font::fntSystem, 0, (uint8_t*)"Off");
						else
							DisplayTask->ParamIndicNum(84, 2, sys_para[System::EXPR_CCN]-1);
					}
				break;
				case 3:
					if(sys_para[System::EXPR_STORE_LEVEL])
					{
						sys_para[System::EXPR_STORE_LEVEL]--;
						DisplayTask->StringOut(84, 3, Font::fntSystem, 0, (uint8_t*)"Off");
					}
				break;
			}
		break;

		case CalMin:
		break;

		case CalMax:
		break;
	}
}
