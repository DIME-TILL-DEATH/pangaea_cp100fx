#ifndef GUIMODULE_H_
#define GUIMODULE_H_

#include "menus/abstractmenu.h"

#include "modules.h"

namespace GuiModules
{

typedef struct
{
	const char* name;
	uint8_t* enablePtr;
	AbstractMenu* (*createMenu)(AbstractMenu* parent);
	void (*enableFunction)();
	dsp_enable_pos_t dspEnablePosition;
}TModule;

AbstractMenu* createRfMenu(AbstractMenu* parentMenu);
AbstractMenu* createGateMenu(AbstractMenu* parentMenu);
AbstractMenu* createCompressorMenu(AbstractMenu* parentMenu);
AbstractMenu* createPreampMenu(AbstractMenu* parentMenu);
AbstractMenu* createAmpMenu(AbstractMenu* parentMenu);
AbstractMenu* createIrMenu(AbstractMenu* parentMenu);
AbstractMenu* createCab1Menu(AbstractMenu* parentMenu);
AbstractMenu* createCab2Menu(AbstractMenu* parentMenu);
AbstractMenu* createCabBrowserMenu(AbstractMenu* parentMenu);
AbstractMenu* createEqMenu(AbstractMenu* parentMenu);
AbstractMenu* createCabMenu(AbstractMenu* parentMenu);
AbstractMenu* createPhaserMenu(AbstractMenu* parentMenu);
AbstractMenu* createFlangerMenu(AbstractMenu* parentMenu);
AbstractMenu* createChorusMenu(AbstractMenu* parentMenu);
AbstractMenu* createEarlyMenu(AbstractMenu* parentMenu);
AbstractMenu* createDelayMenu(AbstractMenu* parentMenu);
AbstractMenu* createDelayTapMenu(AbstractMenu* parentMenu);
AbstractMenu* createReverbMenu(AbstractMenu* parentMenu);
AbstractMenu* createTremoloMenu(AbstractMenu* parentMenu);
}
#endif /* GUIMODULE_H_ */
