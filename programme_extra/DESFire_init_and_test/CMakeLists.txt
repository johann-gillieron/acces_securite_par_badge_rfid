#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
set(SYSCALL_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../include")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C)

target_sources(app PRIVATE 
    src/main.c
    src/desfire/DesfireApp.cpp
    src/desfire/AES128.cpp
    src/desfire/DES.cpp
    src/desfire/Desfire.cpp
    src/desfire/Utils.cpp
    )
