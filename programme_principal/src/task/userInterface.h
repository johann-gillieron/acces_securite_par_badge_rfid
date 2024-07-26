/* @brief User interface for the TCP tests
 * 
 * @details This file contains the user interface for the TCP tests. It is used to interact with the user and to get the necessary input for the tests.
 * 
 * @date 2024-07-21
 */

#ifndef __USER_INTERFACE_H
#define __USER_INTERFACE_H

#include "deviceInformation.h"

#define TIME_ADD_OR_MINUS 600 // Time to add or minus in seconds for the session (10 minute)
#define TIME_MAX_SESSION 21600 // Maximum time for a session in seconds (6 hours)
#define SCREEN_LOCK_TIME 30 // Time to lock the screen in seconds

/*! @struct sSessionContext
 * @brief Data structure with the session context information
 * @typedef SessionContext_t
 * @brief Data type with the session context information
 */
typedef struct sSessionContext{
		uint16_t user_id;
        uint8_t user_name_size;
		uint8_t user_name[MAX_USER_NAME_SIZE];
		uint8_t phone_number[PHONE_NUMBER_SIZE];
		uint8_t desfire_id[DESFIRE_ID_SIZE];
        uint8_t user_type;
}SessionContext_t;

enum USER_INTERFACE_STATE_ENUM {
    USER_INTERFACE_SLEEP = 0,
    USER_INTERFACE_AUTHENTICATION = 1,
    USER_INTERFACE_ACCESS_DENIED = 2,
    USER_INTERFACE_ACCESS_GRANTED = 3,
    USER_INTERFACE_SESSION_ACTIF = 4,
    USER_INTERFACE_SESSION_SLEEP = 5,
    USER_INTERFACE_SESSION_END = 6,
    USER_INTERFACE_MAINTENANCE_LOCK = 7,
};


/*! userInterface_init initializes the user interface
 *  @brief This function is the user interface thread.
*/
void userInterface_thread(void);

/*! userInterface_thread_init initializes the user interface thread
 *  @brief This function is important to initialize the user interface thread.
*/
void userInterface_thread_init(void);

#endif /* USER_INTERFACE_H */