#include "attenuatormenu.h"

#include "system.h"
#include "bitmaps.h"
#include "eeprom.h"

#include "master_setters.h"

#include "stringlistparam.h"

AttenuatorMenu::AttenuatorMenu(AbstractMenu* parentMenu)
	: ParamListMenu(parentMenu, MENU_ATTENUATOR)
{
	const uint8_t paramCount = 2;
	BaseParam* params[paramCount];

	params[0] = new StringListParam(&AttenuatorDesc.volume,
			 {" +4 dB", " +3 dB", " +2 dB", " +1 dB", "  0 dB", " -1 dB", " -2 dB", " -3 dB",
			" -4 dB", " -5 dB", " -6 dB", " -7 dB", " -8 dB", " -9 dB", "-10 dB", "-11 dB", "-12 dB",
			"-13 dB", "-14 dB", "-15 dB", "-16 dB", "-17 dB", "-18 dB", "-19 dB", "-20 dB", "-21 dB",
			"-23 dB", "-24 dB", "-27 dB", "-30 dB", "-34 dB", "-35 dB", "-36 dB", "-37 dB", "-38 dB",
			"-40 dB", "-41 dB", "-42 dB", "-43 dB", "-44 dB", "-45 dB", "-46 dB", "-47 dB", "-48 dB",
			"-49 dB", "-50 dB", "-51 dB", "-52 dB", "-53 dB", "-54 dB", "-55 dB", "-56 dB", "-57 dB",
			"-58 dB", "-59 dB", "-60 dB"}, 6);
	params[0]->setDisplayPosition(70);
	params[0]->setInverse(true);

	params[1] = new StringListParam(&AttenuatorDesc.source, {"Global", "Preset"}, 7);
	params[1]->setDisplayPosition(76);

	setVolumeIndicator(TVolIndicatorType::VOL_INDICATOR_IN, DSP_INDICATOR_IN);

	setParams(params, paramCount);
	setIcon(false, ICON_NONE);

	DisplayTask->potWrite();
}

void AttenuatorMenu::keyUp()
{
	exitMenu();
	topLevelMenu->returnFromChildMenu();
}

void AttenuatorMenu::key1()
{
	exitMenu();
	topLevelMenu->returnFromChildMenu();
}

void AttenuatorMenu::key2()
{
	exitMenu();
	topLevelMenu->key2();
}

void AttenuatorMenu::key3()
{
	exitMenu();
	topLevelMenu->key3();
}

void AttenuatorMenu::key4()
{
	exitMenu();
	topLevelMenu->key4();
}

void AttenuatorMenu::key5()
{
	exitMenu();
	topLevelMenu->key5();
}

void AttenuatorMenu::exitMenu()
{
	if(sys_para[System::ATTENUATOR_MODE])
		EEPROM_SavePreset(currentPresetNumber, &currentPreset);

	EEPROM_SaveSystemData();
	DisplayTask->SetVolIndicator(TVolIndicatorType::VOL_INDICATOR_OFF, DSP_INDICATOR_OUT);
}
