#ifndef MODULESMENU_H_
#define MODULESMENU_H_

#include "abstractmenu.h"
#include "nameeditmenu.h"
#include "presetactionsmenu.h"
#include "dialog.h"
#include "modules.h"
#include "guimodules.h"

class TModule
{
public:
	TModule()
	{
		name = "";
	};

	TModule(const char* moduleName, uint8_t* moduleEnablePtr, void (*moduleSetterFunction)(uint32_t value),
			AbstractMenu* (*moduleCreateMenu)(AbstractMenu* parent) = nullptr,
			void (*moduleEnableFunction)(AbstractMenu* parent) = nullptr)
		: name{moduleName},
		  enablePtr{moduleEnablePtr},
		  setterFunction{moduleSetterFunction},
		  createMenu{moduleCreateMenu},
		  enableFunction{moduleEnableFunction}
	{
	};

	const char* name;

	uint8_t* enablePtr{nullptr};
	void (*setterFunction)(uint32_t value) {nullptr};
	AbstractMenu* (*createMenu)(AbstractMenu* parent) {nullptr};
	void (*enableFunction)(AbstractMenu* parent) {nullptr};

	bool operator==(const TModule& other) const
	{
		if(*enablePtr != *(other.enablePtr)) return false;

		if(name == nullptr && other.name == nullptr) return true;
		if(name == nullptr || other.name == nullptr) return false;
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

	TModule RF{"RF", &currentPreset.paramData.switches.resonance_filter, rf_on_setter, &GuiModules::createRfMenu};
	TModule GT{"GT", &currentPreset.paramData.switches.gate, gate_on_setter, &GuiModules::createGateMenu};
	TModule CM{"CM", &currentPreset.paramData.switches.compressor, compressor_on_setter, &GuiModules::createCompressorMenu};
	TModule PR{"PR", &currentPreset.paramData.switches.preamp, preamp_on_setter, &GuiModules::createPreampMenu};
	TModule PA{"PA", &currentPreset.paramData.switches.amp, amp_on_setter, &GuiModules::createAmpMenu};
	TModule IR{"IR", &currentPreset.paramData.switches.cab, ir_on_setter, &GuiModules::createIrMenu, &ModulesMenu::enableCab};
	TModule EQ{"EQ", &currentPreset.paramData.switches.eq, eq_on_setter, &GuiModules::createEqMenu};
	TModule PH{"PH", &currentPreset.paramData.switches.phaser, phaser_on_setter, &GuiModules::createPhaserMenu};
	TModule FL{"FL", &currentPreset.paramData.switches.flanger, flanger_on_setter, &GuiModules::createFlangerMenu};
	TModule CH{"CH", &currentPreset.paramData.switches.chorus, chorus_on_setter, &GuiModules::createChorusMenu};
	TModule DL{"DL", &currentPreset.paramData.switches.delay, delay_on_setter, &GuiModules::createDelayMenu};
	TModule ER{"ER", &currentPreset.paramData.switches.early_reflections, early_on_setter, &GuiModules::createEarlyMenu};
	TModule RV{"RV", &currentPreset.paramData.switches.reverb, reverb_on_setter, &GuiModules::createReverbMenu};
	TModule TR{"TR", &currentPreset.paramData.switches.tremolo, tremolo_on_setter, &GuiModules::createTremoloMenu};
};

#endif /* MODULESMENU_H_ */
