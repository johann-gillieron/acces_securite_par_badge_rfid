/*
 * tcp.h
 *
 * Created on: 04.07.2024 
 */

#ifndef __TCP_H
#define __TCP_H

#include <zephyr/kernel.h>
//#include "deviceInformation.h"

/*********************************
 * Enumerations and definitions  *
 ********************************/
#define USE_TCP_THREADING false

/*! @enum COM_PROTOCOL
 * @brief Communication protocol enumeration
*/
enum COM_PROTOCOL{
	COM_PROTOCOL_UDP = 0,
	COM_PROTOCOL_TCP = 1,
	COM_PROTOCOL_UNKNOWN = 2
};

/*! @enum FRAME_TYPE_ENUM
 * @brief Send data type enumeration
*/
enum FRAME_TYPE_ENUM {
	FRAME_TYPE_UNKNOWN = 0x00,
	FRAME_TYPE_LOG_DATA = 0x01,
	FRAME_TYPE_ADD_USER_DATA = 0x02,
	FRAME_TYPE_REMOVE_USER_DATA = 0x03,
	FRAME_TYPE_UPDATE_REQUEST = 0x04,
	FRAME_TYPE_TIME_SYNC = 0x05,
	FRAME_TYPE_DEVICE_CONFIG = 0x06,
	FRAME_TYPE_DISCONNECT = 0x07
};

/*! @struct sTcpContext
 * @brief Data structure with the communication context information
 * @typedef TcpContext_t
 * @brief Data type with the communication context information
*/
typedef struct sTcpContext{
	/* global info */
	volatile bool pause_traffic;
	bool server_connected;
	bool data_received;
	uint8_t nb_data_to_receive;
	int tcp_sock_fd;
	char buffer_logs[CONFIG_NET_BUF_DATA_SIZE];
	uint16_t buffer_logs_len;
	uint16_t size_off_payload;
	char buffer_rx[CONFIG_NET_BUF_DATA_SIZE];
	uint16_t buffer_rx_len;
	
	/* Kconfig info */
	const unsigned char *server_ip;
	int port;
}TcpContext_t;

/*********************************
 * 		Function prototypes      *
 ********************************/
/*! @brief Function to handle the communication loop
 * 
 * @param context Pointer to the TCP context
 * @return 1 if the communication exit because server disconnected, 2 if it is because update is pending, negative otherwise
 */
int communication_loop(TcpContext_t *context);

/*! @brief Function to initialize the TCP client
 * 
 * @param context Pointer to the TCP context
 * @return bool true if the initialization is successful, false otherwise
 */
bool tcp_client_init(TcpContext_t *context);

/*! @brief Function to start the TCP tests
 * 
 * @param context Pointer to the TCP context
 * @param duration Duration of the test in seconds
 */
void begin_tcp_tests(TcpContext_t *context, int duration);


/*! @brief Function to handle the TCP disconnection
 * 
 * @param context Pointer to the TCP context
*/
void tcp_disconnected(TcpContext_t *context);

#endif /* TCP_H */