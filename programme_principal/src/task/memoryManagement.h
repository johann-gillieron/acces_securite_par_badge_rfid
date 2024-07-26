/*
 * Header file for flash_mem.c
 *
 *  Created on: 2024-07-21
 */
#ifndef __FLASH_MEM_H_
#define __FLASH_MEM_H_
#include <stdio.h>
#include "deviceInformation.h"
#include "users.h"
#include "logs.h"

// Define flash memory size and start address
#define SPI_FLASH_START_OFFSET 0x00000000
#define SPI_FLASH_SECTOR_SIZE 4096	// 4 kB
#define SPI_FLASH_NB_SECTOR 2048	// Number of sectors
#define SPI_FLASH_SIZE SPI_FLASH_SECTOR_SIZE*SPI_FLASH_NB_SECTOR	// 8 MB (4 kB * 2048 sectors) or 64 Mbits

// Define the flash memory partition
#define FLASH_MEM_OTA_START_ADDR SPI_FLASH_START_OFFSET
#define FLASH_MEM_OTA_SIZE 0x100000	// 1 MB
#define FLASH_MEM_CONFIG_START_ADDR FLASH_MEM_OTA_START_ADDR + FLASH_MEM_OTA_SIZE
#define FLASH_MEM_CONFIG_SIZE 0x1000	// 4 kB
#define FLASH_MEM_USER_DATA_START_ADDR FLASH_MEM_CONFIG_START_ADDR + FLASH_MEM_CONFIG_SIZE
#define FLASH_MEM_USER_DATA_SIZE 0x201000	// 1 MB => 16'384 users (name size 37 Char) + 4 kB for informations (nb users, table with user ID)
#define FLASH_MEM_LOG_DATA_START_ADDR FLASH_MEM_USER_DATA_START_ADDR + FLASH_MEM_USER_DATA_SIZE
#define FLASH_MEM_LOG_DATA_SIZE 0x201000	// 2 MB => 262'144 logs + 4 kB for informations (nb logs, last log send)

/*! @enum FLASH_MEM_PARTITION
 * @brief Flash memory partition enumeration
*/
enum FLASH_MEM_PARTITION{
	FLASH_MEM_PARTITION_OTA = 0,
	FLASH_MEM_PARTITION_CONFIG = 1,
	FLASH_MEM_PARTITION_USER_DATA = 2,
	FLASH_MEM_PARTITION_LOG_DATA = 3,
	FLASH_MEM_PARTITION_ALL = 4
};

/*! @enum COM_PROTOCOL
 * @brief Communication protocol enumeration
*/
void memoryManagement_thread(void);

/*! memoryManagement_thread_init initializes the memory management thread
 *  @brief This function is important to initialize the memory management thread.
 */
void memoryManagement_thread_init(void);

/*! flash_init initializes the flash device
 *  @brief This function is important to initialize the flash device before any read/write operation.
 *  @param flash The flash device to initialize
 *  @return 0 if successful, -1 otherwise
 */
int flash_init(const struct device *flash);

/*! flash_erase_partition implement the erases of the flash memory
 *  @brief Erase the flash memory for the defined partition(ENUM FLASH_MEM_PARTITION)
 *  @param memory_partition The memory partition to erase
 *
 *  @return 0 on success, negative errno code on fail.
 */
int flash_erase_partition(uint8_t memory_partition);

/*! @brief Read the device config from the flash memory
 *  @param device_config The device config to read
 *  @return 0 on success, negative errno code on fail.
 */
int flash_write_device_config(DeviceConfig_t *device_config);

/*! @brief Read the device config from the flash memory
 *  @param device_config The device config to read
 *  @return 0 on success, negative errno code on fail.
 */
int flash_read_device_config(DeviceConfig_t *device_config);

/*! @brief Search for a user by user ID in the flash memory
 *  @param desfire_id The defire ID to search
 *  @param user_found The user found data buffer
 *  @return 1 on success, 0 if not found and negative errno code on fail.
 */
int users_search_by_defire_id(uint8_t *desfire_id, UserID_t *user_found);

#endif /* FLASH_MEM_H */