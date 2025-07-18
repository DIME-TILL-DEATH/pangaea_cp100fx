#ifndef SYSTEMMENU_H_
#define SYSTEMMENU_H_

#include "paramlistmenu.h"

class SystemMenu: public ParamListMenu
{
public:
	SystemMenu(AbstractMenu* parent, gui_menu_type menuType);

	void encoderPressed() override;
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

	static AbstractMenu* createFootswitchMenu(AbstractMenu* parent);
	static AbstractMenu* createMidiPcMapMenu(AbstractMenu* parent);

	static void expressionPrint(void* parameter);
	static void expressionDescrease(void* parameter);
	static void expressionIncrease(void* parameter);
	static void expressionKeyDown(void* parameter);

	static void tunerExtPrint(void* parameter);
	static void tunerExtDescrease(void* parameter);
	static void tunerExtIncrease(void* parameter);
	static void tunerExtKeyDown(void* parameter);

	static void tempoDecrease(void* parameter);
	static void tempoIncrease(void* parameter);

	static void tunerSpeedDescrease(void* parameter);
	static void tunerSpeedIncrease(void* parameter);
};

#endif /* SYSTEMMENU_H_ */
