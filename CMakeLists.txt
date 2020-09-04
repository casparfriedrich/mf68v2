cmake_minimum_required(VERSION 3.13.1)

set(BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(BOARD atsamd21g_r3_breakout)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(mf86v2)

target_sources(app PRIVATE src/main.cpp)
target_sources(app PRIVATE src/hid.cpp)
