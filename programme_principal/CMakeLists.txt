#
# Copyright (c) 2023 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(tcp_tests)

target_include_directories(app PUBLIC ${ZEPHYR_BASE}/subsys/net/ip)

target_sources(app PRIVATE
	src/main.c
	src/task/wifi.c
	src/task/tcp.c
	src/task/memoryManagement.c
	src/task/userInterface.c
    src/task/desfire/DesfireApp.cpp
    src/task/desfire/AES128.cpp
    src/task/desfire/DES.cpp
    src/task/desfire/Desfire.cpp
    src/task/desfire/Utils.cpp
)
