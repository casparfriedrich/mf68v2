#include "led.hpp"

#include <logging/log.h>
#include <usb/class/usb_hid.h>
#include <usb/usb_device.h>
#include <zephyr.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_DBG);

extern struct k_sem app_hid_init;
extern struct k_sem app_hid_ready;

void usb_status_cb(enum usb_dc_status_code status, const u8_t *param)
{
	switch (status) {
	case USB_DC_ERROR:
		LOG_INF("USB error reported by the controller");
		break;
	case USB_DC_RESET:
		LOG_INF("USB reset");
		break;
	case USB_DC_CONNECTED:
		LOG_INF("USB connection established, hardware enumeration is completed");
		break;
	case USB_DC_CONFIGURED:
		k_sem_give(&app_hid_ready);
		LOG_INF("USB configuration done");
		break;
	case USB_DC_DISCONNECTED:
		LOG_INF("USB connection lost");
		break;
	case USB_DC_SUSPEND:
		LOG_INF("USB connection suspended by the HOST");
		break;
	case USB_DC_RESUME:
		LOG_INF("USB connection resumed by the HOST");
		break;
	case USB_DC_INTERFACE:
		LOG_INF("USB interface selected");
		break;
	case USB_DC_SET_HALT:
		LOG_INF("Set Feature ENDPOINT_HALT received");
		break;
	case USB_DC_CLEAR_HALT:
		LOG_INF("Clear Feature ENDPOINT_HALT received");
		break;
	case USB_DC_SOF:
		LOG_INF("Start of Frame received");
		break;
	case USB_DC_UNKNOWN:
		LOG_INF("Initial USB connection status");
		break;
	}
}

void main()
{
	led::on<led::Led0>();

	k_sem_take(&app_hid_init, K_FOREVER);

	int err __unused = usb_enable(usb_status_cb);
	__ASSERT(!err, "usb_enable failed");

	led::off<led::Led0>();

	while (true) {
		led::flash<led::Led0>();
		k_sleep(K_SECONDS(1));
	}
}
