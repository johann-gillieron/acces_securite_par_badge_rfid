/*! @file deviceInformation.h
 * @brief This file contains the data structures and variables used between the thread functions.
 *
 * @author Johann Gilliéron
 * @date 2024-07-18
 */

#ifndef __DEVICE_DATA_H_
#define __DEVICE_DATA_H_
#include <stdio.h>

/*! @enum DEVICE_TYPE_ENUM
 * @brief Device type enumeration
*/
enum DEVICE_TYPE_ENUM {
	DEVICE_TYPE_UNKNOWN = 0,
	DEVICE_TYPE_DEVKIT = 1,
	DEVICE_TYPE_READER_HW_V1 = 2,
	DEVICE_TYPE_READER_HW_V2 = 3
};

/*! @enum DEVICE_STATUS_ENUM
 * @brief Device status enumeration
*/
enum DEVICE_STATUS_ENUM {
	DEVICE_STATUS_OK = 0,
	DEVICE_UPDATE_DOWNLOADED = 1,
	DEVICE_UPDATED = 2,
	DEVICE_STATUS_ERROR = 3
};

// Default values for the device configuration
#define DEFAULT_DEVICE_ID 0x00 // Device ID
#define DEFAULT_DEVICE_STATUS DEVICE_STATUS_OK
#define DEFAULT_DEVICE_MAJOR_VERSION CONFIG_DEVICE_MAJOR_VERSION
#define DEFAULT_DEVICE_MINOR_VERSION CONFIG_DEVICE_MINOR_VERSION
#define DEFAULT_DEVICE_FIRST_RUN true
#if CONFIG_DEVICE_TYPE_DEVKIT
#define DEFAULT_DEVICE_TYPE DEVICE_TYPE_DEVKIT
#endif
#if CONFIG_DEVICE_TYPE_READER_HW_V1
#define DEFAULT_DEVICE_TYPE DEVICE_TYPE_READER_HW_V1
#endif
#if CONFIG_DEVICE_TYPE_READER_HW_V2
#define DEFAULT_DEVICE_TYPE DEVICE_TYPE_READER_HW_V2
#endif
#if CONFIG_DEVICE_TYPE_UNKNOWN
#define DEFAULT_DEVICE_TYPE DEVICE_TYPE_UNKNOWN
#endif

// Define for the user data
#define MAX_USER_NAME_SIZE 36 // Maximum user name to print on the screen
#define PHONE_NUMBER_SIZE 16 // +41 cc ccc cc cc
#define DESFIRE_ID_SIZE 8 // 7 bytes for the UID + 1 byte for a secret byte
#define MAX_USERS 16384 // 16'384 users (64 bytes per entry) + 4 kB for informations (nb users, table with user ID) all define in memoryManagement.h
#define MAX_LOGS 262144 // 262'144 logs (8 Bytes) + 4 kB for informations (nb logs, last log send) all define in memoryManagement.h


/*! @struct sDeviceConfig
 * @brief Data structure with the device configuration information
 * @typedef DeviceConfig_t
 * @brief Data type with the device configuration information 7 bytes
*/
typedef struct sDeviceConfig{
		uint8_t device_id				: 8;
		uint8_t device_type				: 8;
		uint8_t device_status			: 8;
		uint8_t device_major_version	: 8;
		uint8_t device_minor_version	: 8;
		uint16_t CRC_16_ansi			: 16;
} __attribute__((packed)) DeviceConfig_t;

extern DeviceConfig_t device_config;

extern uint32_t time_offset;

#endif /* __DEVICE_DATA_H_ */