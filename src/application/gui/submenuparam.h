#ifndef SUBMENUPARAM_H_
#define SUBMENUPARAM_H_

#include "abstractmenu.h"
#include "baseparam.h"

class SubmenuParam : public BaseParam
{
public:
	SubmenuParam(gui_param_type paramType, const char* name, AbstractMenu* menu);
//	~SubmenuParam();

	void showSubmenu();

	uint8_t value() const override {return 0;};

private:
	AbstractMenu* m_menu;
};


#endif /* SUBMENUPARAM_H_ */
