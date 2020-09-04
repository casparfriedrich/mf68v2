#include <drivers/gpio.h>
#include <drivers/pinmux.h>
#include <init.h>
#include <soc.h>

static int board_pinmux_init(struct device *dev)
{
	ARG_UNUSED(dev);

	struct device *muxa __unused =
		device_get_binding(DT_LABEL(DT_NODELABEL(pinmux_a)));
	struct device *muxb __unused =
		device_get_binding(DT_LABEL(DT_NODELABEL(pinmux_b)));

#ifdef CONFIG_PINMUX_SAM0
#if (ATMEL_SAM0_DT_SERCOM_CHECK(5, atmel_sam0_uart) && CONFIG_UART_SAM0)
	pinmux_pin_set(muxb, 22, PINMUX_FUNC_D); // SERCOM5 (TX)
	pinmux_pin_set(muxb, 23, PINMUX_FUNC_D); // SERCOM5 (RX)
#endif

#ifdef CONFIG_USB_DC_SAM0
	pinmux_pin_set(muxa, 25, PINMUX_FUNC_G); // USB D+
	pinmux_pin_set(muxa, 24, PINMUX_FUNC_G); // USB D-
#endif
#endif

	return 0;
}

SYS_INIT(board_pinmux_init, PRE_KERNEL_1, CONFIG_PINMUX_INIT_PRIORITY);
