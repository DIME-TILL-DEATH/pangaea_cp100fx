#ifndef SUBMENUPARAM_H_
#define SUBMENUPARAM_H_

#include "abstractmenu.h"
#include "baseparam.h"

class SubmenuParam : public BaseParam
{
public:
	SubmenuParam(gui_param_type paramType, const char* name, AbstractMenu* menu, void* param = nullptr);
	SubmenuParam(gui_param_type paramType, const char* name,
			AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), void* param = nullptr);
	//	~SubmenuParam();

	void showSubmenu();
	void showSubmenu(AbstractMenu* parent);

	uint8_t value() const override {return 0;};

private:
	AbstractMenu* m_menu;

	AbstractMenu* (*m_submenuCreationFunction)(AbstractMenu* parent) = nullptr;
};


#endif /* SUBMENUPARAM_H_ */
