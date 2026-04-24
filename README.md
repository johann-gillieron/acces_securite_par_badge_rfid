# Accès sécurité par badge RFID (FR)
Ceci est mon répertoire de projet de Bachelor : `Électronique compacte pour accès sécurité par badge RFID` fait lors de mes études à la HEIG-VD

Ce système permet de gérer l'accès à un système de manière sécuriser en se basant sur la technologie DESFire EVx, d'être basse consommation avec l'utilisation de la fonctionnalité du Wi-Fi 6 TWT (Target Wake Time).

## Matériel
Pour réaliser ce projet, il faut:
- une carte de développement NRF7002 DK de Nordic Semiconductor;
- une carte de développement avec la puce CLRC663 de NXP (lecteur NFC);
- un écran e-Ink basé sur le contrôleur SSD1680 avec une résolution de 250 par 122 pixels;
- un ordinateur/serveur avec une base de données MariaDB (un Raspberry pi est suffisant pour gérer cette tâche);
- un routeur Wi-Fi compatible avec la norme AX (Wi-Fi 6) et TWT (Target Wake Time) (le TP-Link AX55 par exemple).

## Connexion entre les différents modules et la carte NRF7002 DK
Voici une table avec les différentes interconnexions:
| nRF7002 DK | CLRC663 | e-Ink | IO       | notes    |
| ---------- | ------- | ----- | -------- | -------- |
|P1.05       | CS      |       |          |          |
|P1.11       |         | RST   |          |          |
|P1.12       |         | DC    |          |          |
|P1.13       | MOSI    | MOSI  |          | bus SPI3 |
|P1.14       | MISO    | MISO  |          | bus SPI3 |
|P1.15       | SCK     | SCK   |          | bus SPI3 |
|P1.27       |         | ECS   |          |          |
|P1.02       |         | BUSY  |          |          |
|P1.06       |         |       | LED 1    | commande relais   |
|P1.07       |         |       | LED 2    |          |
|P1.08       |         |       | Bouton 1 |          |
|P1.09       |         |       | Bouton 2 |          |

## Installation
### Pour le NRF7002 DK
Pour compiler ce projet et le flasher sur la carte de développement NRF7002 DK, il faut au préalable avoir installer l'environnement de développement Zephyr RTOS selon la marche à suivre de Nordic Semiconductor [Installing the nRF Connect SDK](https://web.archive.org/web/20240711194954/https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html).

Une fois l'environnement installer, il faut installer le driver et l'api pour le CLRC663 qui se trouve [ici](https://github.com/johann-gillieron/driver_CLRC663_zephyr_rtos).

### Pour le serveur
Cette procédure d'installation est faite pour un Raspberry.
Le système d’exploitation utilisé est `Raspberry Pi OS Lite`, une version allégée de `Raspberry Pi OS` sans interface graphique, ce qui permet de réduire la consommation de ressources et de garantir une meilleure sécurité.

Pour installer cette OS sur une carte SD, il suffit de télécharger l’utilitaire Raspberry Pi Imager et de suivre les instructions pour installer l’OS sur la carte SD.

Après avoir démarré le Raspberry : pour installer les dépendances nécessaires, il suffit d’exécuter les commandes suivantes :
```shell
sudo apt update
sudo apt full-upgrade-y
sudo apt install python3 python3-pip
sudo apt install mariadb-server
sudo apt install libmariadb-dev libmariadb3
pip3 install flask
pip3 install mariadb==1.1.10
pip3 install pyopenssl
```
Pour paramétrer mariadb (la base de données), il suffit de suivre les instructions suivantes :
1) il faut lancer le script de sécurisation de la base de données :
```shell
sudo mysql_secure_installation
```
2) il faut suivre les instructions pour créer un mot de passe pour l’utilisateur `root` (conseil : notez-le quelque part pour ne pas l’oublier);
3) il faut répondre "Y" à toutes les commandes pour sécuriser la base de données supprimer les utilisateurs anonymes, désactiver la connexion root à distance, supprimer la base de données de test et recharger les privilèges);
4)  Maintenant que la base de données est sécurisée, il faut créer une base de données et un utilisateur ("myuser" avec le mot de passe "password" ce ne sont que des exemples) pour le serveur de communication :
```shell
sudo mysql-u root-p
CREATE DATABASE user_access_db;
CREATE USER 'myuser'@'localhost' IDENTIFIED BY 'password';
ALTER USE 'root'@'localhost' IDENTIFIED WITH mysql_native_password USING PASSWORD('password');
GRANT ALL PRIVILEGES ON user_access_db.* TO 'myuser'@'localhost';
FLUSH PRIVILEGES;
USE user_access_db;
CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, user_id CHAR(8) NOT NULL, first_name VARCHAR(50) NOT NULL, last_name VARCHAR(50) NOT NULL, phone VARCHAR(16) NOT NULL, secret VARCHAR(23) NOT NULL, technician BOOLEAN DEFAULT FALSE, machine_1 BOOLEAN DEFAULT FALSE, machine_2 BOOLEAN DEFAULT FALSE);
CREATE DATABASE user_site_web;
GRANT ALL PRIVILEGES ON user_site_web.* TO 'myuser'@'localhost';
FLUSH PRIVILEGES;
USE user_site_web;
CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL);
INSERT INTO users (username, password) VALUES ('user1', 'password1');
```
Maintenant, il y a donc deux bases de données qui ont été créées,  "user_access_db" pour stocker les utilisateurs avec les accès au système et une base de données "user_site_web" pour stocker les utilisateurs du site web.

Quand les bases de données ont été créées, il faut télécharger la partie serveur du projet sur le serveur où les bases de données ont été créée.


## Paramètres
### Pour la partie serveur
Il faut paramétrer les identifiants pour accéder aux bases de données du serveur dans les scripts *Serveur_connecteur_DB_Client.py* (*config_mariadb* c'est pour les accès au machine/système) et *siteweb/server_site.py* (au début du script *config* c'est pour les accès au site et *config2* c'est pour les accès au machine/système).

### Pour la partie client (le NRF7002 DK)
Une fois le SDK de Nordic Semiconducteur installé ainsi que le driver du CLR663, il faut changer quelques paramètres pour accéder au routeur ainsi qu'au serveur dans le fichier *prj.conf* dans le dossier *programme_principal*:

Pour configurer la connexion au routeur et le type de sécurité:
```c
CONFIG_WIFI_KEY_MGMT_WPA2=y
#CONFIG_WIFI_KEY_MGMT_WPA2_256=y
#CONFIG_WIFI_KEY_MGMT_WPA3=y
CONFIG_WIFI_SSID="Le SSID du routeur"
CONFIG_WIFI_PASSWORD="Le mot de passe du routeur"
```
Pour configurer la connexion au serveur:
```c
CONFIG_TCP_SERVER_IP="L'adresse IPv4 du serveur"
CONFIG_TCP_SERVER_PORT=Numéro du port du serveur
```
Pour le type d'appareil (pas encore utilisé mais prévu pour une future version):
```c
CONFIG_DEVICE_TYPE_DEVKIT=y
#CONFIG_DEVICE_TYPE_READER_HW_V1=y
#CONFIG_DEVICE_TYPE_READER_HW_V2=y
#CONFIG_DEVICE_TYPE_CUSTOM=y
```
Pour configurer le timing du TWT (Attention seulement pour les utilisateurs sachant ce qu'ils font sinon le système pourrait ne plus fonctionner correctement):
```c
# Time when the wifi is awake [us]
CONFIG_TWT_WAKE_INTERVAL=65000
# Time for one TWT cycle (Awake + sleep time) [us]
CONFIG_TWT_INTERVAL=10000000
```

## Utilisation
### Côté serveur
Pour lancer ces scripts sans avoir de retour des journaux, il suffit d'entrer les commandes suivantes:
```shell
python siteweb/server_site.py &
python Serveur_connecteur_DB_Client.py &
```
Sinon pour afficher des logs, il suffit de faire les mêmes commandes mais sans les *&* à la fin de ligne.
Pour accéder à l'interface web, il suffit d'entrer l'adresse IPv4 dans un navigateur pour accéder à la page de connexion. (Les identifiants ont été créer avec la base de données).

Pour ajouter un utilisateur pour une machine, il faut cliquer sur *User Management Access* puis renseigner les champs; pour obtenir le secret DESFire en hexadécimal, il faut utiliser le programme *DESFire_init_and_test* (pour le client) dans le dossier *programme_extra*.

### Côté client
Il suffit maintenant que tout est installé et configuré, de compiler le programme principal et de le flasher sur la carte de développement NRF7002 DK.

# Secure access by RFID tag (EN)

This is my Bachelor's project repository: `Compact electronics for RFID badge access control`, developed during my studies at HEIG-VD.

This system manages access to a system securely using DESFire EVx technology and is low power with the use of Wi-Fi 6 TWT (Target Wake Time) functionality.

## Material
To make this project, we need:
- a development kit NRF7002 DK from Nordic Semiconductor;
- a development kit with the chip CLRC663 from NXP;
- a e-Ink screen with the controller SSD1680 and a resolution of 250 per 122 pixels;
- a compute/server with a MariaDB data base (a Raspberry pi is sufficient for this task);
- a Wi-Fi router compatible the AX standard (Wi-Fi 6) and compatible with TWT (Target Wake Time) (for example: TP-Link AX55).

## Connection between the different modules and the NRF7002 DK board
Here is the table with the different interconnections:
| nRF7002 DK | CLRC663 | e-Ink | IO       | notes    |
| ---------- | ------- | ----- | -------- | -------- |
|P1.05       | CS      |       |          |          |
|P1.11       |         | RST   |          |          |
|P1.12       |         | DC    |          |          |
|P1.13       | MOSI    | MOSI  |          | bus SPI3 |
|P1.14       | MISO    | MISO  |          | bus SPI3 |
|P1.15       | SCK     | SCK   |          | bus SPI3 |
|P1.27       |         | ECS   |          |          |
|P1.02       |         | BUSY  |          |          |
|P1.06       |         |       | LED 1    | relay command |
|P1.07       |         |       | LED 2    |          |
|P1.08       |         |       | Button 1 |          |
|P1.09       |         |       | Button 2 |          |

## Installations
### Development kit NRF7002 DK
To compile and flash the project onto the development kit NRF7002 DK, you must first have installed the Zephy RTOS development environment according to the procedure from Nordic Semiconductor [Installing the nRF Connect SDK](https://web.archive.org/web/20240711194954/https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html).

Once the environment is installed, you need to install the driver and API for the CLRC663 which can be found [here](https://github.com/johann-gillieron/driver_CLRC663_zephyr_rtos).

### Server side
This install procedure is made for a Raspberry Pi.
The operating system used is `Raspberry Pi OS Lite`, a lightweight version of the `Raspberry Pi OS` without a graphical interface, which reduce resource consumption and ensure better security.

To install this OS, you will need a micro-SD card. Simply download the Raspberry Pi Imager utility and follow the instructions to install the OS onto the SD card.

After starting the Raspberry Pi, to install the necessary dependencies, simply run the following commands:
```shell
sudo apt update
sudo apt full-upgrade-y
sudo apt install python3 python3-pip
sudo apt install mariadb-server
sudo apt install libmariadb-dev libmariadb3
pip3 install flask
pip3 install mariadb==1.1.10
pip3 install pyopenssl
```
To setup mariadb (the database), you need to follow the instructions:
1) Launch the script to secure the database:
```shell
sudo mysql_secure_installation
```
2) Follow the instructions to create a password for the user `root` (Tip: write it down somewhere so you don't forget it);
3) Answer `Y` for all commands to secure the database (delete the anonymous users, disable the distant root connection, delete the test database and reload the privileges);
4)  Now the database is securely setup, you need to create databases and a user ("myuser" with the password "password" these are just examples) for the communication server:
```shell
sudo mysql-u root-p
CREATE DATABASE user_access_db;
CREATE USER 'myuser'@'localhost' IDENTIFIED BY 'password';
ALTER USE 'root'@'localhost' IDENTIFIED WITH mysql_native_password USING PASSWORD('password');
GRANT ALL PRIVILEGES ON user_access_db.* TO 'myuser'@'localhost';
FLUSH PRIVILEGES;
USE user_access_db;
CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, user_id CHAR(8) NOT NULL, first_name VARCHAR(50) NOT NULL, last_name VARCHAR(50) NOT NULL, phone VARCHAR(16) NOT NULL, secret VARCHAR(23) NOT NULL, technician BOOLEAN DEFAULT FALSE, machine_1 BOOLEAN DEFAULT FALSE, machine_2 BOOLEAN DEFAULT FALSE);
CREATE DATABASE user_site_web;
GRANT ALL PRIVILEGES ON user_site_web.* TO 'myuser'@'localhost';
FLUSH PRIVILEGES;
USE user_site_web;
CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL);
INSERT INTO users (username, password) VALUES ('user1', 'password1');
```

So now there are two databases that have been created, "user_access_db" to store users with access to the system and a database "user_site_web" to store website users access.

Once the databases have been created, the server-side portion of the project must be uploaded to the server where the databases were created.

## Settings
### For the server side
You must setup the credentials to access to server's database in the scripts *Serveur_connecteur_DB_Client.py* (*config_mariadb* is for the machine/system access) and *siteweb/server_site.py* (at the beginning of the script *config* is for the web site access and *config2* is for the machine/system access).

### For the client side (the NRF7002 DK)
Once the SDK of Nordic Semiconductor and the CLRC663 driver are installed, you need to change some parameters to access the router and the server in the *prj.conf* file in the *programme_principal* folder.

To configure the router connection and security type:
```c
CONFIG_WIFI_KEY_MGMT_WPA2=y
#CONFIG_WIFI_KEY_MGMT_WPA2_256=y
#CONFIG_WIFI_KEY_MGMT_WPA3=y
CONFIG_WIFI_SSID="Le SSID du routeur"
CONFIG_WIFI_PASSWORD="Le mot de passe du routeur"
```
To configure the connection to the server:
```c
CONFIG_TCP_SERVER_IP="L'adresse IPv4 du serveur"
CONFIG_TCP_SERVER_PORT=Numéro du port du serveur
```
For the device type (not yet used but planned for a future version):
```c
CONFIG_DEVICE_TYPE_DEVKIT=y
#CONFIG_DEVICE_TYPE_READER_HW_V1=y
#CONFIG_DEVICE_TYPE_READER_HW_V2=y
#CONFIG_DEVICE_TYPE_CUSTOM=y
```
To configure TWT timing (caution: only for users who know what they are doing, otherwise the system may no longer function correctly):
```c
# Time when the wifi is awake [us]
CONFIG_TWT_WAKE_INTERVAL=65000
# Time for one TWT cycle (Awake + sleep time) [us]
CONFIG_TWT_INTERVAL=10000000
```

## Usage
### Server side
To run these scripts without receiving logs output, simply enter the following commands:
```shell
python siteweb/server_site.py &
python Serveur_connecteur_DB_Client.py &
```
Alternatively, to display logs, simply use the same commands but without the *&* at the end of the line.
To access to the web interface, simply enter the IPv4 address in a browser to access the login page. (The credentials were created using the database).

To add a user for a machine, simply click on *User Management Access* and then fill in the fields; to obtain the DESFire secret in hexadecimal, you must use the *DESFire_init_and_test* program (for the client) in the *programme_extra* folder.

### Client side
Now that everything is installed and configured, all that remains is to compile the main program and flash it onto the NRF7002 DK development board.
