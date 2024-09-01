/*! @file tcp.c
 * @brief This file contains the implementation of the TCP Client task.
 *
 * @author Johann Gilliéron
 * @date 2024-07-04
 * 
 * @note This file is inspired by the traffic_gen.c file from the Nordic sample.
 */

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(tcp, CONFIG_LOG_DEFAULT_LEVEL);

#include <nrfx_clock.h>
#include <zephyr/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <zephyr/init.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_event.h>
#include <zephyr/net/socket.h>
#include <zephyr/drivers/gpio.h>
#include <fcntl.h>
#include <errno.h>

#include "net_private.h"
#include "deviceInformation.h"
#include "tcp.h"
#include "memoryManagement.h"
#include "users.h"
#include "logs.h"

// Setting to enable or disable the TWT feature
#define TWT_USE true
#define TCP_SLEEP_TIME_MS 1000
#define TWT_AWAKE_TIME_MS (uint32_t)(CONFIG_TWT_WAKE_INTERVAL / 1000)
#define TWT_INTERVAL_TIME_MS (uint32_t)(CONFIG_TWT_INTERVAL / 1000)

/*********************************
 * Global variable configuration *
 ********************************/
//! Global variable to store the communication context
//TcpContext_t comm_context;
static bool tcp_context_initialized = false;


/*********************************
 * 		Function declarations	 *
 ********************************/
/*! @brief Function to handle the TCP disconnection
 * 
 * @param context Pointer to the TCP context
*/
void tcp_disconnected(TcpContext_t *context)
{
	context->server_connected = false;
}

/*! @brief Function to disconnect from the server
 * 
 * @param context Pointer to the TCP context
*/
void disconnect_from_server(TcpContext_t *context)
{
	char buffer_tx[1];
	buffer_tx[0] = FRAME_TYPE_DISCONNECT;
	if(context->tcp_sock_fd > 0)
	{
		send(context->tcp_sock_fd, buffer_tx, sizeof(buffer_tx), 0);
		close(context->tcp_sock_fd);
		context->server_connected = false;
		context->tcp_sock_fd = -1;
	}
}

/*! @brief Function to connect to the server
 * 
 * @param context Pointer to the TCP context
 * @return int 1 if the connection is successful, -1 otherwise
*/
static int connect_to_server(TcpContext_t *context)
{
	struct sockaddr_in serv_addr;
	int sockfd;
	if (context->server_connected)
	{
		disconnect_from_server(context);
	}

	if (context->tcp_sock_fd > 0)
	{
		close(context->tcp_sock_fd);
		context->tcp_sock_fd = -1;
	}

	/* Create tcp path socket */
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		LOG_ERR("Failed to create tcp client socket");
		return -errno;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(CONFIG_TCP_SERVER_PORT);

	/* Convert IPv4 addresses from text to binary form */
	if (inet_pton(AF_INET, context->server_ip, &serv_addr.sin_addr) <= 0) {
		LOG_ERR("Invalid address: Address not supported");
		close(sockfd);
		return -errno;
	}

	/* Connect to the TRAFFIC_GEN server */
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		LOG_ERR("Failed to connect TCP server");
		LOG_ERR("Run the TCP server in other end before running TCP serveur app");
		close(sockfd);
		return -errno;
	}
	context->server_connected = true;

	k_sleep(K_SECONDS(2));
	LOG_INF("Connected To TCP Server!!!");

	return sockfd;
}

/*! @brief Function to receive data from the server
 * 
 * @param context Pointer to the TCP context
 * @param dev_conf Pointer to the device configuration
 * @param type Pointer to the type of the received data
 * @param option Pointer to the option of the received data
 * @return int 0 on success, negative value otherwise
 */
static int receive_data_from_server(TcpContext_t *context, DeviceConfig_t *dev_conf, char *type, void* option) 
{
	int ret;
	uint32_t timestamp;
	char *buffer_rx = context->buffer_rx;
	uint16_t rx_length = 0;

	if (context->pause_traffic)
	{
		return -1;
	}
	// Receive the data from the server
	ret = recv(context->tcp_sock_fd, type, 1, 0);
	if (ret < 0) {
		LOG_ERR("Failed to receive data from server %d", errno);
		return -errno;
	}
	LOG_INF("Received type: %d\n", *type);
	
	// Check the type of the received data
	switch (*type) {
		case FRAME_TYPE_UNKNOWN:
			LOG_ERR("Unknown type received from the server");
			ret = -4;
			break;

		case FRAME_TYPE_ADD_USER_DATA:
			LOG_INF("Receiving new user data from the server");
			// Receive the length of the data
			ret = recv(context->tcp_sock_fd, buffer_rx, 2, 0);
			if (ret < 0) {
				LOG_ERR("Failed to receive data from server %d", errno);
				return -3;
			}
			rx_length = (uint16_t)buffer_rx[0] << 8 | (uint16_t)buffer_rx[1];
			LOG_INF("Received length: %d\n", rx_length);
			if (rx_length > 0) {
				ret = recv(context->tcp_sock_fd, buffer_rx, rx_length, 0);
				if (ret < 0) {
					LOG_ERR("Failed to receive data from server %d", errno);
					return -3;
				}
				LOG_INF("Receiving add user data from the server : %d\n", ret);
				if (ret > sizeof(uint32_t)) {
					LOG_HEXDUMP_INF(buffer_rx, ret, "Received data");
					UserID_t user_id;
					for (int i = 0; i < (int)(rx_length / sizeof(UserID_t)); i++) {
						bytecpy(&user_id, buffer_rx + i * sizeof(UserID_t), sizeof(UserID_t));
						LOG_INF("User ID: %d", user_id.user_id_number);
						k_msgq_put(&users_add_queue, &user_id, K_NO_WAIT);
					}
					break;
				} else {
					LOG_ERR("Invalid add user data size");
					ret = false;
				}
			} else {
				ret = 0;
			}
			break;
		
		case FRAME_TYPE_REMOVE_USER_DATA:
			LOG_INF("Receiving remove user data from the server");
			// Receive the length of the data
			ret = recv(context->tcp_sock_fd, buffer_rx, 2, 0);
			if (ret < 0) {
				LOG_ERR("Failed to receive data from server %d", errno);
				return -3;
			}
			rx_length = (uint16_t)buffer_rx[0] << 8 | (uint16_t)buffer_rx[1];
			LOG_INF("Received length: %d\n", rx_length);
			if (rx_length > 0){
				ret = recv(context->tcp_sock_fd, buffer_rx, rx_length, 0);
				if (ret < 0) {
					LOG_ERR("Failed to receive data from server %d", errno);
					return -3;
				}
				LOG_INF("Receiving remove user data from the server : %d\n", ret);
				if (ret > sizeof(uint16_t)){
					LOG_HEXDUMP_INF(buffer_rx, ret, "Received data");
					uint16_t user_id;
					for (int i = 0; i < (int)(ret / sizeof(uint16_t)); i++) {
						bytecpy(&user_id, buffer_rx + i * sizeof(uint16_t), sizeof(uint16_t));
						LOG_INF("User ID: %d", user_id);
						k_msgq_put(&users_id_remove_queue, &user_id, K_NO_WAIT);
					}
					break;
				} else{
					LOG_ERR("Invalid remove user data size");
					ret = false;
				}
			} else {
				ret = 0;
			}
			context->data_received = true;
			break;

		case FRAME_TYPE_LOG_DATA:
			LOG_INF("Receiving log data from the server");
			break;

		case FRAME_TYPE_DEVICE_CONFIG:
			LOG_INF("Receiving device configuration from the server");
			ret = recv(context->tcp_sock_fd, buffer_rx, sizeof(DeviceConfig_t), 0);
			if (ret < 0) {
				LOG_ERR("Failed to receive data from server %d", errno);
				return -3;
			}
			if (ret == sizeof(DeviceConfig_t)) {
				if(buffer_rx[2] == dev_conf->device_type) {
					if(buffer_rx[1] != dev_conf->device_id) {
						LOG_WRN("New Device ID received from the server: %d", buffer_rx[1]);
						dev_conf->device_id = buffer_rx[1];
//TODO: flash_write_device_config(device_config);
						return 0;
					}
					if (buffer_rx[4] == dev_conf->device_major_version && buffer_rx[5] == dev_conf->device_minor_version){
						LOG_INF("Device configuration is up to date");
					} else {
						LOG_WRN("Device firmware is outdated");
						LOG_INF("Device major version: %d", dev_conf->device_major_version);
						LOG_INF("Device minor version: %d", dev_conf->device_minor_version);
						return 2;
					}
				}
			} else {
				LOG_ERR("Invalid device configuration size");
				return -2;
			}
			break;

		case FRAME_TYPE_DISCONNECT:
			context->server_connected = false;
			LOG_INF("Receiving disconnect info from the server");
			break;

		case FRAME_TYPE_TIME_SYNC:
			LOG_INF("Receiving time sync response from the server");
			ret = recv(context->tcp_sock_fd, buffer_rx, sizeof(uint32_t), 0);
			if (ret < 0) {
				LOG_ERR("Failed to receive data from server %d", errno);
				return -3;
			}
			if (ret == sizeof(uint32_t)) {
				timestamp = (uint32_t)buffer_rx[0] << 24 | (uint32_t)buffer_rx[1] << 16 | (uint32_t)buffer_rx[2] << 8 | (uint32_t)buffer_rx[3];
				LOG_INF("Received timestamp: %d", timestamp);
				// Set the time offset
				time_offset = timestamp - (uint32_t)(k_uptime_get_32()/1000);
				LOG_INF("Time offset: %d", time_offset);
			} else {
				LOG_ERR("Invalid time sync response size");
				return -2;
			}
			break;

		default:
			LOG_ERR("Invalid type");
			//clean_tcp_buffer(context->tcp_sock_fd);
			buffer_rx[0] = FRAME_TYPE_UNKNOWN;
			send(context->tcp_sock_fd, buffer_rx, 1, 0); // Send unknown type to the server
			return -4;
	}

	return ret;
}

/*! @brief Function to send the configuration information to the server
 * 
 * @param context Pointer to the TCP context
 * @return 0 if the device ID change, 1 if the configuration information is ok, 2 if an firmware update is pending, 3 if traffic pause, negative otherwise
*/
static int send_config_info_to_server(TcpContext_t *context, DeviceConfig_t *dev_conf)
{
	int ret;
	const char msg_len = sizeof(DeviceConfig_t) + 1; // 1 byte for the message type
	char buffer_tx[msg_len]; // 1 byte for the message type
	char type;
	bytecpy(buffer_tx + 1, dev_conf, sizeof(DeviceConfig_t));
	buffer_tx[0] = FRAME_TYPE_DEVICE_CONFIG;
	if (context->pause_traffic)
	{
		return 3;
	}
	ret = send(context->tcp_sock_fd, &buffer_tx, msg_len, 0);
	if (ret < 0) {
		LOG_ERR("Failed to send TCP config info to TCP server %d", errno);
		return -errno;
	}

	ret = receive_data_from_server(context, dev_conf, &type, NULL);
	if (ret < 0 || type != FRAME_TYPE_DEVICE_CONFIG) {
		LOG_ERR("Failed to receive device config from TCP server %d", errno);
		return -1;
	}

	return 1;
}

/*! @brief Function to synchronize the time with the server
 * 
 * @param context Pointer to the TCP context
 * @return 1 on success, 0 if pause traffic, negative value otherwise
 */
int time_sync(TcpContext_t *context)
{
	int ret;
	char buffer_tx[1];
	uint32_t timestamp;
	char buffer_rx[sizeof(timestamp) + 1]; // Buffer to receive the server response
	buffer_tx[0] = FRAME_TYPE_TIME_SYNC;
	if (context->pause_traffic)
	{
		return 0;
	}

	// Send the time sync request to the server
	ret = send(context->tcp_sock_fd, buffer_tx, sizeof(buffer_tx), 0);
	if (ret < 0) {
		LOG_ERR("Failed to send time sync info to the server %d", errno);
		return -errno;
	}
	
	ret = receive_data_from_server(context, &device_config, buffer_rx, NULL);
	if (ret < 0) {
		return ret;
	}
	return 1;
}

/*! @brief Function to setup the TCP socket path
 * 
 * @param context Pointer to the TCP context
 * @return int 1 if the setup is successful, -1 otherwise
*/
static int setup_tcp_sock_path(TcpContext_t *context)
{
	int ret;
	if (context->pause_traffic)
	{
		while(context->pause_traffic)
		{
			k_sleep(K_MSEC(10));
		}
	}

	ret = connect_to_server(context);
	if (ret < 0) {
		return -1;
	}
	context->tcp_sock_fd = ret;
	
	if (context->pause_traffic)
	{
		while(context->pause_traffic)
		{
			k_sleep(K_MSEC(10));
		}
	}

	/* send config info to server */
	ret = send_config_info_to_server(context, &device_config);
	if (ret < 0) {
		return -1;
	}

	return 1;
}

/*! @brief Function to initialize the TCP context
 * 
 * @param context Pointer to the TCP context
 */
void tcp_context_init(TcpContext_t *context)
{
	context->pause_traffic = false;
	context->server_connected = false;
	context->nb_data_to_receive = 1;
	context->data_received = true;
	context->tcp_sock_fd = -1;
	context->buffer_logs_len = sizeof(uint8_t) + sizeof(uint16_t);
	context->buffer_logs[0] = FRAME_TYPE_LOG_DATA;
	context->buffer_logs[1] = 0;
	context->buffer_logs[2] = 0;
	context->buffer_rx_len = 0;
	context->size_off_payload = 0;
	BUILD_ASSERT(strlen(CONFIG_TCP_SERVER_IP) > 0,
				 "The server IPv4 address is not configured, use SERVER_CONFIG_IPV4_ADDR");
	context->server_ip = CONFIG_TCP_SERVER_IP;
	context->port = CONFIG_TCP_SERVER_PORT;
	tcp_context_initialized = true;
}

/*! @brief Function to send logs to the server
 * 
 * @param context Pointer to the TCP context
 * @return int nb_byte receive if the logs are sent successfully, -1 if the traffic is paused, -2 otherwise
 */
int tcp_send_logs(TcpContext_t *context)
{
	int ret;
	if(context->pause_traffic)
	{
		return -1;
	}
	if (context->buffer_logs_len < 3)
	{
		LOG_INF("No logs to send");
		context->buffer_logs_len = 3;
	}
	LOG_HEXDUMP_WRN(context->buffer_logs, context->buffer_logs_len, "Data to send");
	ret = send(context->tcp_sock_fd, context->buffer_logs, context->buffer_logs_len, 0);
	context->data_received = false;
	if (ret < 0)
	{
		LOG_ERR("Failed to send data to TCP server %d", errno);
		return -2;
	}
	LOG_WRN("Data sent to server");
	context->buffer_rx_len = 0;
	char data_type;
	// The user to add and remove data from the server
	ret = receive_data_from_server(context, &device_config, &data_type, NULL);
	if (ret < 0 && ret != -4)
	{
		LOG_ERR("Failed to receive user data from TCP server %d", errno);
		return ret;
	} else if (ret == -4) {
		LOG_INF("Bad data type received from the server");
		return 0;
	}
	ret = receive_data_from_server(context, &device_config, &data_type, NULL);
	if (ret < 0 && ret != -4)
	{
		LOG_ERR("Failed to receive data from TCP server %d", errno);
		return ret;
	} else if (ret == -4) {
		LOG_INF("Bad data type received from the server");
		return 0;
	}
	context->buffer_logs_len = 3;
	context->size_off_payload = 0;
	bytecpy(context->buffer_logs + 1, &context->size_off_payload, sizeof(context->size_off_payload));

	return ret;
}

/*! @brief Function to send data to the server for X seconds
 * 
 * @param context Pointer to the TCP context
 * @param X Duration of the test in seconds
 */
void tcp_send_X_seconds(TcpContext_t *context, int X)
{
	int ret;
	char buffer_tx[30];
	char buffer_rx[1500];
	uint32_t start_time;
	int total_duration = X * 1000; // 30 s in ms
	bytecpy(buffer_tx, "Hello from the client, can you send me somthing funny?", 54);
	start_time = k_uptime_get_32();
	while ((k_uptime_get_32() - start_time) < total_duration)
	{	
		if(context->data_received)
		{
			if(!(context->pause_traffic))
			{
				ret = send(context->tcp_sock_fd, buffer_tx, 21, 0);
				if (ret < 0)
				{
					LOG_ERR("Failed to send data to TCP server %d", errno);
					context->server_connected = false;
					break;
				}
				LOG_WRN("Data sent to server");
				context->data_received = false;
			}
		}
		ret = recv(context->tcp_sock_fd, buffer_rx, sizeof(buffer_rx), MSG_DONTWAIT);
		if (ret < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// No data is available to be read
				continue;
			} else {
				LOG_ERR("Failed to receive data from TCP server %d", errno);
				break;
			}
		} else {
			LOG_WRN("Data received from server %d", ret);
			context->data_received = true;
			continue;
		}
		
		k_sleep(K_USEC(100));
	}
}
	
/*! @brief Function to start the TCP tests
 * 
 * @param context Pointer to the TCP context
 * @param duration Duration of the test in seconds
 */
void begin_tcp_tests(TcpContext_t *context, int duration)
{
	if(tcp_context_initialized == false)
	{
		LOG_ERR("TCP context not initialized, please call tcp_context_init() before calling begin_tcp_tests()");
		return;
	}
	tcp_send_X_seconds(context, duration);
}

/*! @brief Function to initialize the TCP client
 * 
 * @param context Pointer to the TCP context
 * @return bool true if the initialization is successful, false otherwise
 */
bool tcp_client_init(TcpContext_t *context)
{
	if(!tcp_context_initialized)
	{
		tcp_context_init(context);
	}

	if(setup_tcp_sock_path(context) < 0)
	{
		LOG_ERR("Failed to setup TCP socket path");
		return false;
	}
	LOG_DBG("TCP socket path setup successfully");

	if (time_sync(context) < 0)
	{
		LOG_ERR("Failed to synchronize the time with the server");
		return false;
	}
	return true;
}

/*! @brief Function to handle the communication loop
 * 
 * @param context Pointer to the TCP context
 * @return 1 if the communication exit because server disconnected, 2 if it is because update is pending, negative otherwise
 */
int communication_loop(TcpContext_t *context)
{
	LOG_INF("Starting Main communication Loop TCP Client");
	int ret;
	int64_t last_time_sync = 0;
	int64_t last_device_config_sync = 0;
	Log_t log;
	
	if (tcp_context_initialized == false)
	{
		LOG_ERR("TCP context not initialized, please call tcp_client_init() before calling communication_loop()");
		return -1;
	}
	while(1)
	{
		if (!(context->pause_traffic))
		{
			// Send the logs to the server
			ret = tcp_send_logs(context);
			if (ret == -ECONNRESET)
			{
				LOG_ERR("Connection closed by server");
				context->server_connected = false;
				return 1;
			}
			else if (ret == -ETIMEDOUT)
			{
				LOG_INF("Timeout while waiting for data");
				if(context->pause_traffic) {
					LOG_INF("Traffic paused");
					continue;
				}
			}
			else if (ret < 0)
			{
				LOG_ERR("Failed to send logs to the server %d", errno);
				context->server_connected = false;
				return 1;
			}
				
			// Update the device configuration only once per hour
			if ((k_uptime_get() - last_device_config_sync) > 3600000)// 1 hour
			{
				ret = send_config_info_to_server(context, &device_config);
				if (ret < 0)
				{
					LOG_ERR("Failed to send config info to TCP server %d", errno);
					context->server_connected = false;
					return 1;
				}
				if (ret == 2)
				{
					LOG_INF("Firmware update is pending");
					return 2;
				}
				if (ret == 3)
				{
					continue;
				}
				last_device_config_sync = k_uptime_get();
			}
				
			// Update the device time only once per 12 hours
			if ((k_uptime_get() - last_time_sync) > 432000000)// 12 hour
			{
				ret = time_sync(context);
				if (ret < 0)
				{
					LOG_ERR("Failed to synchronize the time with the server %d", errno);
					context->server_connected = false;
					return 1;
				}
				if (ret == 0)
				{
					continue;
				}
				last_time_sync = k_uptime_get();
			}

			k_sleep(K_MSEC(TCP_SLEEP_TIME_MS)); // TCP_SLEEP_TIME_MS to avoid blocking the CPU for too long
#if TWT_USE == false
			context->pause_traffic = true; // Pause the traffic
#endif
		} else {
			// Load the logs from the queue
        	while (k_msgq_get(&logs_send_queue, &log, K_NO_WAIT) == 0)
			{
				LOG_INF("Log received from queue");

				// Check if the log buffer is full
				if ((context->buffer_logs_len + sizeof(log)) >= CONFIG_NET_BUF_DATA_SIZE - 56) // 56 bytes for the header IP, TCP, etc.
				{
					LOG_ERR("Buffer full, sending logs to the server");
					ret = tcp_send_logs(context);
					if (ret < 0)
					{
						LOG_ERR("Failed to send logs to the server %d", errno);
						context->server_connected = false;
						return 1;
					}
				}
				bytecpy(context->buffer_logs + context->buffer_logs_len, &log, sizeof(log));
				context->buffer_logs_len += sizeof(log);
				context->size_off_payload = context->buffer_logs_len - 3;
				bytecpy(context->buffer_logs + 1, &context->size_off_payload, sizeof(context->size_off_payload));
				LOG_INF("Log added to buffer %d and payload %d", context->buffer_logs_len, context->size_off_payload);
			
			}
			k_sleep(K_MSEC(60));
			
#if TWT_USE == false
			context->pause_traffic = false; // Resume the traffic
#endif
		}
		if (!(context->server_connected))
		{
			LOG_ERR("Failed to connect to the server");
			return 1;
		}
		k_sleep(K_USEC(50));
	}
	disconnect_from_server(context);
	return 0;
}