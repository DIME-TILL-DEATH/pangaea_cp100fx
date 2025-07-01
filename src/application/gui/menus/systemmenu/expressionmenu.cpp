#include "expressionmenu.h"

#include "cs.h"
#include "gui/elements/allFonts.h"
#include "gui/elements/icon_bit.h"
#include "display.h"
#include "enc.h"
#include "eepr.h"

#include "BF706_send.h"

#include "../../gui_task.h"


ExpressionMenu::ExpressionMenu(AbstractMenu *parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_EXPRESSION;
}

void ExpressionMenu::show(TShowMode swhoMode)
{
	currentMenu = this;

	DisplayTask->Clear();
	for(uint8_t i = 0; i<4; i++)
	{
		DisplayTask->StringOut(3, i, TDisplayTask::fntSystem, 0, (uint8_t*)&expr_menu[i]);
	}

	DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0, (uint8_t*)&expr_type[sys_para[EXPRESSION_TYPE] & 0x7f]);

	if(!sys_para[EXP_CCN])
		DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
	else
		DisplayTask->ParamIndicNum(84, 2, sys_para[EXP_CCN]-1);

	if(!sys_para[STORE_EXP_LEVEL])
		DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
	else
		DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"On ");
}

void ExpressionMenu::task()
{
	if(!m_encoderKnobSelected)
		DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 2*tim5_fl, (uint8_t*)&expr_menu[m_parNum]);
}

void ExpressionMenu::encoderPressed()
{
	if(!m_encoderKnobSelected)
	{
		m_encoderKnobSelected = 1;
		switch(m_parNum)
		{
			case 0:
				DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
			break;
			case 1:
//				DisplayTask->StringOut(67, 1, TDisplayTask::fntSystem, 0, (uint8_t*)set_min);
//				current_menu_type = MENU_ADC_CALIBRATE;
				tim5_start(0);
			break;
			case 2:
				DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
			break;
		}
	}
	else
	{
		m_encoderKnobSelected = 0;
		DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 0, (uint8_t*)&expr_menu[m_parNum]);
	}
	clean_flag();
}

void ExpressionMenu::encoderClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum<3)
		{
			DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 0, (uint8_t*)&expr_menu[m_parNum++]);
			DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
			tim5_start(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case 0:
				if((sys_para[EXPRESSION_TYPE]&0x7f)<4)
				{
					DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0,
							(uint8_t*)expr_type[++sys_para[EXPRESSION_TYPE] & 0x7f]);
					adc_pin_init();
					if(sys_para[EXPRESSION_TYPE] > 2)
						ext_send(127);
				}
			break;
			case 2:
				if(sys_para[EXP_CCN] < 128)
				{
					sys_para[EXP_CCN] = enc_speed_inc(sys_para[EXP_CCN], 100);
					if(!sys_para[EXP_CCN])
						DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
					else
						DisplayTask->ParamIndicNum(84, 2, sys_para[EXP_CCN]-1);
				}
			break;
			case 3:
				if(!sys_para[STORE_EXP_LEVEL])
				{
					sys_para[STORE_EXP_LEVEL]++;
					DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"On ");
				}
			break;
		}
	}
	clean_flag();
}

void ExpressionMenu::encoderCounterClockwise()
{
	if(!m_encoderKnobSelected)
	{
		if(m_parNum>0)
		{
			DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 0, (uint8_t*)&expr_menu[m_parNum--]);
			DisplayTask->StringOut(3, m_parNum, TDisplayTask::fntSystem, 2, (uint8_t*)&expr_menu[m_parNum]);
			tim5_start(0);
		}
	}
	else
	{
		switch(m_parNum)
		{
			case 0:
				if((sys_para[EXPRESSION_TYPE]&0x7f)>1)
				{
					DisplayTask->StringOut(60, 0, TDisplayTask::fntSystem, 0,
							(uint8_t*)&expr_type[--sys_para[EXPRESSION_TYPE] & 0x7f]);
					adc_pin_init();
					if(sys_para[EXPRESSION_TYPE]>2)
						ext_send(127);
				}
			break;
			case 2:
				if(sys_para[EXP_CCN]>0)
				{
					sys_para[EXP_CCN] = enc_speed_dec(sys_para[EXP_CCN], 0);
					if(!sys_para[EXP_CCN])
						DisplayTask->StringOut(84, 2, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
					else
						DisplayTask->ParamIndicNum(84, 2, sys_para[EXP_CCN]-1);
				}
			break;
			case 3:
				if(sys_para[STORE_EXP_LEVEL])
				{
					sys_para[STORE_EXP_LEVEL]--;
					DisplayTask->StringOut(84, 3, TDisplayTask::fntSystem, 0, (uint8_t*)"Off");
				}
			break;
		}
	}

	clean_flag();
}
