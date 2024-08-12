/*
 * logs.h
 *
 * Created on: 11.07.2024 
 */

#ifndef __LOGS_H
#define __LOGS_H

#include <stdio.h>

/*! @enum LOG_TYPE_ENUM
 * @brief Log type enumeration
*/
enum LOG_TYPE_ENUM {
	LOG_ACTION_UNKNOWN = 0,
	LOG_ACTION_ACTIVATE = 1,
	LOG_ACTION_DISABLE = 2,
	LOG_ACTION_ACCESS_GRANTED = 3,
	LOG_ACTION_ACCESS_DENIED = 4,
	LOG_ACTION_LOCK = 5,
	LOG_ACTION_UNLOCK = 6,
	LOG_ACTION_SLEEP = 7
};

/*! @struct sLog
 * @brief Data structure with the log information
 * @typedef Log_t
 * @brief Data type with the log information (8 Bytes)
*/
typedef struct sLog{
		uint16_t user_id;
		uint32_t timestamp;
		uint8_t log_type;
		char padding[8 - sizeof(uint16_t) - sizeof(uint32_t) - sizeof(uint8_t)];
} __attribute__((packed)) Log_t;

extern struct k_msgq logs_add_queue;
extern struct k_msgq logs_send_queue;

#endif /* LOGS_H */