/*! @file flash_mem.c
 *  @brief A flash application to read and write data to the flash memory.
 *  @note Based on the zephyr sample spi_flash with Copyright (c) 2016 Intel Corporation.
 *
 * @author Johann Gilliéron
 * @date 2024-07-21
 */

#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
LOG_MODULE_REGISTER(memory_management, CONFIG_LOG_DEFAULT_LEVEL);

#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>

#include <stdio.h>

#include "memoryManagement.h"

const struct device *flash_dev;
bool flash_init_done = false;

K_MSGQ_DEFINE(users_add_queue, sizeof(UserID_t), 512, 1);
K_MSGQ_DEFINE(users_id_remove_queue, sizeof(uint16_t), 512, 1);
K_MSGQ_DEFINE(logs_add_queue, sizeof(Log_t), 64, 1);
K_MSGQ_DEFINE(logs_send_queue, sizeof(Log_t), 180, 1);

/*********************************
 * 		Thread Configuration 	 *
 ********************************/
// Memory Management thread priority level
#define MEMORYMANAGEMENT_STACK_SIZE 8192
// Memory Management thread priority level
#define MEMORYMANAGEMENT_PRIORITY 8
// Memory Management stack definition
K_THREAD_STACK_DEFINE(MEMORYMANAGEMENT_STACK, MEMORYMANAGEMENT_STACK_SIZE);
// Variable to identify the Memory Management thread
static struct k_thread memoryManagementThread;

/****************************
 * Utility flash functions	*
 ***************************/
/*! @brief Erase the flash memory for the defined partition(ENUM FLASH_MEM_PARTITION)
 *  @param memory_partition The memory partition to erase
 *
 *  @return 0 on success, negative errno code on fail.
 */
int flash_erase_partition(uint8_t memory_partition)
{
    int ret;
    switch(memory_partition)
    {
        case FLASH_MEM_PARTITION_OTA:
			LOG_WRN("Erasing flash memory OTA partition");
            ret = flash_erase(flash_dev, FLASH_MEM_OTA_START_ADDR, FLASH_MEM_OTA_SIZE);
            break;
        case FLASH_MEM_PARTITION_CONFIG:
			LOG_WRN("Erasing flash memory CONFIG partition");
            ret = flash_erase(flash_dev, FLASH_MEM_CONFIG_START_ADDR, FLASH_MEM_CONFIG_SIZE);
            break;
        case FLASH_MEM_PARTITION_USER_DATA:
			LOG_WRN("Erasing flash memory USER DATA partition");
            ret = flash_erase(flash_dev, FLASH_MEM_USER_DATA_START_ADDR, FLASH_MEM_USER_DATA_SIZE);
            break;
        case FLASH_MEM_PARTITION_LOG_DATA:
			LOG_WRN("Erasing flash memory LOG DATA partition");
            ret = flash_erase(flash_dev, FLASH_MEM_LOG_DATA_START_ADDR, FLASH_MEM_LOG_DATA_SIZE);
            break;
        case FLASH_MEM_PARTITION_ALL:
			LOG_WRN("Erasing all flash memory partition");
            ret = flash_erase(flash_dev, FLASH_MEM_OTA_START_ADDR, FLASH_MEM_OTA_SIZE);
            if(ret != 0) break;
			LOG_WRN("Flash memory OTA partition erased");
            ret = flash_erase(flash_dev, FLASH_MEM_CONFIG_START_ADDR, FLASH_MEM_CONFIG_SIZE);
            if(ret != 0) break;
			LOG_WRN("Flash memory CONFIG partition erased");
            ret = flash_erase(flash_dev, FLASH_MEM_USER_DATA_START_ADDR, FLASH_MEM_USER_DATA_SIZE);
            if(ret != 0) break;
			LOG_WRN("Flash memory USER DATA partition erased");
            ret = flash_erase(flash_dev, FLASH_MEM_LOG_DATA_START_ADDR, FLASH_MEM_LOG_DATA_SIZE);
			LOG_WRN("Flash memory LOG DATA partition erased");
			break;
		default:
			ret = -1;
			break;
    }

	return ret;
}

/*! @brief Initialize the flash device
 *  @return 0 if successful, -1 otherwise
 */
int flash_init(const struct device *flash)
{
    flash_dev = flash;
	if (!device_is_ready(flash_dev)) {
		return -1;
	}

	size_t ret = flash_get_write_block_size(flash_dev);
	LOG_INF("Flash minimum write block size: %zu bytes", ret);
	ret = flash_get_page_count(flash_dev);
	LOG_INF("Flash page count: %zu", ret);
    flash_init_done = true;
	
	return 0;
}

/************************
 * Logs flash functions	*
 ***********************/
/*! @brief Read logs data from the flash memory
 *  @param logs_idx The actual index of logs buffer
 *  @param last_log_send The last log send buffer
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_logs_data(uint32_t *logs_idx, uint32_t *last_log_send) 
{
	int ret;
	ret = flash_read(flash_dev, FLASH_MEM_LOG_DATA_START_ADDR, logs_idx, sizeof(uint32_t));
	if (ret != 0) {
		LOG_ERR("Failed to read the actual index of logs from flash! %d", ret);
		return ret;
	}
	ret = flash_read(flash_dev, FLASH_MEM_LOG_DATA_START_ADDR + sizeof(uint32_t), last_log_send, sizeof(uint32_t));
	if (ret != 0) {
		LOG_ERR("Failed to read the last log index sent from flash! %d", ret);
	}
	return ret;
}

/*! @brief Write logs data to the flash memory
 *  @param logs_idx The actual index of logs buffer
 *  @param last_log_send The last log send buffer
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_write_logs_data(uint32_t logs_idx, uint32_t last_log_send) 
{
	int ret;
	off_t addr = FLASH_MEM_LOG_DATA_START_ADDR;

	// Need to erase the first sector to write the new logs data
	ret = flash_erase(flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
	if (ret != 0) {
		LOG_ERR("Failed to erase the actual index of logs from flash! %d", ret);
		return ret;
	}

	ret = flash_write(flash_dev, addr, &logs_idx, sizeof(uint32_t));
	if (ret != 0) {
		LOG_ERR("Failed to write the actual index of logs to flash! %d", ret);
		return ret;
	}

	addr += sizeof(uint32_t);
	ret = flash_write(flash_dev, addr, &last_log_send, sizeof(uint32_t));
	if (ret != 0) {
		LOG_ERR("Failed to write the last log index sent to flash! %d", ret);
	}
	return ret;
}

/*! @brief Read a log from the flash memory
 *  @param logs_idx The index of logs buffer
 *  @param log_data The log data buffer to write
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_log(uint32_t log_idx, Log_t *log_data)
{
	if (log_idx >= (FLASH_MEM_LOG_DATA_SIZE - SPI_FLASH_SECTOR_SIZE) / sizeof(Log_t)) {
		LOG_ERR("Log number too high! %d", log_idx);
		return -1;
	}
	int ret;
	ret = flash_read(flash_dev, FLASH_MEM_LOG_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + log_idx * sizeof(Log_t), log_data, sizeof(Log_t));
	if (ret != 0) {
		LOG_ERR("Failed to read log data from flash! %d", ret);
	}
	return ret;
}

/*! @brief Write a log to the flash memory
 *  @param logs_idx The actual index of logs buffer
 *  @param log_data The log data buffer to write
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_write_log(uint32_t log_idx, Log_t *log_data)
{
	int ret;
	off_t addr = FLASH_MEM_LOG_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + log_idx * sizeof(Log_t);
	if (log_idx >= (FLASH_MEM_LOG_DATA_SIZE - SPI_FLASH_SECTOR_SIZE) / sizeof(Log_t)) {
		LOG_ERR("Log number too high! %d", log_idx);
		return -1;
	}

	if (log_idx % (SPI_FLASH_SECTOR_SIZE / sizeof(Log_t)) == 0) {
		// Need to erase the first sector to write the new logs data
		ret = flash_erase (flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
		if (ret != 0) {
			LOG_ERR("Failed to erase the actual index of logs from flash! %d", ret);
			return ret;
		}
	}

	ret = flash_write(flash_dev, addr, log_data, sizeof(Log_t));
	if (ret != 0) {
		LOG_ERR("Failed to write log data to flash! %d", ret);
	}
	return ret;
}

/****************************
 * Users flash functions	*
 ***************************/
/*! @brief Read users data from the flash memory
 *  @param nb_users The user number buffer (can be use as index)
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_users_data(uint16_t *nb_users)
{
	int ret;
	ret = flash_read(flash_dev, FLASH_MEM_USER_DATA_START_ADDR, nb_users, sizeof(uint16_t));
	if (ret != 0) {
		LOG_ERR("Failed to read number of users from flash! %d", ret);
	}
	return ret;
}

/*! @brief Write users data to the flash memory
 *  @param nb_users The user number buffer
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_write_users_data(uint16_t nb_users)
{
	int ret;
	off_t addr = FLASH_MEM_USER_DATA_START_ADDR;

	// Need to erase the first sector to write the number of users
	ret = flash_erase (flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
	if (ret != 0) {
		LOG_ERR("Failed to erase the actual index of logs from flash! %d", ret);
		return ret;
	}

	ret = flash_write(flash_dev, addr, &nb_users, sizeof(uint16_t));
	if (ret != 0) {
		LOG_ERR("Failed to write number of users to flash! %d", ret);
	}
	return ret;
}

/*! @brief Read user from the flash memory
 *  @param user_data The user data buffer
 *  @param user_no The user place in flash to read (0 = first user, 1 = second user, ...)
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_user_id(UserID_t *user_data, uint16_t user_no)
{
	int ret;

	if (user_no >= (FLASH_MEM_USER_DATA_SIZE - SPI_FLASH_SECTOR_SIZE)  / sizeof(UserID_t)) {
		LOG_ERR("User number too high! %d", user_no);
		return -1;
	}

	ret = flash_read(flash_dev, FLASH_MEM_USER_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + user_no * sizeof(UserID_t), user_data, sizeof(UserID_t));
	if (ret != 0) {
		LOG_ERR("Failed to read user data from flash! %d", ret);
	}
	return ret;
}

/*! @brief Write user id to the flash memory
 *  @param user_data The user data buffer to write
 *  @param user_no The user place in flash to write (0 = first user, 1 = second user, ...)
 * 	@param flag_last_user Flag to indicate if it is the last user
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_write_user_id(UserID_t *user_data, uint16_t user_no, uint8_t flag_last_user)
{
	int ret;
	char data_buf[SPI_FLASH_SECTOR_SIZE];
	off_t addr = FLASH_MEM_USER_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + user_no * sizeof(UserID_t);
	off_t addr_start_offset = (addr % SPI_FLASH_SECTOR_SIZE);
	off_t addr_sector = addr - addr_start_offset; // Get the sector start address
	
	if (user_no >= (FLASH_MEM_USER_DATA_SIZE - SPI_FLASH_SECTOR_SIZE) / sizeof(UserID_t)) {
		LOG_ERR("User number too high! %d", user_no);
		return -1;
	}

    if(flag_last_user)
    {
        if(addr_start_offset == 0)
        {
            ret = flash_erase(flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
            if (ret != 0) {
                LOG_ERR("Failed to erase user data from flash! %d", ret);
                return ret;
            }
        }
        ret = flash_write(flash_dev, addr, user_data, sizeof(UserID_t));
        if (ret != 0) 
        {
            LOG_ERR("Failed to write user data to flash! %d", ret);
            return ret;
        }
    }
    else
    {
        // Read the sector to keep the other
        ret = flash_read(flash_dev, addr_sector, data_buf, SPI_FLASH_SECTOR_SIZE);
        if (ret != 0) {
            LOG_ERR("Failed to read user data from flash! %d", ret);
            return ret;
        }

        // Write the new user data
        memcpy(data_buf + addr_start_offset, user_data, sizeof(UserID_t));

        // Need to erase the first sector to write the new user data
        ret = flash_erase(flash_dev, addr_sector, SPI_FLASH_SECTOR_SIZE);
        if (ret != 0) {
            LOG_ERR("Failed to erase user data from flash! %d", ret);
            return ret;
        }

        // Write the sector with the new user id data
        ret = flash_write(flash_dev, addr_sector, data_buf, SPI_FLASH_SECTOR_SIZE);

        if (ret != 0) {
            LOG_ERR("Failed to write user data to flash! %d", ret);
        }

    }
    flash_read(flash_dev, addr, data_buf, sizeof(UserID_t));
    LOG_HEXDUMP_INF(data_buf, sizeof(UserID_t), "User data written to flash:");
	return ret;
}

/*! @brief Delete the last user from the flash memory
 *  @param user_no The user place in flash to delete (0 = first user, 1 = second user, ...)
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_delete_last_user(uint16_t user_no)
{
    int ret;
    off_t addr = FLASH_MEM_USER_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + user_no * sizeof(UserID_t);
    ret = flash_erase(flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
    if (ret != 0) {
        LOG_ERR("Failed to erase user data from flash! %d", ret);
        return ret;
    }
    return ret;
}

/*! @brief Read user desfire id only from the flash memory
 *  @param user_no The user place in flash to read (0 = first user, 1 = second user, ...)
 *  @param desfire_id The desfire id data buffer
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_user_desfire_id(uint16_t user_no, uint8_t *desfire_id)
{
	int ret;

	if (user_no >= (FLASH_MEM_USER_DATA_SIZE - SPI_FLASH_SECTOR_SIZE) / sizeof(UserID_t)) {
		LOG_ERR("User number too high! %d", user_no);
		return -1;
	}

	ret = flash_read(flash_dev, FLASH_MEM_USER_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + (user_no + 1) * sizeof(UserID_t) - DESFIRE_ID_SIZE , desfire_id, DESFIRE_ID_SIZE);
	if (ret < 0) {
		LOG_ERR("Failed to read user desfire id from flash! %d", ret);
        return ret;
	}
	return 0;
}

/*! @brief Read user id number from the flash memory
 *  @param user_no The user place in flash to read (0 = first user, 1 = second user, ...)
 *  @param user_id_number The user id number buffer
 *  @return 0 on success, negative errno code on fail.
 */
static int flash_read_user_id_number(uint16_t user_no, uint16_t *user_id_number)
{
	int ret;

	if (user_no >= (FLASH_MEM_USER_DATA_SIZE - SPI_FLASH_SECTOR_SIZE) / sizeof(UserID_t)) {
		LOG_ERR("User number too high! %d", user_no);
		return -1;
	}

	ret = flash_read(flash_dev, FLASH_MEM_USER_DATA_START_ADDR + SPI_FLASH_SECTOR_SIZE + user_no * sizeof(UserID_t), user_id_number, sizeof(uint16_t));
	if (ret < 0) {
		LOG_ERR("Failed to read user id number from flash!");
        return ret;
	}
	return 0;
}


/*! @brief Search for a user by user ID
 *  @param desfire_id The defire ID to search
 *  @param user_found The user found data buffer
 *  @return 1 on success, 0 if not found and negative errno code on fail.
 */
int users_search_by_defire_id(uint8_t *desfire_id, UserID_t *user_found)
{   
    uint16_t nb_users;
    int ret;
    ret = flash_read_users_data(&nb_users);
    if (ret != 0)
    {
        LOG_ERR("Failed to read the number of users from flash! %d\n", ret);
        return ret;
    }
    uint8_t defireID_loaded[DESFIRE_ID_SIZE];
    for (uint16_t i = 0; i < nb_users; i++)
    {
        ret = flash_read_user_desfire_id(i, defireID_loaded);
        if (ret == 0 && memcmp(desfire_id, defireID_loaded, DESFIRE_ID_SIZE) == 0)
        {
            ret = flash_read_user_id(user_found, i);
            if(ret != 0)
            {
                LOG_ERR("Failed to read the user from flash! %d\n", ret);
                return ret;
            }
            return 1;
        }
        else if(ret < 0)
        {
            LOG_ERR("Flash read error");
            return ret;
        }
    }
    return 0;
}

/*! @brief Search for a user by user ID number
 *  @param user_id_num The user ID number to search
 *  @param idx The index of the user
 *  @param nb_users The number of users
 *  @return 1 on success, 0 if not found and negative errno code on fail.
 */
static int users_search_by_user_id_number(uint16_t user_id_num, uint16_t *idx, uint16_t *nb_users)
{
    int ret;
    uint16_t user;
    for (uint16_t i = 0; i < *nb_users; i++)
    {
        ret = flash_read_user_id_number(i, &user);
        LOG_INF("Idx : %d : User id number: %d", i, user);
        if (ret == 0 && user == user_id_num)
        {
            *idx = i;
            return 1;
        }
        else if (ret < 0)
        {
            LOG_ERR("Flash read error");
            return ret;
        }
    }
    return 0;
}

/*! @brief Initialize the users data
 *  @param new_user The new user to add
 *  @param nb_users The number of users
 *  @return 0 on success, negative errno code on fail.
 */
static int users_add_new(UserID_t new_user, uint16_t *nb_users)
{
    int ret;
    uint16_t idx;
    if (*nb_users >= MAX_USERS)
    {
        LOG_ERR("Cannot add new user, max number of users reached!");
        return -ENOMEM;
    }

    ret = users_search_by_user_id_number(new_user.user_id_number, &idx, nb_users);
    if (ret == 1)
    {
        LOG_WRN("User %d already found!", new_user.user_id_number);
        return 0;
    }
    
    // write the new user to flash
    ret = flash_write_user_id(&new_user, *nb_users, true);
    if (ret != 0)
    {
        LOG_ERR("Failed to write the new user to flash! %d\n", ret);
        return ret;
    }
    (*nb_users)++;

    return ret;
}

/*! @brief Remove a user by user ID number
 *  @param user_id The user ID number to remove
 *  @return 0 on success, negative errno code on fail.
 */
static int users_remove_user(uint16_t user_id, uint16_t *nb_users)
{
    int ret;
    uint16_t idx = 0;
    UserID_t last_user;
    if (*nb_users == 0)
    {
        LOG_ERR("No more user to remove!");
        return 0;
    }

    ret = users_search_by_user_id_number(user_id, &idx, nb_users);
    LOG_INF("User id to remove: %d", user_id);
    if (ret < 0)
    {
        LOG_ERR("Error while searching for the user to remove! %d\n", ret);
        return ret;
    } else if (ret == 0)
    {
        LOG_WRN("User not found!");
        return 0;
    }

    // read the last user
    ret = flash_read_user_id(&last_user, *nb_users - 1);
    if (ret != 0)
    {
        LOG_ERR("Failed to read the last user from flash! %d", ret);
        return ret;
    }

    // write the last user to the position of the user to remove
    ret = flash_write_user_id(&last_user, idx, false);
    if (ret != 0)
    {
        LOG_ERR("Failed to write the last user to the position of the user to remove! %d", ret);
        return ret;
    }
    (*nb_users)--;
    ret = flash_delete_last_user(*nb_users);
    return ret;
}

/************************************
 * Device config flash functions	*
 ***********************************/
/*! @brief Read the device config from the flash memory
 *  @param device_config The device config to read
 *  @return 0 on success, negative errno code on fail.
 */
int flash_write_device_config(DeviceConfig_t *device_config)
{
	int ret;
	off_t addr = FLASH_MEM_CONFIG_START_ADDR;

	// Need to erase the first sector to write the new device config
	ret = flash_erase(flash_dev, addr, SPI_FLASH_SECTOR_SIZE);
	if (ret != 0) {
		LOG_ERR("Failed to erase device config from flash! %d", ret);
		return ret;
	}

	ret = flash_write(flash_dev, addr, device_config, sizeof(DeviceConfig_t));
	if (ret != 0) {
		LOG_ERR("Failed to write device config to flash! %d", ret);
	}
	return ret;
}

/*! @brief Read the device config from the flash memory
 *  @param device_config The device config to read
 *  @return 0 on success, negative errno code on fail.
 */
int flash_read_device_config(DeviceConfig_t *device_config)
{
	int ret;
	ret = flash_read(flash_dev, FLASH_MEM_CONFIG_START_ADDR, device_config, sizeof(DeviceConfig_t));
	if (ret != 0) {
		LOG_ERR("Failed to read device config from flash! %d", ret);
	}
	return ret;
}


/****************************
 * MemoryManagement thread	*
 ***************************/
void memoryManagement_thread()
{
    int ret;
    uint32_t last_log_sent;
    uint32_t logs_idx;
    UserID_t new_user;
    uint16_t user_id;
    uint16_t nb_users;
    Log_t new_log;
    Log_t log;
    ret = flash_read_users_data(&nb_users);
    if (ret != 0)
    {
        LOG_ERR("Failed to read the number of users from flash! %d\n", ret);
    }
    else
    {
        LOG_INF("Number of users in data: %d", nb_users);
        if (nb_users > MAX_USERS)
        {
            nb_users = 0;
            ret = flash_write_users_data(nb_users);
            if (ret != 0)
            {
                LOG_ERR("Failed to write the number of users to flash! %d\n", ret);
            }
        }
    }
    ret = flash_read_logs_data(&logs_idx, &last_log_sent);
    if (ret != 0)
    {
        LOG_ERR("Flash read logs config sent error");
        return;
    }
    LOG_INF("Logs data loaded: logs_idx = %d, last_log_sent = %d", logs_idx, last_log_sent);
    if (logs_idx >= MAX_LOGS)
    {
        logs_idx = 0;
        last_log_sent = 0;
        ret = flash_write_logs_data(logs_idx, last_log_sent);
        if (ret != 0)
        {
            LOG_ERR("Flash write logs data error");
            return;
        }
    }

    LOG_INF("== MemoryManagement thread started ==");

    while(1)
    {
        LOG_INF("MemoryManagement thread running");
        // Check if there is user to remove
        while (k_msgq_get(&users_id_remove_queue, &user_id, K_NO_WAIT) == 0)
        {
            LOG_INF("User removed: %d", user_id);
            ret = users_remove_user(user_id, &nb_users);
            if (ret < 0)
            {
                LOG_ERR("Failed to remove the user id from the flash! %d\n", ret);
            }
            else
            {
                ret = flash_write_users_data(nb_users);
                if (ret != 0)
                {
                    LOG_ERR("Failed to write the number of users to flash! %d\n", ret);
                }
            }
        }

        // Check if there is new user to add
        while (k_msgq_get(&users_add_queue, &new_user, K_NO_WAIT) == 0)
        {
            LOG_INF("New user added");
            ret = users_add_new(new_user, &nb_users);
            if (ret != 0)
            {
                LOG_ERR("Failed to write the user id to flash! %d\n", ret);
            }
            else
            {
                ret = flash_write_users_data(nb_users);
                if (ret != 0)
                {
                    LOG_ERR("Failed to write the number of users to flash! %d\n", ret);
                }
            }
        }

        // Check if there is new log to add
        while (k_msgq_get(&logs_add_queue, &new_log, K_NO_WAIT) == 0)
        {
            if (last_log_sent != logs_idx)
            {
                LOG_INF("Last log sent: %d", last_log_sent);
                LOG_INF("Logs index: %d", logs_idx);
                while(last_log_sent != logs_idx)
                {
                    ret = flash_read_log(last_log_sent, &log);
                    if (ret != 0)
                    {
                        LOG_ERR("Failed to read the last log from flash! %d\n", ret);
                    }
                    else
                    {
                        ret = k_msgq_put(&logs_send_queue, &log, K_NO_WAIT);
                        if (ret != 0)
                        {
                            LOG_ERR("Failed to put the last log in the send queue! %d\n", ret);
                            break;
                        }
                        else
                        {
                            last_log_sent++;
                            if (last_log_sent >= MAX_LOGS)
                            {
                                last_log_sent = 0;
                            }
                            ret = flash_write_logs_data(logs_idx, last_log_sent);
                            if (ret != 0)
                            {
                                LOG_ERR("Failed to write the logs data to flash! %d\n", ret);
                                break;
                            }
                        }
                    }
                }
            }
                
            LOG_INF("New log added");
            ret = flash_write_log(logs_idx, &log);
            if (ret != 0)
            {
                LOG_ERR("Failed to write the log to flash! %d\n", ret);
            }
            else
            {
                ret = k_msgq_put(&logs_send_queue, &new_log, K_NO_WAIT);
                if (ret != 0)
                {
                    LOG_ERR("Failed to put the log in the send queue! %d\n", ret);
                    logs_idx++;
                    ret = flash_write_logs_data(logs_idx, last_log_sent);
                    if (ret != 0)
                    {
                        LOG_ERR("Failed to write the logs data to flash! %d\n", ret);
                    }
                }
                else
                {
                    last_log_sent = logs_idx;
                    logs_idx++;
                    ret = flash_write_logs_data(logs_idx, last_log_sent);
                    if (ret != 0)
                    {
                        LOG_ERR("Failed to write the logs data to flash! %d\n", ret);
                    }
                }
            }
        }
        k_sleep(K_SECONDS(5)); // Sleep 5 seconds
    }
    k_sleep(K_FOREVER);
}

void memoryManagement_thread_init(){
    if (!flash_init_done) {
        LOG_ERR("Flash memory not initialized!");
        LOG_ERR("Please call flash_init() before using the flash memory");
        return;
    }
    k_thread_create(&memoryManagementThread, MEMORYMANAGEMENT_STACK, K_THREAD_STACK_SIZEOF(MEMORYMANAGEMENT_STACK),
            (k_thread_entry_t)memoryManagement_thread, NULL, NULL, NULL, MEMORYMANAGEMENT_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&memoryManagementThread, "MemoryManagement");
    k_thread_start(&memoryManagementThread);
}