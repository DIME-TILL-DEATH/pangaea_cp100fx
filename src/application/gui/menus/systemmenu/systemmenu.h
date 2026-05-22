#ifndef SYSTEMMENU_H_
#define SYSTEMMENU_H_

#include "paramlistmenu.h"

class SystemMenu: public ParamListMenu
{
public:
	SystemMenu(AbstractMenu* parent, gui_menu_type menuType);

	void keyUp() override;
	void keyDown() override;

	void key1() override;
	void key2() override;
	void key3() override;
	void key4() override;
	void key5() override;

	static AbstractMenu* create(AbstractMenu* parent);

private:
	uint8_t m_previousCabConfig;

	bool editingFinished();

	static AbstractMenu* createMidiPcMapMenu(AbstractMenu* parent);

	static void expressionPrint(void* parameter);
	static void expressionDescrease(void* parameter);
	static void expressionIncrease(void* parameter);
	static void expressionKeyDown(void* parameter);

	static void tunerExtPrint(void* parameter);
	static void tunerExtDescrease(void* parameter);
	static void tunerExtIncrease(void* parameter);
	static void tunerExtKeyDown(void* parameter);
};

#endif /* SYSTEMMENU_H_ */
