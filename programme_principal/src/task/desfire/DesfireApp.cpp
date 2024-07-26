// The functions in this file are used to store and retrieve the dynamic secrets on a Desfire card.
#include "desfireApp.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DesfireApp, LOG_LEVEL_INF);
#include <stdint.h>
#include <zephyr/drivers/nfc.h>

#include "Utils.h"
#include "Buffer.h"
#include "Secrets.h"
#include "desfire.h"

#define USE_DESFIRE true
#define DEBUG_DESFIRE true

#if USE_DESFIRE
    // This compiler switch defines if you use AES (128 bit) or DES (168 bit) for the PICC master key and the application master key.
    // Cryptographers say that AES is better.
    // But the disadvantage of AES encryption is that it increases the power consumption of the card more than DES.
    // The maximum read distance is 5,3 cm when using 3DES keys and 4,0 cm when using AES keys.
    // (When USE_DESFIRE == false the same Desfire card allows a distance of 6,3 cm.)
    // If the card is too far away from the antenna you get a timeout error at the moment when the Authenticate command is executed.
    // IMPORTANT: Before changing this compiler switch, please execute the RESTORE command on all personalized cards!
    #define USE_AES false

    // This define should normally be zero
    // If you want to run the selftest (only available if USE_DESFIRE == true) you must set this to a value > 0.
    // Then you can enter TEST into the terminal to execute a selftest that tests ALL functions in the Desfire class.
    // The value that you can specify here is 1 or 2 which will be the debug level for the selftest.
    // At level 2 you see additionally the CMAC and the data sent to and received from the card.
    #define COMPILE_SELFTEST  0
    
    // This define should normally be false
    // If this is true you can use Classic cards / keyfobs additionally to Desfire cards.
    // This means that the code is compiled for Defire cards, but when a Classic card is detected it will also work.
    // This mode is not recommended because Classic cards do not offer the same security as Desfire cards.
    #define ALLOW_ALSO_CLASSIC false
#endif

#if USE_DESFIRE

    Desfire          gi_CLRC663; // The class instance that communicates with Mifare Desfire cards   

    #if USE_AES
        #define DESFIRE_KEY_TYPE   AES
        #define DEFAULT_APP_KEY    gi_CLRC663.AES_DEFAULT_KEY
    #else
        #define DESFIRE_KEY_TYPE   DES
        #define DEFAULT_APP_KEY    gi_CLRC663.DES3_DEFAULT_KEY
    #endif
    
    DESFIRE_KEY_TYPE gi_PiccMasterKey;
#endif

static struct kCard k_Card;

// prototypes
extern "C" {
bool read_nfc_card(uint8_t UID[10], uint8_t *UID_length, uint8_t *SAK, uint8_t ATQA[2], uint8_t ATS[32], uint8_t *ATS_length);
bool AuthenticatePICC(uint8_t* pu8_KeyVersion);
bool GenerateDesfireSecrets(struct kCard* pk_Card, DESFireKey* pi_AppMasterKey, uint8_t u8_StoreValue[8]);
bool CheckDesfireSecret(struct kCard* pk_Card, uint8_t u8_StoreValue[8]);
bool ChangePiccMasterKey(struct kCard k_Card);
bool StoreDesfireSecret(struct kCard* pk_Card);
bool RestoreDesfireCard(struct kCard k_Card);
void print_get_version(DESFireCardVersion *version);

bool init_defireApp() {
    LOG_INF("Initializing Desfire application");
#if USE_DESFIRE
    gi_PiccMasterKey.SetKeyData(SECRET_PICC_MASTER_KEY, sizeof(SECRET_PICC_MASTER_KEY), 0x00);
#endif
    return true;
}

bool read_nfc_card(uint8_t UID[10], uint8_t *UID_length, uint8_t *SAK, uint8_t ATQA[2], uint8_t ATS[32], uint8_t *ATS_length) {
    // Read the card
    k_Card = {0};
    uint8_t ats_length;
    bool nfc_com_status;
    k_Card.u8_UidLength = nfc_card_select(clrc663, k_Card.u8_UID, &k_Card.u8_SAK, k_Card.u8_ATQA, k_Card.u8_ATS);
    
    *UID_length = k_Card.u8_UidLength;

    if (k_Card.u8_UidLength == 0) 
    {
        LOG_DBG("No card detected");
        return false;
    }
    else
    {
        bytecpy(UID, k_Card.u8_UID, k_Card.u8_UidLength);
        *SAK = k_Card.u8_SAK;
        bytecpy(ATQA, k_Card.u8_ATQA, 2);
        ats_length = k_Card.u8_ATS[0];
        *ATS_length = ats_length;
        k_Card.e_CardType = CARD_Unknown;
        k_Card.u8_KeyVersion = 0x00;
        k_Card.e_CardType = CARD_CLASSIC;

        if (ats_length > 0) 
        {
            bytecpy(ATS, k_Card.u8_ATS+1, ats_length);
            nfc_com_status = gi_CLRC663.GetCardVersion(&k_Card.u8_Version);

            if(!nfc_com_status) 
            {
                LOG_DBG("No data received from card get version command");
            } 
            else 
            {
                k_Card.e_CardType = CARD_Desfire;
#if DEBUG_DESFIRE
                LOG_HEXDUMP_DBG(&k_Card.u8_Version, 28, "Version: ");
                print_get_version(&k_Card.u8_Version);
                uint32_t u32_IDlist[28];
                uint8_t     u8_AppCount;
                nfc_com_status = gi_CLRC663.GetApplicationIDs(u32_IDlist, &u8_AppCount);
                if(!nfc_com_status) 
                {
                    LOG_INF("No data received from card get application IDs command");
                } 
                else 
                {
                    LOG_INF("Application count: %d", u8_AppCount);
                    for (uint8_t i = 0; i < u8_AppCount; i++) 
                    {
                        LOG_INF("Application ID: %x", u32_IDlist[i]);
                    }
                }
#endif
            }
        } 
        else 
        {
            LOG_WRN("No ATS : NFC tag not compliant with ISO14443-4");
        }
    }
    return true;
}

bool AuthenticatePICC(uint8_t* pu8_KeyVersion)
{
    if (!gi_CLRC663.SelectApplication(0x000000)){ // PICC level
        LOG_ERR("Failed to select application");
        return false;
    }

    if (!gi_CLRC663.GetKeyVersion(0, pu8_KeyVersion)){ // Get version of PICC master key
        LOG_ERR("Failed to get key version");
        return false;
    }

    // The factory default key has version 0, while a personalized card has key version CARD_KEY_VERSION
    if (*pu8_KeyVersion == CARD_KEY_VERSION)
    {
        LOG_INF("Card is personalized");
        if (!gi_CLRC663.Authenticate(0, &gi_PiccMasterKey))
            return false;
    }
    else // The card is still in factory default state
    {
        LOG_INF("Card is in factory default state");
        if (!gi_CLRC663.Authenticate(0, &gi_CLRC663.DES3_DEFAULT_KEY))
            return false;
    }
    return true;
}

bool init_card() {
    // First delete the application (The current application master key may have changed after changing the user name for that card)
    //if (!gi_CLRC663.DeleteApplicationIfExists(0x010000))
    //    LOG_ERR("Failed to delete application 010000");

    if (k_Card.e_CardType == CARD_Desfire) {
        if (!(gi_CLRC663.SelectApplication(CARD_APPLICATION_ID))) // PICC level
            LOG_INF("Failed to select application(0x%x)", CARD_APPLICATION_ID);
            if(!StoreDesfireSecret(&k_Card)) {
                LOG_ERR("Failed to store secret on card");
                return false;
            }
        } else {
            LOG_ERR("Card application is already initialized");
            return true;
        }
    return true;
}

bool read_card_secret(uint8_t *secret)
{
    uint8_t u8_FileData[8];
    uint8_t u8_ats_length;
    k_Card = {0};
    bool nfc_com_status;
    k_Card.u8_UidLength = nfc_card_select(clrc663, k_Card.u8_UID, &k_Card.u8_SAK, k_Card.u8_ATQA, k_Card.u8_ATS);

    if (k_Card.u8_UidLength == 0) 
    {
        k_Card.e_CardType = CARD_Unknown;
        LOG_DBG("No card detected");
        return false;
    }
    else
    {
        k_Card.u8_KeyVersion = 0x00;
        k_Card.e_CardType = CARD_CLASSIC;
        u8_ats_length = k_Card.u8_ATS[0];
        if (u8_ats_length > 0)
        {
            k_Card.e_CardType = CARD_Desfire;
            nfc_com_status = gi_CLRC663.GetCardVersion(&k_Card.u8_Version);
            if (k_Card.e_CardType == CARD_Desfire) {
                if (!CheckDesfireSecret(&k_Card, u8_FileData)) {
                    LOG_ERR("Failed to check secret on card");
                    return false;
                }
            }
        } else {
            LOG_ERR("Card is not a Desfire card");
            return false;
        }
    }
    bytecpy(secret, u8_FileData, 8); // Copy the secret to the output parameter
    return true;

}

// Generate two dynamic secrets: the Application master key (AES 16 uint8_t or DES 24 uint8_t) and the 8 uint8_t StoreValue.
// Both are derived from the 7 uint8_t card UID and the the user name + random data stored in EEPROM using two 24 uint8_t 3K3DES keys.
// This function takes only 6 milliseconds to do the cryptographic calculations.
bool GenerateDesfireSecrets(struct kCard* pk_Card, DESFireKey* pi_AppMasterKey, uint8_t u8_StoreValue[8])
{
    // The buffer is initialized to zero here
    uint8_t u8_Data[24] = {0}; 

    // Copy the 7 uint8_t card UID into the buffer
    if (pk_Card->u8_UidLength < 7) {
        LOG_ERR("Invalid UID length");
        return false;
    }

    memcpy(u8_Data, pk_Card->u8_UID, 7);
    u8_Data[7] = pk_Card->u8_Version.yearProd;

    uint8_t u8_AppMasterKey[24];

    DES i_3KDes;
    if (!i_3KDes.SetKeyData(SECRET_APPLICATION_KEY, sizeof(SECRET_APPLICATION_KEY), 0) || // set a 24 uint8_t key (168 bit)
        !i_3KDes.CryptDataCBC(CBC_SEND, KEY_ENCIPHER, u8_AppMasterKey, u8_Data, 24)) {
        LOG_ERR("Failed to encrypt data for application key 24");
        return false;
    }

    if (!i_3KDes.SetKeyData(SECRET_STORE_VALUE_KEY, sizeof(SECRET_STORE_VALUE_KEY), 0) || // set a 24 uint8_t key (168 bit)
        !i_3KDes.CryptDataCBC(CBC_SEND, KEY_ENCIPHER, u8_StoreValue, u8_Data, 8)) {
        LOG_ERR("Failed to encrypt data for store value 8");
        return false;
    }
    LOG_HEXDUMP_INF(u8_StoreValue, 8, "secret: ");

    // If the key is an AES key only the first 16 uint8_ts will be used
    if (!pi_AppMasterKey->SetKeyData(u8_AppMasterKey, sizeof(u8_AppMasterKey), CARD_KEY_VERSION)) {
        LOG_ERR("Failed to set application key");
        return false;
    }
    return true;
}

// Check that the data stored on the card is the same as the secret generated by GenerateDesfireSecrets()
bool CheckDesfireSecret(struct kCard* pk_Card, uint8_t *out_Value)
{
    uint8_t u8_StoreValue[8];
    DESFIRE_KEY_TYPE i_AppMasterKey;
    if (!GenerateDesfireSecrets(pk_Card, &i_AppMasterKey, u8_StoreValue))
        return false;

    if (!gi_CLRC663.SelectApplication(0x000000)) // PICC level
        return false;

    uint8_t u8_Version; 
    if (!gi_CLRC663.GetKeyVersion(0, &u8_Version))
        return false;

    // The factory default key has version 0, while a personalized card has key version CARD_KEY_VERSION
    //if (u8_Version != CARD_KEY_VERSION)
    //    return false;

    if (!gi_CLRC663.SelectApplication(CARD_APPLICATION_ID))
        return false;

    if (!gi_CLRC663.Authenticate(0, &i_AppMasterKey))
        return false;

    // Read the 8 uint8_t secret from the card
    uint8_t u8_FileData[8];
    if (!gi_CLRC663.ReadFileData(CARD_FILE_ID, 0, 8, u8_FileData))
        return false;

    if (memcmp(u8_FileData, u8_StoreValue, 8) != 0)
        return false;

    LOG_INF("Secret on card is correct");

    bytecpy(out_Value, u8_StoreValue, 8);
    return true;
}

// Store the SECRET_PICC_MASTER_KEY on the card
bool ChangePiccMasterKey(struct kCard k_Card)
{
    uint8_t u8_KeyVersion;
    if (!AuthenticatePICC(&u8_KeyVersion))
        return false;

    if (u8_KeyVersion != CARD_KEY_VERSION) // empty card
    {
        // Store the secret PICC master key on the card.
        if (!gi_CLRC663.ChangeKey(0, &gi_PiccMasterKey, NULL))
            return false;

        // A key change always requires a new authentication
        if (!gi_CLRC663.Authenticate(0, &gi_PiccMasterKey))
            return false;
    }
    return true;
}

// Create the application SECRET_APPLICATION_ID,
// store the dynamic Application master key in the application,
// create a StandardDataFile SECRET_FILE_ID and store the dynamic 16 uint8_t value into that file.
// This function requires previous authentication with PICC master key.
bool StoreDesfireSecret(struct kCard* pk_Card)
{
    if (CARD_APPLICATION_ID == 0x000000 || CARD_KEY_VERSION == 0){
        LOG_ERR("Invalid values in Secrets.h");
        return false; // severe errors in Secrets.h -> abort
    }
  
    DESFIRE_KEY_TYPE i_AppMasterKey;
    uint8_t u8_StoreValue[8];
    if (!GenerateDesfireSecrets(pk_Card, &i_AppMasterKey, u8_StoreValue)){
        LOG_ERR("Failed to generate secrets");
        return false;
    }
    // First delete the application (The current application master key may have changed after changing the user name for that card)
    if (!gi_CLRC663.DeleteApplicationIfExists(CARD_APPLICATION_ID)){
        LOG_ERR("Failed to delete application");
        return false;
    }
    // Create the new application with default settings (we must still have permission to change the application master key later)
    if (!gi_CLRC663.CreateApplication(CARD_APPLICATION_ID, KS_FACTORY_DEFAULT, 1, i_AppMasterKey.GetKeyType())){
        LOG_ERR("Failed to create application");
        return false;
    }
    // After this command all the following commands will apply to the application (rather than the PICC)
    if (!gi_CLRC663.SelectApplication(CARD_APPLICATION_ID)){
        LOG_ERR("Failed to select application");
        return false;
    }
    // Authentication with the application's master key is required
    if (!gi_CLRC663.Authenticate(0, &DEFAULT_APP_KEY)){
        LOG_ERR("Failed to authenticate application");
        return false;
    }
    // Change the master key of the application
    if (!gi_CLRC663.ChangeKey(0, &i_AppMasterKey, NULL)){
        LOG_ERR("Failed to change application key");
        return false;
    }
    // A key change always requires a new authentication with the new key
    if (!gi_CLRC663.Authenticate(0, &i_AppMasterKey)){
        LOG_ERR("Failed to authenticate application");
        return false;
    }
    // After this command the application's master key and it's settings will be frozen. They cannot be changed anymore.
    // To read or enumerate any content (files) in the application the application master key will be required.
    // Even if someone knows the PICC master key, he will neither be able to read the data in this application nor to change the app master key.
    if (!gi_CLRC663.ChangeKeySettings(KS_CHANGE_KEY_FROZEN)){
        LOG_ERR("Failed to freeze application key");
        return false;
    }
    // --------------------------------------------

    // Create Standard Data File with 16 uint8_ts length
    DESFireFilePermissions k_Permis;
    k_Permis.e_ReadAccess         = AR_KEY0;
    k_Permis.e_WriteAccess        = AR_KEY0;
    k_Permis.e_ReadAndWriteAccess = AR_KEY0;
    k_Permis.e_ChangeAccess       = AR_KEY0;
    if (!gi_CLRC663.CreateStdDataFile(CARD_FILE_ID, &k_Permis, 8)){
        LOG_ERR("Failed to create data file");
        return false;
    }

    // Write the StoreValue into that file
    if (!gi_CLRC663.WriteFileData(CARD_FILE_ID, 0, 8, u8_StoreValue)){
        LOG_ERR("Failed to write data file");
        return false;       
    }
    return true;
}

// If you have already written the master key to a card and want to use the card for another purpose 
// you can restore the master key with this function. Additionally the application SECRET_APPLICATION_ID is deleted.
// If a user has been stored in the EEPROM for this card he will also be deleted.
bool RestoreDesfireCard(struct kCard k_Card){

    if ((k_Card.e_CardType & CARD_Desfire) == 0)
    {
        LOG_ERR("The card is not a Desfire card.");
        return false;
    }

    uint8_t u8_KeyVersion;
    if (!AuthenticatePICC(&u8_KeyVersion))
        return false;

    return true;
}


void print_get_version(DESFireCardVersion *version)
{
    LOG_INF("== Card version information: ==");
    LOG_HEXDUMP_INF(version->uid, 7,"UID: ");
    if(version->hardwareVendorId == 0x04 && version->softwareVendorId == 0x04) LOG_INF("Vendor: NXP");
    else if (version->hardwareVendorId == version->softwareVendorId) LOG_INF("Vendor: Unknown, ID %x", version->hardwareVendorId);
    else LOG_INF("Vendor: Error Software and hardware vendor ID do not match");

    if (version->hardwareType == 0x01 && version->softwareType == 0x01) LOG_INF("Type: DESFIRE EV%d", version->hardwareSubType);
    else LOG_INF("Type: Error Software and hardware type do not match to DESFIRE EVx format");

    LOG_INF("Hardware version: %d.%d", version->hardwareMajVersion ,version->hardwareMinVersion);

    if (version->hardwareProtocol == 0x05) LOG_INF("Hardware protocol: ISO14443-2/3");
    else LOG_INF("Hardware protocol: Error Unknown type %x", version->hardwareType);

    LOG_INF("Software version: %d.%d", version->softwareMajVersion, version->softwareMinVersion);
    if (version->softwareProtocol == 0x05) LOG_INF("Software protocol: ISO14443-3/4");
    else LOG_INF("Software protocol: Error Unknown type %x", version->softwareType);
    
    if (version->hardwareStorageSize == version->softwareStorageSize) LOG_INF("Storage size: %d bytes", 1 << (version->hardwareStorageSize / 2));
    else LOG_INF("Storage size: Error Software and hardware storage size do not match");

    LOG_HEXDUMP_INF(version->batchNo, 5, "Batch number: ");
    LOG_INF("Production week: %x", version->cwProd);
    LOG_INF("Production year: 20%x", version->yearProd);
    LOG_INF("== Card version information end ==");
}

} // extern "C"