# acces_securite_par_badge_rfid
Ceci est mon répertoire de projet de Bachelor : Électronique compacte pour accès sécurité par badge RFID fait lors de mes études à la HEIG-VD

Ce système permet de gérer l'accès à un système de manière sécuriser en se basant sur la technologie DESFire EVx, d'être basse consommation avec l'utilisation de la fonctionnalité du Wi-Fi 6 TWT.

## Liste du matériel
Pour réaliser ce projet, il faut:
- une carte de développement NRF7002 DK de Nordic Semiconductor;
- une carte de développement avec la puce clrc663 (lecteur NFC);
- un écran e-Ink basé sur le contrôleur SSD1680 avec une résolution de 250 par 122 pixels;
- un ordinateur/serveur avec une base de donnée MariaDB (un Raspberry pi est amplement suffisant pour gérer cette tâche);
- un routeur Wi-Fi compatible norme AX (Wi-Fi 6) et aussi compatible TWT (Target Wake Time) (le TP-Link AX55 par exemple).

## Connexion entre les différents modules et la carte NRF7002 DK
Voici la table avec les différentes inter-connexions:
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
Pour pouvoir compliler ce projet et le flasher sur la carte de développement NR7002 DK, il faut au préalable avoir installer l'environnement de développement Zephyr RTOS selon la marche à suivre de Nordic Semiconductor [Installing the nRF Connect SDK](https://web.archive.org/web/20240711194954/https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html).

Une fois l'environnement installer, il faut installer le driver et l'api pour le CLRC663 qui ce trouve dans [ici](https://github.com/johann-gillieron/driver_clrc663_zephyr_rtos).

### Pour le serveur
Cette procédure d'installation est faite pour une Raspberry.
Le système d’exploitation utilisé est Raspberry Pi OS Lite, une version allégée de
Raspberry Pi OS sans interface graphique, ce qui permet de réduire la consommation
de ressources et de garantir une meilleure sécurité.

Pour installer cette OS sur une carte SD, il suffit de télécharger l’utilitaire Raspberry Pi Imager et de suivre les instructions pour installer l’OS sur la carte SD.

Après avoir démarré le Raspberry : pour installer les dépendances nécessaires, il
suffit d’exécuter les commandes suivantes :
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
Pour paramétrer mariadb (la base de données), il suffit de suivre les instructions
suivantes :
1) il faut lancer le script de sécurisation de la base de
données :
```shell
sudo mysql_secure_installation
```
2) il faut suivre les instructions pour créer un mot de passe pour l’utilisateur
root (conseil : notez-le quelque part pour ne pas l’oublier);
3) il faut répondre "Y" à toutes les commandes pour sécuriser la base de données (supprimer les utilisateurs anonymes, désactiver la connexion root à distance, supprimer la base de donnée de test et recharger les privilèges);
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
Il y a donc deux bases de données qui ont été créées,  "user_access_db" pour stocker les utilisateurs avec les accès au système et une base de données "user_site_web" pour
stocker les utilisateurs du site web.

Quand les bases de données ont été créées, il faut télécharger la partie contenue dans le dossier serveur sur le serveur ou les bases de données ont été créée.


## Paramètres
### Pour la partie serveur
Il faut paramétrer les identifiants pour accéder aux bases de données du serveur dans les scripts *Serveur_connecteur_DB_Client.py* (*config_mariadb* c'est pour les accès au machine/système) et *siteweb/server_site.py* (au début du script *config* c'est pour les accès au site et *config2* c'est pour les accès au machine/système).

### Pour la partie client (le NRF7002 DK)
Une fois le SDK de Nordic Semiconducteur installé ainsi que le driver du CLR663, il faut changer quelques paramètres pour accéder au routeur ainsi que au serveur dans le fichier *prj.conf* dans le dossier *programme_principal*:

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
Pour le type d'appareil (pas encore utiliser mais prévu pour une futur version):
```c
CONFIG_DEVICE_TYPE_DEVKIT=y
#CONFIG_DEVICE_TYPE_READER_HW_V1=y
#CONFIG_DEVICE_TYPE_READER_HW_V2=y
#CONFIG_DEVICE_TYPE_CUSTOM=y
```
Pour configurer le timing du TWT (Attention seulement pour les utilisateurs sachant ce qu'ils font sinon le système pourrais ne plus fonctionner correctement):
```c
# Time when the wifi is awake [us]
CONFIG_TWT_WAKE_INTERVAL=65000
# Time for one TWT cycle (Awake + sleep time) [us]
CONFIG_TWT_INTERVAL=10000000
```

## Utilisation
### Côté serveur
Pour lancer ces scripts sans avoir de retour des logs, il suffit d'entrer les commandes suivantes:
```shell
python siteweb/server_site.py &
python Serveur_connecteur_DB_Client.py &
```
Sinon pour afficher des logs, il suffit de faire les mêmes commandes mais sans les *&* à la fin de ligne.
Pour accéder à l'interface web, il suffit d'entrer l'adresse IPv4 dans un navigateur pour accéder à la page de connexion. (Les identifiants ont été créer avec la base de données).

Pour ajouter un utilisateur pour une machine, il faut cliquer sur *User Management Access* puis renseigner les champs, pour obtenir le secret DESFire en hexadécimal, il faut utiliser le programme *DESFire_init_and_test* dans le dossier *programme_extra*.

### Côté client
Il suffit maintenant que tout est installé et configuré, de compiler le programme principal et de le flasher sur la carte de développement NRF7002 DK.

## License
Ce projet est open-source sous licence Apache 2.0.