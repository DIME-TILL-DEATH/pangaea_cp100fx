#include "modulesmenu.h"

#include "eepr.h"
#include "fs_browser.h"
#include "preset.h"

#include "controllers_task.h"
#include "display_task.h"
#include "filesystem_task.h"
#include "io_task.h"
#include "sdtest_task.h"
#include "ui_task.h"
#include "sharc_task.h"

#include "bitmaps.h"

#include "stringlistparam.h"
#include "stringoutparam.h"

#include "paramlistmenu.h"
#include "controllersmenu.h"
#include "cabbrowsermenu.h"

uint8_t ModulesMenu::m_numMenu = 0;

ModulesMenu::ModulesMenu(AbstractMenu* parent)
{
	topLevelMenu = parent;
	m_menuType = MENU_MODULES;

	modules[0] = RF;
	modules[1] = GT;
	modules[2] = CM;
	modules[3] = PR;
	modules[4] = PA;
	modules[5] = IR;
	modules[6] = EQ;
	modules[7] = PH;
	modules[8] = FL;
	modules[9] = CH;
	modules[10] = DL;
	modules[11] = ER;
	modules[12] = RV;
	modules[13] = TR;
}

void ModulesMenu::show(TShowMode showMode)
{
	for(uint8_t i=0; i<modulesCount; i++)
	{
		modulesPrevState[i] = TModule("" , &prevEnabledState[i]);
	}

	currentMenu = this;

	if(showMode == TShowMode::FirstShow) presetEdited = false;

	DisplayTask->SetVolIndicator(TVolIndicatorType::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	DisplayTask->Clear();
	refresh();

	restartBlinking(0);
}

void ModulesMenu::refresh()
{
	arrangeModules();

	for(uint8_t i=0; i<modulesCount; i++)
	{
		if(modulesPrevState[i] != modules[i])
		{
			iconRefresh(i);
			modulesPrevState[i] = modules[i];
			prevEnabledState[i] = *(modules[i].enablePtr);
			modulesPrevState[i].enablePtr = &prevEnabledState[i];
		}
	}
}

void ModulesMenu::task()
{
	uint8_t xCoord, yCoord;

	if(m_numMenu < 7)
	{
		xCoord = 2 + 18*m_numMenu;
		yCoord = 0;
	}
	else
	{
		xCoord = 2 + 18*(m_numMenu - 7);
		yCoord = 2;
	}

	if(blinkFlag == 0) DisplayTask->EffectIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, *modules[m_numMenu].enablePtr);
	else DisplayTask->EffectIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, 2);
}

void ModulesMenu::encoderPressed()
{
	presetEdited = true;

	*modules[m_numMenu].enablePtr = !((bool)*modules[m_numMenu].enablePtr);
	SharcTask->setParameter(DSP_ADDRESS_MODULES_ENABLE, modules[m_numMenu].dspEnablePosition, *modules[m_numMenu].enablePtr);

	*(modulesPrevState[m_numMenu].enablePtr) = *(modules[m_numMenu].enablePtr);

	if(modules[m_numMenu].enableFunction) modules[m_numMenu].enableFunction(this);

	restartBlinking(1);
}

void ModulesMenu::encoderClockwise()
{
	iconRefresh(m_numMenu);

	if(m_numMenu < 13) m_numMenu++;
	else m_numMenu = 0;

	restartBlinking(0);
}

void ModulesMenu::encoderCounterClockwise()
{
	iconRefresh(m_numMenu);

	if(m_numMenu > 0) m_numMenu--;
	else m_numMenu = 13;

	restartBlinking(0);
}

void ModulesMenu::keyUp()
{
	if(presetEdited == 1)
	{
		shownChildMenu = new Dialog(this, Dialog::SaveChanges);
		shownChildMenu->show();
	}
	else
	{
		topLevelMenu->returnFromChildMenu();
	}
	restartBlinking(0);
}

void ModulesMenu::keyDown()
{
	if(*modules[m_numMenu].enablePtr)
	{
		presetEdited = true;
		shownChildMenu = modules[m_numMenu].createMenu(this);
		shownChildMenu->show();
	}

	restartBlinking(0);
}

void ModulesMenu::key1()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new Dialog(this, Dialog::ErasePreset);
	shownChildMenu->show();

	restartBlinking(0);
}

void ModulesMenu::key2()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	const uint8_t paramCount = 3;
	BaseParam* params[paramCount];

	params[0] = new StringOutParam("Preset level");
	params[0]->setDisplayPosition(ParamListMenu::leftPad + 4*6);
	params[1] = new StringListParam("Control", &currentPreset.modulesBuf[PRESET_VOLUME_CONTROL], {"On ", "Off"}, 3);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &currentPreset.modulesBuf[PRESET_VOLUME]);
	params[2]->setDspAddress(DSP_ADDRESS_PRESET_VOLUME, PARAM_EQUAL_POS);

	ParamListMenu* menu = new ParamListMenu(this, MENU_PRESET_VOLUME);
	if(menu)
	{
		menu->setParams(params, paramCount);
		menu->setIcon(false, ICON_NONE);
		menu->setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_OUT, DSP_INDICATOR_OUT);
	}

	presetEdited = true;
	shownChildMenu = menu;
	shownChildMenu->show();

	restartBlinking(0);
}

void ModulesMenu::key3()
{
	if(shownChildMenu) delete shownChildMenu;

	presetEdited = true;
	shownChildMenu = new ControllersMenu(this);
	shownChildMenu->show();
}

void ModulesMenu::key4()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	presetEdited = true;
	shownChildMenu = new NameEditMenu(this);
	shownChildMenu->show();
}

void ModulesMenu::key5()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new PresetActionsMenu(this, PresetActionsMenu::Copy);
	shownChildMenu->show();
}

void ModulesMenu::iconRefresh(uint8_t num)
{
	DisplayTask->EffectIcon(2 + (num%7) * 18, (num/7) * 2,(uint8_t*)modules[num].name, *modules[num].enablePtr);
}

void ModulesMenu::enableCab(AbstractMenu* parent)
{
	if(currentPreset.cab1NameSize == 0)
	{
		if(TSDTestTask::sdInitState == 1)
		{
		  if(TFsBrowser::impulseDirExist)
		  {
			  parent->showChild(new CabBrowserMenu(parent, 0));
		  }
		  else
		  {
			  DisplayTask->Clear();
			  DisplayTask->StringOut(0, 1, Font::fntSystem, 0, "There is no directory");
			  DisplayTask->StringOut(42, 3, Font::fntSystem, 0, "IMPULSE");
			  UITask->Delay(1000);
			  currentPreset.modulesBuf[cab] = 0;

			  parent->refresh();
		  }
		}
		else
		{
			DisplayTask->Clear();
			if(!TSDTestTask::sdInitState) DisplayTask->StringOut(6, 1, Font::fntSystem, 0, "MicroSD is not ready");
			else DisplayTask->StringOut(0, 1, Font::fntSystem, 0, "MicroSD is loading..");
			UITask->Delay(1000);

			currentPreset.modulesBuf[cab] = 0;

			parent->refresh();
		}
	}
}

void ModulesMenu::arrangeModules()
{
	uint8_t i = 0;

	modules[i++] = RF;
	modules[i++] = GT;
	modules[i++] = CM;
	modules[i++] = PR;
	modules[i++] = PA;

	if(currentPreset.paramData.eq_pre_post) modules[i++] = EQ;
	if(currentPreset.paramData.phaser_pre_post) modules[i++] = PH;
	if(currentPreset.paramData.flanger_pre_post) modules[i++] = FL;

	modules[i++] = IR;

	if(!currentPreset.paramData.eq_pre_post) modules[i++] = EQ;
	if(!currentPreset.paramData.phaser_pre_post) modules[i++] = PH;
	if(!currentPreset.paramData.flanger_pre_post) modules[i++] = FL;

	modules[i++] = CH;
	modules[i++] = DL;
	modules[i++] = ER;
	modules[i++] = RV;
	modules[i++] = TR;
}
