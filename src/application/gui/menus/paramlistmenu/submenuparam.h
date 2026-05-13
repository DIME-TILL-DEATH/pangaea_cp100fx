#ifndef SUBMENUPARAM_H_
#define SUBMENUPARAM_H_

#include "abstractmenu.h"
#include "baseparam.h"

class SubmenuParam : public BaseParam
{
public:
	SubmenuParam(gui_param_type paramType, const char* name,
			AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), AbstractMenu* parentMenu);
	SubmenuParam(gui_param_type paramType, TParamDescriptor* paramDescriptor,
			AbstractMenu* (*submenuCreationFunction)(AbstractMenu* parent), AbstractMenu* parentMenu);
	//	~SubmenuParam();


	const char* name() override;
	void printParam(uint8_t yDisplayPosition) override;
	uint32_t value() const override {return 0;};

	void showSubmenu();
	void showSubmenu(AbstractMenu* parent);

	void select(bool& encoderSelected) override;

private:
	AbstractMenu* m_menu;
	AbstractMenu* m_parentMenu;

	const char* m_menuName;

	AbstractMenu* (*m_submenuCreationFunction)(AbstractMenu* parent) = nullptr;
};


#endif /* SUBMENUPARAM_H_ */
