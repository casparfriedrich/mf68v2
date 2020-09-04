#ifndef HID_HPP_
#define HID_HPP_

#include <drivers/gpio.h>
#include <zephyr.h>

struct Gpio {
	const char *ctrl;
	const int pin;
	const int flags;
};

struct Row {
	Row(Gpio row);
	~Row();

	int get(Gpio col);

    private:
	struct device *ctrl_;
	int pin_;
};

#endif /* HID_HPP_ */
