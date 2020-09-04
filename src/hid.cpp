#include "hid.hpp"
#include "keymap.hpp"
#include "led.hpp"

#include <drivers/gpio.h>
#include <logging/log.h>
#include <usb/class/usb_hid.h>
#include <usb/usb_device.h>
#include <zephyr.h>
#include <devicetree.h>

LOG_MODULE_DECLARE(app, LOG_LEVEL_DBG);

#define foo(x)                                                                 \
	{                                                                      \
		DT_GPIO_LABEL(DT_ALIAS(x), gpios),                             \
			DT_GPIO_PIN(DT_ALIAS(x), gpios),                       \
			DT_GPIO_FLAGS(DT_ALIAS(x), gpios)                      \
	}

K_SEM_DEFINE(app_hid_init, 0, 1);
K_SEM_DEFINE(app_hid_ready, 0, 1); // main.cpp

static const Gpio rows[] = {
	foo(drive0), foo(drive1), foo(drive2), foo(drive3),
	foo(drive4), foo(drive5), foo(drive6), foo(drive7),
};

static const Gpio cols[] = {
	foo(sense0), foo(sense1), foo(sense2), foo(sense3), foo(sense4),
	foo(sense5), foo(sense6), foo(sense7), foo(sense8),
};

Row::Row(Gpio row) : ctrl_(device_get_binding(row.ctrl)), pin_(row.pin)
{
	gpio_pin_set(ctrl_, pin_, 0);
}

Row::~Row()
{
	gpio_pin_set(ctrl_, pin_, 1);
}

int Row::get(Gpio col)
{
	return gpio_pin_get(device_get_binding(col.ctrl), col.pin);
}

// Host <-- Device (Keys)
static void input_ready_cb(void)
{
	k_sem_give(&app_hid_ready);
}

// Host --> Device (LEDs)
static void output_ready_cb(void)
{
	struct device *kbd_dev = device_get_binding("HID_0");
	if (!kbd_dev) {
		LOG_WRN("device_get_binding failed");
		return;
	}

	static u8_t report;
	hid_int_ep_read(kbd_dev, &report, sizeof(report), NULL);

	if (report & HID_KBD_LED_CAPS_LOCK) {
		led::on<led::Led1>();
	} else {
		led::off<led::Led1>();
	}
}

static void app_hid_fn(void *arg0, void *arg1, void *arg2)
{
	int err __unused = 0;

	struct device *kbd_dev = device_get_binding("HID_0");
	__ASSERT_NO_MSG(kbd_dev);

	const u8_t kbd_desc[] = HID_KEYBOARD_REPORT_DESC();
	const struct hid_ops callbacks = {
		.int_in_ready = input_ready_cb,
		.int_out_ready = output_ready_cb,
	};
	usb_hid_register_device(kbd_dev, kbd_desc, sizeof(kbd_desc),
				&callbacks);

	err = usb_hid_init(kbd_dev);
	__ASSERT(!err, "usb_hid_init failed");

	k_sem_give(&app_hid_init);

	static u8_t report[8] = {};

	while (true) {
		u8_t new_report[sizeof(report)] = {};
		u32_t idx = 2;

		for (u8_t i = 0; i < ARRAY_SIZE(rows); i++) {
			Row row(rows[i]);

			for (u8_t j = 0; j < ARRAY_SIZE(cols); j++) {
				if (idx >= sizeof(report)) {
					continue;
				}

				if (!row.get(cols[j])) {
					if (keymap[i][j].modifier) {
						new_report[0] |=
							keymap[i][j].scancode;
					} else {
						new_report[idx++] =
							keymap[i][j].scancode;
					}
				}
			}
		}

		if (memcmp(report, new_report, sizeof(report))) {
			k_sem_take(&app_hid_ready, K_FOREVER);
			hid_int_ep_write(kbd_dev, new_report,
					 sizeof(new_report), NULL);
			memcpy(report, new_report, sizeof(report));
		}

		k_sleep(K_MSEC(10));
	}
}

K_THREAD_DEFINE(app_hid, KB(1), app_hid_fn, NULL, NULL, NULL, 3, 0, 0);
