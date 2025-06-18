#include "submenuparam.h"

#include "BF706_send.h"
#include "enc.h"
#include "eepr.h"

#include "display.h"

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name, AbstractMenu* menu, void* param)
				:BaseParam(paramType, name, param)
{
	m_menu = menu;
}

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name,
		AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), void* param)
				:BaseParam(paramType, name, param)
{
	m_submenuCreationFunction = submenuCreationFunction;
}

void SubmenuParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled) return;

	switch(m_type)
	{
		case BaseParam::GUI_PARAMETER_PAN:
			DisplayTask->ParamIndicPan(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_SUBMENU:
			break;
		case BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME:
		{
			DisplayTask->DelayTimeInd(m_xDisplayPosition, yDisplayPosition, delay_time);

			if(!sys_para[tim_type])
			{
				DisplayTask->DelayTimeInd(m_xDisplayPosition, yDisplayPosition, delay_time);
				DisplayTask->StringOut(m_xDisplayPosition + 35, yDisplayPosition,TDisplayTask::fntSystem , 0 , (uint8_t*)" >");
			}
			else
			{
				DisplayTask->ParamIndicNum(m_xDisplayPosition, yDisplayPosition, 60000/delay_time);
				DisplayTask->StringOut(m_xDisplayPosition + 34, yDisplayPosition, TDisplayTask::fntSystem, 0, (uint8_t*)"BPM");
				DisplayTask->StringOut(m_xDisplayPosition + 70, yDisplayPosition,TDisplayTask::fntSystem , 0 , (uint8_t*)" >");
			}
			break;
		}
		default: break;
	}
}

void SubmenuParam::showSubmenu()
{
	if(m_menu) m_menu->show();
}

void SubmenuParam::showSubmenu(AbstractMenu* parent)
{
	if(m_submenuCreationFunction)
	{
		AbstractMenu* newSubMenu = m_submenuCreationFunction(parent);
		currentMenu = newSubMenu;
		newSubMenu->show();
	}
}
