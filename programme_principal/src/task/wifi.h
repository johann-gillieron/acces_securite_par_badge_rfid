/*
 * Header file for wifi.c
 *
 *  Created on: 2024-07-18
 */
#ifndef __WIFI_H
#define __WIFI_H
#include <zephyr/shell/shell.h>
#include <stdio.h>

/*! @struct sWifiConfig
 * @brief Data structure with the Wifi context information
 * @typedef WifiContext_t
 * @brief Data type with the Wifi context information
*/
typedef struct sWifiConfig{
	const struct shell *sh;
	union {
		struct {
			uint8_t connected	            : 1;
			uint8_t connect_result	        : 1;
			uint8_t twt_enabled             : 1;
			uint8_t wifi_config_finish      : 1;
			uint8_t disconnect_requested	: 1;
			uint8_t _unused		            : 3;
		};
		uint8_t all;
	};
}WifiContext_t;

/*! Communication_thread implements the task WiFi.
* 
* @brief Communication_thread makes the complete connection process, while 
*       printing by LOG commands the connection status.
*       This function is used on an independent thread.
*/
void communication_thread(void);

/*! Communication_thread_init initializes the task Wifi.
*
* @brief Wifi initialization
*/
void communication_thread_init(void);

#endif /* WIFI_H */