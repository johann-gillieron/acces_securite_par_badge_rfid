/**************************************************************************
    
    @author   Elmü   
    This file contains secret values for encryption.
  
**************************************************************************/

#ifndef SECRETS_H
#define SECRETS_H

// The PICC master key.
// This 3K3DES or AES key is the "god key".
// It allows to format the card and erase ALL it's content (except the PICC master key itself).
// This key will be stored on your Desfire card when you execute the command "ADD {Username}" in the terminal.
// To restore the master key to the factory default DES key use the command "RESTORE" in the terminal.
// If you set the compiler switch USE_AES = true, only the first 16 bytes of this key will be used.
// IMPORTANT: Before changing this key, please execute the RESTORE command on all personalized cards!
// IMPORTANT: When you compile for DES, the least significant bit (bit 0) of all bytes in this key 
//            will be modified, because it stores the key version.
const uint8_t SECRET_PICC_MASTER_KEY[24] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// This 3K3DES key is used to derive a 16 uint8_t application master key from the UID of the card and the user name.
// The purpose is that each card will have it's unique application master key that can be calculated from known values.
const uint8_t SECRET_APPLICATION_KEY[24] = { 0x81, 0xDF, 0x6A, 0xD9, 0x89, 0xE9, 0xA2, 0xD1, 0xC5, 0xB3, 0xE3, 0x9D, 0xE9, 0x60, 0x43, 0xE3, 0x5B, 0x60, 0x85, 0x8B, 0x99, 0xD8, 0xD3, 0x5B };

// This 3K3DES key is used to derive the 16 uint8_t store value from the UID of the card and the user name.
// This value is stored in a standard data file on the card.
// The purpose is that each card will have it's unique store value that can be calculated from known values.
const uint8_t SECRET_STORE_VALUE_KEY[24] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// -----------------------------------------------------------------------------------------------------------

// The ID of the application to be created
// This value must be between 0x000001 and 0xFFFFFF (NOT zero!)
const uint32_t CARD_APPLICATION_ID = 0x041167;

// The ID of the file to be created in the above application
// This value must be between 0 and 31
const uint8_t CARD_FILE_ID = 0;

// This 8 bit version number is uploaded to the card together with the key itself.
// This version is irrelevant for encryption. 
// It is just a version number for the key that you can obtain with Desfire::GetKeyVersion().
// The key version can always be obtained without authentication.
// You can theoretically have multiple master keys and by obtaining the version you know which one to use for authentication.
// This value must be between 1 and 255 (NOT zero!)
const uint8_t CARD_KEY_VERSION = 0x98;

#endif
