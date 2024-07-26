# Project: Serveur pour acces sécurité par badge RFID
# Description: Serveur TCP pour la gestion des utilisateurs et des logs
# Author: Johann Gilliéron
# Date: 2024.07.20
import socket, os
import threading
import datetime, time
import struct
import csv
import mariadb

# define the global constants
DEBUG_LVL0 = False # Debug level 0: critical information
DEBUG_LVL1 = True # Debug level 1: information
DEBUG_LVL2 = True # Debug level 2: debug information
TCP_PORT = 3000
TCP_IP_ADDRESS = "0.0.0.0"  # Listen on all available network interfaces
start_text = b"I'm the Server and the first mater. Hello there!"
tech_test= "Dans le cadre de mon travail de Bachelor à la Haute École d'Ingénierie et de Gestion du canton de Vaud (HEIG-VD), j'ai eu l'opportunité de travailler sur un projet passionnant lié à l'Internet des Objets (IoT) et plus particulièrement à la technologie Bluetooth Low Energy (BLE). Mon objectif était de développer une application mobile capable de communiquer avec des capteurs BLE afin de collecter des données en temps réel et de les afficher sur une interface graphique conviviale. Pour cela, j'ai utilisé le langage de programmation C# avec l'environnement de développement intégré (IDE) Visual Studio Code (VS Code) et la bibliothèque open-source LVGL (Light and Versatile Graphics Library) pour la création de l'interface graphique. J'ai également utilisé le protocole de communication série Universal Asynchronous Receiver-Transmitter (UART) pour communiquer avec les capteurs BLE. Après plusieurs semaines de développement et de tests, j'ai finalement réussi à créer une application mobile fonctionnelle et efficace, capable de collecter et d'afficher les données des capteurs BLE en temps réel. Cette expérience m'a permis d'acquérir de nouvelles compétences en programmation et en développement d'applications mobiles, ainsi que de renforcer mes connaissances en matière d'IoT et de BLE.".encode('utf-8')
BYTE_MTU_MAX = 1500
TIMEOUT = 60 # 60 seconds
BYTES_FOR_DEVICE_INFO = 7
BYTES_PER_LOGS = 8
BYTES_PER_USER_DATA = 64
BYTES_PER_USER_ID = 2
MAX_NEW_USERS_PER_PACKET = 22 # 22*64 = 1408
MAX_REMOVED_USERS_PER_PACKET = 704 # 704*2 = 1408
USER_NAME_MAX_SIZE = 36
PHONE_NUMBER_MAX_SIZE = 16
BYTES_FOR_DESFIRE_ID = 8
header_logs_csv = ["Time", "UserID", "Type"]

config_mariadb = {
    'user': 'myuser',
    'password': 'password',
    'host': '127.0.0.1',
    'database': 'user_access_db'
}

# Define the user data to send to the device
New_Users = [[], [], [], [], [], [], [], [], [], [], [], [], [], [], [], []] # 3D array
Removed_User = [[], [], [], [], [], [], [], [], [], [], [], [], [], [], [], []] # 3D array


# Create the TCP server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
server_address = (TCP_IP_ADDRESS, TCP_PORT)
server_socket.bind(server_address)
print("I'm the TCP Server for the Bachelor project: security by RFID badge")
print(f"TCP server is listening on {server_address[0]}:{server_address[1]}...")
server_socket.listen(1024)

# Define the enums
class DeviceTypeEnum:
    DEVICE_TYPE_UNKNOWN = 0
    DEVICE_TYPE_DEVKIT = 1
    DEVICE_TYPE_READER_HW_V1 = 2
    DEVICE_TYPE_READER_HW_V2 = 3

class LogActionEnum:
    LOG_ACTION_UNKNOWN = 0
    LOG_ACTION_ACTIVATE = 1
    LOG_ACTION_DISABLE = 2
    LOG_ACTION_LOGIN = 3
    LOG_ACTION_LOGOUT = 4
    LOG_ACTION_LOCK = 5
    LOG_ACTION_UNLOCK = 6

class DeviceStatusEnum:
    DEVICE_OK = 0
    DEVICE_UPDATE_DOWNLOADED = 1
    DEVICE_UPDATED = 2
    DEVICE_ERROR = 3

class TCP_DATA_TYPE:
	UNKNOWN = 0x00
	LOG_DATA = 0x01
	ADD_USER_DATA = 0x02
	REMOVE_USER_DATA = 0x03
	UPDATE_REQUEST = 0x04
	TIME_SYNC = 0x05
	DEVICE_CONFIG = 0x06
	COM_DISCONNECTION = 0x07


def connect_to_db():
    return mariadb.connect(**config_mariadb)

def get_users_by_machine(cursor):
    query = """
    SELECT id, user_id, first_name, last_name, phone, secret, technician, machine_1, machine_2
    FROM users
    """
    cursor.execute(query)
    users = cursor.fetchall()
    return users


# function et class definitions
# Define the function for the user data to send to the device (user id, user name, phone number, desfire id) in one array
# function et class definitions
# Define the function for the user data to send to the device (user id, user name, phone number, desfire id) in one array
def UserData(user_id, user_first_name, user_name, phone_number, desfire_id, user_type=0):
        local_user_id = struct.pack('<H', user_id) # 2 octets little endian
        user_name_size = len(user_first_name) + len(user_name) + 1
        user_complet_name = user_first_name + " " + user_name
        user_type_byte = struct.pack('>B', user_type) # 1 octet
        local_user_name_size = struct.pack('>B', min(user_name_size, USER_NAME_MAX_SIZE)) # 1 octet 
        local_user_name = user_complet_name[0:USER_NAME_MAX_SIZE]
        real_user_name_size = len(local_user_name.encode('utf-8'))
        if real_user_name_size < USER_NAME_MAX_SIZE:
            local_user_name += ' ' * (USER_NAME_MAX_SIZE - real_user_name_size)
        local_phone_number = phone_number[0:PHONE_NUMBER_MAX_SIZE]
        real_phone_number_size = len(local_phone_number.encode('utf-8'))
        if real_phone_number_size < PHONE_NUMBER_MAX_SIZE:
            local_phone_number += ' ' * (PHONE_NUMBER_MAX_SIZE - real_phone_number_size)
        local_desfire_id = desfire_id[0:BYTES_FOR_DESFIRE_ID]
        byte_array = bytearray()
        byte_array[len(byte_array):] = local_user_id
        byte_array[len(byte_array):] = local_user_name_size
        byte_array[len(byte_array):] = local_user_name.encode('utf-8')
        byte_array[len(byte_array):] = user_type_byte
        byte_array[len(byte_array):] = local_phone_number.encode('utf-8')
        byte_array[len(byte_array):] = local_desfire_id
        return byte_array


def DataBaseUserManagement():
    db = connect_to_db()
    cursor = db.cursor()
    users = get_users_by_machine(cursor)
    for user in users:
        user_id = struct.unpack('<H', user[1][0:2].encode('utf-8'))[0]
        defire_str = user[5].replace(":", "")
        defire_id = bytes.fromhex(defire_str)
        type(user_id)
        if user[7]:  # machine_1 column
            print(f"User ID: {user[1]}, User ID: {bytearray((user[1][0:2]).encode('utf-8'))}, Name: {user[2]} {user[3]}, Phone: {user[4]}, Desfire ID: {user[5]}")
            New_Users[0].append(UserData(user_id, user[2], user[3], user[4], defire_id, user[6]))
        if user[8]:  # machine_2 column
            New_Users[1].append(UserData(user_id, user[2], user[3], user[4], defire_id, user[6]))
            
    cursor.close()
    db.close()

# Define a function to compute the logs receive from the device out : (user id, date, time, action)
def logs_buffer_compute(buffer_logs, logs_size, device_id):
    filename = "logs_device_" + str(device_id) + ".csv"
    logs = []
    nb_logs = int(logs_size / BYTES_PER_LOGS)
    for i in range(nb_logs):
        offset = i * BYTES_PER_LOGS
        time = struct.unpack('>I', buffer_logs[offset:offset+4])
        user_id = struct.unpack('>H', buffer_logs[offset+4:offset+2])
        match buffer_logs[offset+6]:
            case LogActionEnum.LOG_ACTION_ACTIVATE:
                action = "Activate"
            case LogActionEnum.LOG_ACTION_DISABLE:
                action = "Disable"
            case LogActionEnum.LOG_ACTION_LOGIN:
                action = "Login"
            case LogActionEnum.LOG_ACTION_LOGOUT:
                action = "Logout"
            case LogActionEnum.LOG_ACTION_LOCK:
                action = "Lock"
            case LogActionEnum.LOG_ACTION_UNLOCK:
                action = "Unlock"
            case _:
                action = "Unknown"
        logs.append((user_id, time, action))
    # écrire les logs dans un fichier csv
    file_exists = False
    try:
        with open(filename, "r") as f:
            reader = csv.reader(f)
            # vérifier si le fichier a un en-tête
            if next(reader) == header_logs_csv:
                file_exists = True
    except FileNotFoundError:
        pass

    # ouvrir le fichier en mode écriture
    with open(filename, "a", newline="") as f:
        writer = csv.writer(f)
        # si le fichier n'existe pas, ajouter l'en-tête
        if not file_exists:
            writer.writerow(header_logs_csv)
        # ajouter les données
        for row in logs:
            writer.writerow(row)
    return logs


def print_debug_log(text):
    now = datetime.datetime.now()
    now_str = now.strftime("%H:%M:%S")
    print(f"LOG {now_str}: {text}")
    

def process_client_info(buf, adress):
    if len(buf) == BYTES_FOR_DEVICE_INFO :
        device_id = buf[0]
        device_type = buf[1]
        device_status = buf[2]
        device_major_version = buf[3]
        device_minor_version = buf[4]
        if DEBUG_LVL2:
            print(f"Device ID: {device_id}")
            print(f"Device type: {device_type}")
            print(f"Device status: {device_status}")
            print(f"Device major version: {device_major_version}")
            print(f"Device minor version: {device_minor_version}")
        return buf
        '''
        # Check if the device has an update to download
        device_major_version = device_major_version_database[device_id]
        device_minor_version = device_minor_version_database[device_id]
        #check if the device has the good ID

            '''
    
    elif len(buf) < BYTES_FOR_DEVICE_INFO :
        print(f"Error: Not enough data")
        return -1
    
    else:
        print(f"Error: Too much data")
        return -2


def TCP_send_function(client_connection, data_type, data=b'\x00'):
    match data_type:
        case TCP_DATA_TYPE.UNKNOWN:
            data_tx = bytearray()
            data_tx.append(data_type)

        case TCP_DATA_TYPE.DEVICE_CONFIG:
            data_tx = bytearray()
            data_tx.append(data_type)
            data_tx[len(data_tx):] = data

        case TCP_DATA_TYPE.ADD_USER_DATA:
            NB_data = struct.pack('>H', BYTES_PER_USER_DATA*len(data)) # 2 octets big endian
            data_tx = bytearray()
            data_tx.append(TCP_DATA_TYPE.ADD_USER_DATA)
            data_tx[len(data_tx):] = NB_data
            for user in data:
                data_tx[len(data_tx):] = user

        case TCP_DATA_TYPE.REMOVE_USER_DATA:   
            NB_data = struct.pack('>H', BYTES_PER_USER_ID*len(data)) # 2 octets big endian
            data_tx = bytearray()
            data_tx.append(TCP_DATA_TYPE.REMOVE_USER_DATA)
            data_tx[len(data_tx):] = NB_data
            for userID in data:
                data_tx[len(data_tx):] = userID

        case TCP_DATA_TYPE.TIME_SYNC:
            data_tx = bytearray()
            data_tx.append(TCP_DATA_TYPE.TIME_SYNC)
            data_tx[len(data_tx):] = data

        case TCP_DATA_TYPE.COM_DISCONNECTION:
            data_tx = bytearray()
            data_tx.append(TCP_DATA_TYPE.COM_DISCONNECTION)

        case _:
            print_debug_log("Error: Unknown data type to send")
            return -1
            
    if DEBUG_LVL0:
        print_debug_log(f"Data to send: {data_tx}")
    try :
        client_connection.sendall(data_tx)
    except:
        return -1
    return 0


def TCP_receive_function(client_connection, User_to_add = [], User_to_remove = [], device_id=0, client_address=""):
    data_option = 0
    try:
        type_data = client_connection.recv(3)
    except socket.timeout:
        print_debug_log("Error: Timeout - Retry to receive data")  
        try:
            type_data = client_connection.recv(3)
        except:
            print_debug_log("Error: - Communication disconnection")
            return [-1, data_option]
    if DEBUG_LVL0:
        print_debug_log(f"Received data type: {type_data[0]}")
    if not type_data:
        return [-1, data_option]
    
    match type_data[0]:
        case TCP_DATA_TYPE.UNKNOWN:
            print_debug_log("Unknown data received")
            data_option = -1
            return_value = TCP_DATA_TYPE.UNKNOWN

        case TCP_DATA_TYPE.LOG_DATA:
            if len(type_data) == 3:
               buf = type_data[1:3] 
            else:
                buf = client_connection.recv(2)
            buf = buf.replace(b' ', b'')
            print_debug_log(f"Nb bytes log: {buf}")
            try:
                nb_bytes_log = struct.unpack('<H', buf)
            except:
                print_debug_log("Error: Cannot unpack the data")
                TCP_send_function(client_connection, TCP_DATA_TYPE.UNKNOWN)
                return [-1, data_option]
            if DEBUG_LVL0:
                print_debug_log(f"Nb bytes log: {nb_bytes_log[0]}")
            if nb_bytes_log[0] > 0:
                buf = client_connection.recv(nb_bytes_log[0])
                if not buf:
                    TCP_send_function(client_connection, TCP_DATA_TYPE.UNKNOWN)
                    return [TCP_DATA_TYPE.UNKNOWN, -1]

            TCP_send_function(client_connection, TCP_DATA_TYPE.REMOVE_USER_DATA, User_to_remove)
            TCP_send_function(client_connection, TCP_DATA_TYPE.ADD_USER_DATA, User_to_add)

            buf = buf.replace(b' ', b'')

            if nb_bytes_log[0] > 0:
                data = logs_buffer_compute(buf, len(buf), device_id)
                if DEBUG_LVL0:
                    print_debug_log("Log data received")
                    print_debug_log(f"Logs: {data}")
                    data_option = len(data)
                if(len(buf) != nb_bytes_log[0]):
                    TCP_send_function(client_connection, TCP_DATA_TYPE.UNKNOWN)
                    if DEBUG_LVL0:
                        print("Error: Logs bytes numbers not correct ", len(buf))
                        print("Nb Logs: ", nb_bytes_log[0])
            return_value = TCP_DATA_TYPE.LOG_DATA

        case TCP_DATA_TYPE.UPDATE_REQUEST:
            #print_debug_log("Update request received")
            print_debug_log("Not implemented yet")
            return_value = TCP_DATA_TYPE.UPDATE_REQUEST

        case TCP_DATA_TYPE.TIME_SYNC:
            now = int(time.time())
            if DEBUG_LVL1:
                print_debug_log("Time sync received")
                print (f"Time: {now}")
            data = struct.pack('!I', now) # 4 octets
            TCP_send_function(client_connection, TCP_DATA_TYPE.TIME_SYNC, data)
            return_value = TCP_DATA_TYPE.TIME_SYNC

        case TCP_DATA_TYPE.DEVICE_CONFIG:
            if len(type_data) == 3:
                type_data[1:3]
                buf_1 = client_connection.recv(BYTES_FOR_DEVICE_INFO - 2)
                buf = type_data[1:3] + buf_1
            else:
                buf = client_connection.recv(BYTES_FOR_DEVICE_INFO)
            if DEBUG_LVL1:
                print_debug_log("Device config received")
            if not buf:
                return -1
            buf = buf.replace(b' ', b'')
            dev_conf = process_client_info(buf, client_address)
            if isinstance(dev_conf, bytes):
                data_option = dev_conf[0] # device ID
                TCP_send_function(client_connection, TCP_DATA_TYPE.DEVICE_CONFIG, dev_conf)
                return_value =  TCP_DATA_TYPE.DEVICE_CONFIG
            else:
                TCP_send_function(client_connection, TCP_DATA_TYPE.UNKNOWN)
                return_value = -1

        case TCP_DATA_TYPE.COM_DISCONNECTION:
            print_debug_log("Communication disconnection received")
            return_value = TCP_DATA_TYPE.COM_DISCONNECTION

        case _:
            print_debug_log("Unknown data received")
            return_value = -1

    return [return_value, data_option]


def handle_client(client_connection, client_address):
    to_add_Users = [] # 2D array local
    to_remove_Users = [] # 2D array local
    deviceID = 0

    client_connection.settimeout(TIMEOUT)
    print_debug_log(f"Client connected from: {client_address}")
    ret = TCP_receive_function(client_connection, to_add_Users, to_remove_Users, client_address=client_address)
    if ret[0] == TCP_DATA_TYPE.DEVICE_CONFIG:
        print_debug_log("Device config processed")
        deviceID = ret[1]
    else :
        print_debug_log("Error: Device config not processed")
        client_connection.close()
        return
    
    # When we have the device info, we can prepare the user data to add and remove
    to_add_Users = New_Users[deviceID][0:MAX_NEW_USERS_PER_PACKET]
    to_remove_Users = Removed_User[deviceID][0:MAX_REMOVED_USERS_PER_PACKET]
    if DEBUG_LVL2:
        print_debug_log(f"User data prepared {to_add_Users}")
        print_debug_log(f"User data prepared {to_remove_Users}")

    # after processing the device info, we can wait for 
    # the next data : the devie will send the logs and 
    # the server will send the identifiant ta add and remove to the device

    while True:
        if DEBUG_LVL0:
            ret = TCP_receive_function(client_connection, to_add_Users, to_remove_Users, deviceID, client_address=client_address)
            if ret[0] == -1 or ret[0] == TCP_DATA_TYPE.COM_DISCONNECTION:
                print_debug_log("Error or communication disconnection")
                break

            elif ret[0] == TCP_DATA_TYPE.LOG_DATA:
                if DEBUG_LVL2:
                    print_debug_log("Log data processed")
                # Save the previous data and update the data to send
                previous_Removed_Users = Removed_User[deviceID]
                del Removed_User[deviceID][0:len(to_remove_Users)]
                to_remove_Users = Removed_User[deviceID][0:MAX_REMOVED_USERS_PER_PACKET]
                previous_New_Users = New_Users[deviceID]
                del New_Users[deviceID][0:len(to_add_Users)]
                to_add_Users = New_Users[deviceID][0:MAX_NEW_USERS_PER_PACKET]

            elif ret[0] == TCP_DATA_TYPE.UNKNOWN:
                print_debug_log("Unknown data received from the device : so it's an error")
                #Restore the previous data
                Removed_User[deviceID] = previous_Removed_Users
                New_Users[deviceID] = previous_New_Users
        else:
            try:
                ret = TCP_receive_function(client_connection, to_add_Users, to_remove_Users, deviceID, client_address=client_address)
                if ret[0] == -1 or ret[0] == TCP_DATA_TYPE.COM_DISCONNECTION:
                    print_debug_log("Error or communication disconnection")
                    break

                elif ret[0] == TCP_DATA_TYPE.LOG_DATA:
                    if DEBUG_LVL2:
                        print_debug_log("Log data processed")
                    # Save the previous data and update the data to send
                    previous_Removed_Users = Removed_User[deviceID]
                    del Removed_User[deviceID][0:len(to_remove_Users)]
                    to_remove_Users = Removed_User[deviceID][0:MAX_REMOVED_USERS_PER_PACKET]
                    previous_New_Users = New_Users[deviceID]
                    del New_Users[deviceID][0:len(to_add_Users)]
                    to_add_Users = New_Users[deviceID][0:MAX_NEW_USERS_PER_PACKET]

                elif ret[0] == TCP_DATA_TYPE.UNKNOWN:
                    print_debug_log("Unknown data received from the device : so it's an error")
                    #Restore the previous data
                    Removed_User[deviceID] = previous_Removed_Users
                    New_Users[deviceID] = previous_New_Users

            except:
                print_debug_log("Error while receiving data")
                TCP_send_function(client_connection, TCP_DATA_TYPE.COM_DISCONNECTION)
                break

    print_debug_log("Client disconnected")
    client_connection.close()


def main():
    try:
        DataBaseUserManagement()
        while True:
            client_connection, client_address = server_socket.accept()
            thread = threading.Thread(target=handle_client, args=(client_connection, client_address))
            thread.start()
            print(f"Thread actif: {threading.active_count() - 1}")
        
    except KeyboardInterrupt:
        print("Server stopped")
        server_socket.close()
        exit(0)

if __name__ == "__main__":
    main()