/*
 * users.h
 *
 * Created on: 11.07.2024 
 */

#ifndef __USERS_H
#define __USERS_H

#include <stdio.h>
#include "deviceInformation.h"

/*! @struct sUserID
 * @brief Data structure with the user ID information
 * @typedef UserID_t
 * @brief Data type with the user ID information (64 Bytes)
*/
typedef struct sUserID{
		uint16_t user_id_number;
		uint8_t user_name_size;
		char user_name[MAX_USER_NAME_SIZE];
        uint8_t user_type;
		char phone_number[PHONE_NUMBER_SIZE];
		uint8_t desfire_id[DESFIRE_ID_SIZE];
} __attribute__((packed)) UserID_t;

extern struct k_msgq users_add_queue;
extern struct k_msgq users_id_remove_queue;

#endif /* USERS_H */