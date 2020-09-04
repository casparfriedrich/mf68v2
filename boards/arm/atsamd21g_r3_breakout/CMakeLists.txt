if(CONFIG_PINMUX_SAM0)
	zephyr_library()
	zephyr_library_sources(board.c)
endif(CONFIG_PINMUX_SAM0)
