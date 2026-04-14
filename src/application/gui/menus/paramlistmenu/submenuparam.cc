#include "submenuparam.h"

#include "eeprom.h"
#include "system.h"

#include "display_task.h"

SubmenuParam::SubmenuParam(gui_param_type paramType, const char* name,
		AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), AbstractMenu* parentMenu)
				: BaseParam(paramType, name, nullptr)
{
	m_submenuCreationFunction = submenuCreationFunction;
	m_parentMenu = parentMenu;
}

SubmenuParam::SubmenuParam(gui_param_type paramType, TParamDescriptor paramDescriptor,
		AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), AbstractMenu* parentMenu)
				: BaseParam(paramType,paramDescriptor)
{
	m_submenuCreationFunction = submenuCreationFunction;
	m_parentMenu = parentMenu;
}

void SubmenuParam::printParam(uint8_t yDisplayPosition)
{
	if(m_disabled) return;

	switch(m_type)
	{
		case BaseParam::GUI_PARAMETER_PAN:
			DisplayTask->ParamIndPan(m_xDisplayPosition, yDisplayPosition, *m_valuePtr + m_offset);
			break;
		case BaseParam::GUI_PARAMETER_SUBMENU:
			break;
		case BaseParam::GUI_PARAMETER_SUBMENU_DELAY_TIME:
		{
			// переделать в CustomParam?
			if(!sys_para[System::TIME_FORMAT])
			{
				DisplayTask->DelayTimeInd(m_xDisplayPosition, yDisplayPosition, currentPreset.delayTime);
				DisplayTask->StringOut(m_xDisplayPosition + 35, yDisplayPosition, Font::fntSystem , Font::fnsNormal , (uint8_t*)" >");
			}
			else
			{
				DisplayTask->ParamIndNum(m_xDisplayPosition, yDisplayPosition, 60000/currentPreset.delayTime);
				DisplayTask->StringOut(m_xDisplayPosition + Font::symbolWidth(Font::fntSystem) * 4,
										yDisplayPosition, Font::fntSystem, Font::fnsNormal, (uint8_t*)"BPM>");
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

void SubmenuParam::select(bool& encoderSelected)
{
	if(m_submenuCreationFunction)
	{
		AbstractMenu* newSubMenu = m_submenuCreationFunction(m_parentMenu);
		currentMenu = newSubMenu;
		newSubMenu->show();
	}
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
