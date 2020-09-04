#ifndef LEDS_HPP
#define LEDS_HPP

#include <drivers/gpio.h>
#include <zephyr.h>
#include <init.h>

#define FLASH_DURATION K_MSEC(50)

namespace led
{
namespace
{
	struct k_timer led_timer_0;
	struct k_timer led_timer_1;
} // namespace

struct Led0 {
	static constexpr char controller[] =
		DT_GPIO_LABEL(DT_ALIAS(led0), gpios);
	static constexpr int pin = DT_GPIO_PIN(DT_ALIAS(led0), gpios);
	static constexpr int flags = DT_GPIO_FLAGS(DT_ALIAS(led0), gpios);
	static constexpr struct k_timer &timer = led_timer_0;
};

struct Led1 {
	static constexpr char controller[] =
		DT_GPIO_LABEL(DT_ALIAS(led1), gpios);
	static constexpr int pin = DT_GPIO_PIN(DT_ALIAS(led1), gpios);
	static constexpr int flags = DT_GPIO_FLAGS(DT_ALIAS(led1), gpios);
	static constexpr struct k_timer &timer = led_timer_1;
};

template <typename Led> void on()
{
	k_timer_stop(&Led::timer);
	gpio_pin_set(device_get_binding(Led::controller), Led::pin, 1);
}

template <typename Led> void off()
{
	k_timer_stop(&Led::timer);
	gpio_pin_set(device_get_binding(Led::controller), Led::pin, 0);
}

template <typename Led> void flash()
{
	gpio_pin_set(device_get_binding(Led::controller), Led::pin, 1);
	k_timer_start(&Led::timer, FLASH_DURATION, K_FOREVER);
}

namespace
{
	int led_init(struct device *dev)
	{
		gpio_pin_configure(device_get_binding(Led0::controller),
				   Led0::pin, Led0::flags);
		k_timer_init(
			&Led0::timer,
			[](struct k_timer *timer) {
				gpio_pin_set(
					device_get_binding(Led0::controller),
					Led0::pin, 0);
			},
			[](struct k_timer *timer) {
				gpio_pin_set(
					device_get_binding(Led0::controller),
					Led0::pin, 0);
			});

		gpio_pin_configure(device_get_binding(Led0::controller),
				   Led0::pin, Led0::flags);
		k_timer_init(
			&Led1::timer,
			[](struct k_timer *timer) {
				gpio_pin_set(
					device_get_binding(Led1::controller),
					Led1::pin, 0);
			},
			[](struct k_timer *timer) {
				gpio_pin_set(
					device_get_binding(Led1::controller),
					Led1::pin, 0);
			});

		return 0;
	}

	SYS_INIT(led_init, APPLICATION, CONFIG_MAIN_THREAD_PRIORITY);
} // namespace
} // namespace led

#endif // LEDS_HPP
