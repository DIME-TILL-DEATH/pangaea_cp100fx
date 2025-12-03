#ifndef MODULESMENU_H_
#define MODULESMENU_H_

#include "abstractmenu.h"
#include "nameeditmenu.h"
#include "presetactionsmenu.h"
#include "dialog.h"

#include "guimodules.h"

class TModule
{
public:
	TModule()
	{
		name = "";
	};

	TModule(const char* moduleName, uint8_t* moduleEnablePtr, AbstractMenu* (*moduleCreateMenu)(AbstractMenu* parent) = nullptr,
			void (*moduleEnableFunction)(AbstractMenu* parent) = nullptr, dsp_enable_pos_t moduleDspEnablePosition = ENABLE_AMP)
		: name{moduleName},
		  enablePtr{moduleEnablePtr},
		  createMenu{moduleCreateMenu},
		  enableFunction{moduleEnableFunction},
		  dspEnablePosition{moduleDspEnablePosition}
	{
	};

	const char* name;
	uint8_t* enablePtr{nullptr};

	AbstractMenu* (*createMenu)(AbstractMenu* parent) {nullptr};
	void (*enableFunction)(AbstractMenu* parent) {nullptr};
	dsp_enable_pos_t dspEnablePosition;


	bool operator==(const TModule& other) const
	{
		if (*enablePtr != *(other.enablePtr)) return false;
		if(dspEnablePosition != other.dspEnablePosition) return false;

		if (name == nullptr && other.name == nullptr) return true;
		if (name == nullptr || other.name == nullptr) return false;
		return kgp_sdk_libc::strcmp(name, other.name) == 0;
	}


	bool operator!=(const TModule& other) const
	{
		return !(*this == other);
	}
};

class ModulesMenu : public AbstractMenu
{
public:
	ModulesMenu(AbstractMenu* parent);

	void show(TShowMode swhoMode = FirstShow) override;
	void refresh() override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override;
	void keyDown() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

	uint8_t numMenu() {return m_numMenu;};

	static void enableCab(AbstractMenu* parent);

	static constexpr uint8_t modulesCount = 14;

private:
	static uint8_t m_numMenu;
	bool presetEdited{false};

	void iconRefresh(uint8_t num);

	void arrangeModules();

	TModule modules[ModulesMenu::modulesCount];
	TModule modulesPrevState[ModulesMenu::modulesCount];
	uint8_t prevEnabledState[ModulesMenu::modulesCount];

	TModule RF{"RF", &currentPreset.modules.rawData[ENABLE_RESONANCE_FILTER], &GuiModules::createRfMenu, nullptr, ENABLE_RESONANCE_FILTER};
	TModule GT{"GT", &currentPreset.modules.rawData[ENABLE_GATE], &GuiModules::createGateMenu, nullptr, ENABLE_GATE};
	TModule CM{"CM", &currentPreset.modules.rawData[ENABLE_COMPRESSOR], &GuiModules::createCompressorMenu, nullptr, ENABLE_COMPRESSOR};
	TModule PR{"PR", &currentPreset.modules.rawData[ENABLE_PREAMP], &GuiModules::createPreampMenu, nullptr, ENABLE_PREAMP};
	TModule PA{"PA", &currentPreset.modules.rawData[ENABLE_AMP], &GuiModules::createAmpMenu, nullptr, ENABLE_AMP};
	TModule IR{"IR", &currentPreset.modules.rawData[ENABLE_CAB], &GuiModules::createIrMenu, &ModulesMenu::enableCab, ENABLE_CAB};
	TModule EQ{"EQ", &currentPreset.modules.rawData[ENABLE_EQ], &GuiModules::createEqMenu, nullptr, ENABLE_EQ};
	TModule PH{"PH", &currentPreset.modules.rawData[ENABLE_PHASER], &GuiModules::createPhaserMenu, nullptr, ENABLE_PHASER};
	TModule FL{"FL", &currentPreset.modules.rawData[ENABLE_FLANGER], &GuiModules::createFlangerMenu, nullptr, ENABLE_FLANGER};
	TModule CH{"CH", &currentPreset.modules.rawData[ENABLE_CHORUS], &GuiModules::createChorusMenu, nullptr, ENABLE_CHORUS};
	TModule DL{"DL", &currentPreset.modules.rawData[ENABLE_DELAY], &GuiModules::createDelayMenu, nullptr, ENABLE_DELAY};
	TModule ER{"ER", &currentPreset.modules.rawData[ENABLE_EARLY_REFLECTIONS], &GuiModules::createEarlyMenu, nullptr, ENABLE_EARLY_REFLECTIONS};
	TModule RV{"RV", &currentPreset.modules.rawData[ENABLE_REVERB], &GuiModules::createReverbMenu, nullptr, ENABLE_REVERB};
	TModule TR{"TR", &currentPreset.modules.rawData[ENABLE_TREMOLO], &GuiModules::createTremoloMenu, nullptr, ENABLE_TREMOLO};
};

#endif /* MODULESMENU_H_ */
