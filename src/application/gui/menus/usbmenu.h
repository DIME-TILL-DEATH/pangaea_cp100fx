#ifndef SRC_APPLICATION_GUI_MENUS_USBMENU_H_
#define SRC_APPLICATION_GUI_MENUS_USBMENU_H_

#include "abstractmenu.h"
#include "usb.h"

class UsbMenu: public AbstractMenu
{
public:
	UsbMenu();

	void show(TShowMode showMode = FirstShow) override;
	void task() override;

	void encoderPressed() override;
	void encoderClockwise() override;
	void encoderCounterClockwise() override;

	void keyUp() override {};
	void keyDown() override {};

	void key1() override {};
	void key2() override {};
	void key3() override {};
	void key4() override {};
	void key5() override {};

	TUsbTask::TMode connectionMode() {return usb_connect_type;};

	bool isConnected() {return usbConnected;};

private:
	uint8_t m_parNum{0};

	uint8_t m_countOff{20};

	TUsbTask::TMode usb_connect_type;
	bool usbConnected{false};

	bool autoconnectOn{true};

	static constexpr uint8_t strUsbMenu[][19] = {"Serial port", "Usb drive", "Autoconnect after:"};
	static constexpr uint8_t strPositions[3] = {30, 36, 6};

	void start_usb();
};

#endif /* SRC_APPLICATION_GUI_MENUS_USBMENU_H_ */
