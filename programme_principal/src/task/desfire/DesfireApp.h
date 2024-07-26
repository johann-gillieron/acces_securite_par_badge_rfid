/*! \file desfireApp.h
 *  \brief DESFire application functions.
 *  This file contains the functions to initialize the DESFire application and to read the UID, ATQA, ATS and SAK from a NFC card.
 *  @note This header file is made to be compliant with both C and C++.
 */

#ifndef __DESFIREAPP_H__
#define __DESFIREAPP_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// The master key for the PICC (Proximity Integrated Circuit Card) is stored in secret.h.
bool init_defireApp();

// Function to read the UID, ATQA, ATS and SAK from a NFC card.
bool read_nfc_card(uint8_t *UID, uint8_t *UID_length, uint8_t *SAK, uint8_t *ATQA, uint8_t *ATS, uint8_t *ATS_length);

// The master key for the PICC (Proximity Integrated Circuit Card) is stored in secret.h.
bool AuthenticatePICC(uint8_t* pu8_KeyVersion);

// Function to read the secret key from the NFC card.
bool read_card_secret(uint8_t *secret);

// Function to initialize the card with the application and the secret keys.
bool init_card();

#ifdef __cplusplus
}
#endif
#endif // __DESFIREAPP_H__