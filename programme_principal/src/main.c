/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 * @brief WiFi TWT sample
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app, CONFIG_LOG_DEFAULT_LEVEL);

#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/crc.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/sys/printk.h>
#include <zephyr/init.h>

#include "task/wifi.h"
#include "task/deviceInformation.h"
#include "task/memoryManagement.h"
#include "task/userInterface.h"


#if !DT_NODE_EXISTS(DT_NODELABEL(clrc663_0))
#error "whoops, node label clrc663_0 not found"
#endif

const struct device *clrc663 = DEVICE_DT_GET(DT_NODELABEL(clrc663_0));

#if !DT_NODE_EXISTS(DT_NODELABEL(mx25r64))
#error "whoops, node label mx25r64 (Flash SPI) not found"
#endif
const struct device *flash_device = DEVICE_DT_GET(DT_NODELABEL(mx25r64));


const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

DeviceConfig_t device_config;
uint32_t time_offset = 0;

void load_device_config(void)
{
	uint16_t crc;
	int ret;

	// Load the device configuration from the flash memory
	ret = flash_read_device_config(&device_config);
	if (ret != 0) {
		LOG_ERR("Failed to load the device configuration from the flash memory");
	}

	// Check if the device configuration is valid
	crc = crc16_ansi((char *)&device_config, sizeof(DeviceConfig_t) - sizeof(uint16_t));
	if(crc != device_config.CRC_16_ansi) {
		LOG_WRN("Device configuration is invalid, set the device configuration to the default values");
		// Set the device configuration to the default values
		if (device_config.device_id == 0xff) {
			device_config.device_id = DEFAULT_DEVICE_ID;
		}
		device_config.device_type = DEFAULT_DEVICE_TYPE;
		device_config.device_status = DEVICE_STATUS_OK;
		device_config.device_major_version = DEFAULT_DEVICE_MAJOR_VERSION;
		device_config.device_minor_version = DEFAULT_DEVICE_MINOR_VERSION;
		crc = crc16_ansi((char *)&device_config, sizeof(DeviceConfig_t) - sizeof(uint16_t));
		device_config.CRC_16_ansi = crc;
		// Save the device configuration to the flash memory
		ret = flash_write_device_config(&device_config);
		if (ret != 0) {
			LOG_ERR("Failed to save the device configuration to the flash memory");
		}
	} else {
		LOG_INF("Device configuration loaded successfully");
	}

	// check if the device is updated or not
	if (device_config.device_major_version != DEFAULT_DEVICE_MAJOR_VERSION || device_config.device_minor_version != DEFAULT_DEVICE_MINOR_VERSION) {
		LOG_WRN("Device configuration is outdated, set the device configuration to the new version values");
		// Set the device configuration to the default values
		device_config.device_type = DEFAULT_DEVICE_TYPE;
		device_config.device_status = DEVICE_STATUS_OK;
		device_config.device_major_version = DEFAULT_DEVICE_MAJOR_VERSION;
		device_config.device_minor_version = DEFAULT_DEVICE_MINOR_VERSION;
		crc = crc16_ansi((char *)&device_config, sizeof(DeviceConfig_t) - sizeof(uint16_t));
		device_config.CRC_16_ansi = crc;
		// Save the device configuration to the flash memory
		ret = flash_write_device_config(&device_config);
		if (ret != 0) {
			LOG_ERR("Failed to save the device configuration to the flash memory");
		}
	}

	// Print the device configuration for the debug
	LOG_INF("Device ID: %d", device_config.device_id);
	LOG_INF("Device type: %d", device_config.device_type);
	LOG_INF("Device status: %d", device_config.device_status);
	LOG_INF("Device major version: %d", device_config.device_major_version);
	LOG_INF("Device minor version: %d", device_config.device_minor_version);
}

void clean_flash(void)
{
	int ret;

	// Erase the flash memory for the device configuration
	ret = flash_erase_partition(FLASH_MEM_PARTITION_USER_DATA);
	if (ret != 0) {
		LOG_ERR("Failed to erase the flash memory for the user data");
	}
}

int main(void)
{
	LOG_INF("Starting %s with CPU frequency: %d MHz", CONFIG_BOARD, SystemCoreClock/MHZ(1));
	if (flash_init(flash_device) != 0) {
		LOG_ERR("Flash init failed");
		k_sleep(K_FOREVER);
	}
	//clean_flash();
	load_device_config();
	memoryManagement_thread_init();
	communication_thread_init();
	userInterface_thread_init();
	while(1)
	{
		//LOG_INF("Hello from main");
		k_sleep(K_SECONDS(1));
	}
	k_sleep(K_FOREVER);
	return 0;
}
