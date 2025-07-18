#include "modulesmenu.h"

#include "guimodules.h"

#include "appdefs.h"
#include "cs.h"
#include "fs.h"
#include "eepr.h"
#include "allFonts.h"
#include "display.h"
#include "enc.h"
#include "cc.h"
#include "BF706_send.h"

#include "sd_test.h"
#include "fs_browser.h"

#include "modules.h"
#include "preset.h"

#include "paramlistmenu.h"
#include "controllersmenu.h"
#include "cabbrowsermenu.h"

#include "stringlistparam.h"
#include "stringoutparam.h"

GuiModules::TModule modules[ModulesMenu::modulesCount];

GuiModules::TModule RF = {"RF", &currentPreset.modules.rawData[ENABLE_RESONANCE_FILTER], &GuiModules::createRfMenu, nullptr, ENABLE_RESONANCE_FILTER};
GuiModules::TModule GT = {"GT", &currentPreset.modules.rawData[ENABLE_GATE], &GuiModules::createGateMenu, nullptr, ENABLE_GATE};
GuiModules::TModule CM = {"CM", &currentPreset.modules.rawData[ENABLE_COMPRESSOR], &GuiModules::createCompressorMenu, nullptr, ENABLE_COMPRESSOR};
GuiModules::TModule PR = {"PR", &currentPreset.modules.rawData[ENABLE_PREAMP], &GuiModules::createPreampMenu, nullptr, ENABLE_PREAMP};
GuiModules::TModule PA = {"PA", &currentPreset.modules.rawData[ENABLE_AMP], &GuiModules::createAmpMenu, nullptr, ENABLE_AMP};
GuiModules::TModule IR = {"IR", &currentPreset.modules.rawData[ENABLE_CAB], &GuiModules::createIrMenu, &ModulesMenu::enableCab, ENABLE_CAB};
GuiModules::TModule EQ = {"EQ", &currentPreset.modules.rawData[ENABLE_EQ], &GuiModules::createEqMenu, nullptr, ENABLE_EQ};
GuiModules::TModule PH = {"PH", &currentPreset.modules.rawData[ENABLE_PHASER], &GuiModules::createPhaserMenu, nullptr, ENABLE_PHASER};
GuiModules::TModule FL = {"FL", &currentPreset.modules.rawData[ENABLE_FLANGER], &GuiModules::createFlangerMenu, nullptr, ENABLE_FLANGER};
GuiModules::TModule CH = {"CH", &currentPreset.modules.rawData[ENABLE_CHORUS], &GuiModules::createChorusMenu, nullptr, ENABLE_CHORUS};
GuiModules::TModule DL = {"DL", &currentPreset.modules.rawData[ENABLE_DELAY], &GuiModules::createDelayMenu, nullptr, ENABLE_DELAY};
GuiModules::TModule ER = {"ER", &currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS], &GuiModules::createEarlyMenu, nullptr, ENABLE_EARLY_REFLECTIONS};
GuiModules::TModule RV = {"RV", &currentPreset.modules.rawData[ENABLE_REVERB], &GuiModules::createReverbMenu, nullptr, ENABLE_REVERB};
GuiModules::TModule TR = {"TR", &currentPreset.modules.rawData[ENABLE_TREMOLO], &GuiModules::createTremoloMenu, nullptr, ENABLE_TREMOLO};


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
	currentMenu = this;

	if(showMode == TShowMode::FirstShow) presetEdited = false;

	DisplayTask->SetVolIndicator(TDisplayTask::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
	refresh();

	tim5_start(0);
}

void ModulesMenu::refresh()
{
	DisplayTask->Clear();
	for(uint8_t i=0; i<modulesCount; i++)
	{
		iconRefresh(i);
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

	if(blinkFlag_fl == 0) DisplayTask->EfIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, *modules[m_numMenu].enablePtr);
	else DisplayTask->EfIcon(xCoord, yCoord, (uint8_t*)modules[m_numMenu].name, 2);
}

void ModulesMenu::encoderPressed()
{
	presetEdited = true;

	*modules[m_numMenu].enablePtr = !((bool)*modules[m_numMenu].enablePtr);
	DSP_GuiSendParameter(DSP_ADDRESS_MODULES_ENABLE, modules[m_numMenu].dspEnablePosition, *modules[m_numMenu].enablePtr);

	if(modules[m_numMenu].enableFunction) modules[m_numMenu].enableFunction(this);

	tim5_start(1);
}

void ModulesMenu::encoderClockwise()
{
	iconRefresh(m_numMenu);

	if(m_numMenu < 13) m_numMenu++;
	else m_numMenu = 0;

	tim5_start(0);
}

void ModulesMenu::encoderCounterClockwise()
{
	iconRefresh(m_numMenu);

	if(m_numMenu > 0) m_numMenu--;
	else m_numMenu = 13;

	tim5_start(0);
}

void ModulesMenu::keyUp()
{
	if(presetEdited == 1)
	{
		confirmSaveMenu.setTopLevelMenu(topLevelMenu);
		saveDialog.setYesMenu(&confirmSaveMenu);
		saveDialog.setNoMenu(topLevelMenu);
		saveDialog.show();
	}
	else
	{
		topLevelMenu->returnFromChildMenu(TReturnMode::KeepChild);
	}
	tim5_start(0);
}

void ModulesMenu::keyDown()
{
	if(*modules[m_numMenu].enablePtr)
	{
		presetEdited = true;
		shownChildMenu = modules[m_numMenu].createMenu(this);
		shownChildMenu->show();
	}

	tim5_start(0);
}

void ModulesMenu::key1()
{
	currentMenu = this;

	if(shownChildMenu) delete shownChildMenu;

	shownChildMenu = new Dialog(this, Dialog::ErasePreset);
	shownChildMenu->show();

	tim5_start(0);
}

void ModulesMenu::key2()
{
	currentMenu = this;

	// было своё copyMenu
	if(shownChildMenu) delete shownChildMenu;

	const uint8_t paramCount = 3;
	BaseParam* params[paramCount];

	params[0] = new StringOutParam("Preset level");
	params[0]->setDisplayPosition(40);
	params[1] = new StringListParam("Control", &currentPreset.modules.rawData[PRESET_VOLUME_CONTROL], {"On ", "Off"}, 3);
	params[2] = new BaseParam(BaseParam::GUI_PARAMETER_LEVEL, "Level", &currentPreset.modules.rawData[PRESET_VOLUME]);
	params[2]->setDspAddress(DSP_ADDRESS_PRESET_VOLUME, PARAM_EQUAL_POS);

	ParamListMenu* menu = new ParamListMenu(this, MENU_PRESET_VOLUME);
	if(menu)
	{
		menu->setParams(params, paramCount);
		menu->setIcon(false, ICON_NONE);
		menu->setVolumeIndicator(TDisplayTask::VOL_INDICATOR_OUT, DSP_INDICATOR_OUT);
	}

	shownChildMenu = menu;
	shownChildMenu->show();

	tim5_start(0);
}

void ModulesMenu::key3()
{
	// было своё copyMenu
	if(shownChildMenu) delete shownChildMenu;

	presetEdited = true;
	shownChildMenu = new ControllersMenu(this);
	shownChildMenu->show();
}

void ModulesMenu::key4()
{
	currentMenu = this;

	// было своё copyMenu
	if(shownChildMenu) delete shownChildMenu;

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
	DisplayTask->EfIcon(2 + (num%7) * 18, (num/7) * 2,(uint8_t*)modules[num].name, *modules[num].enablePtr);
}

void ModulesMenu::enableCab(ModulesMenu* parent)
{
	if(cab1.name.size == 0)
	{
		if(TSD_TESTTask::sdInitState == 1)
		{
		  if(TFsBrowser::impulseDirExist)
		  {
			  parent->showChild(new CabBrowserMenu(parent, 1));
		  }
		  else
		  {
			  DisplayTask->Clear();
			  DisplayTask->StringOut(0, 1, Font::fntSystem, 0, "There is no directory");
			  DisplayTask->StringOut(42, 3, Font::fntSystem, 0, "IMPULSE");
			  CSTask->CS_del(1000);
			  currentPreset.modules.rawData[cab] = 0;

			  parent->refresh();
		  }
		}
		else
		{
			DisplayTask->Clear();
			if(!TSD_TESTTask::sdInitState) DisplayTask->StringOut(6, 1, Font::fntSystem, 0, "MicroSD is not ready");
			else DisplayTask->StringOut(0, 1, Font::fntSystem, 0, "MicroSD is loading..");
			CSTask->CS_del(1000);

			currentPreset.modules.rawData[cab] = 0;

			parent->refresh();
		}
	}
}
