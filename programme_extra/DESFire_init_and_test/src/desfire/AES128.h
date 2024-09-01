
#ifndef TI_OPT_AES_H_
#define TI_OPT_AES_H_

#include "DesFireKey.h"

class AES : public DESFireKey
{
public:
    AES();
    ~AES();
    bool SetKeyData(const uint8_t* u8_Key, int32_t s32_KeySize, uint8_t u8_Version);
    bool CryptDataBlock(uint8_t* u8_Out, const uint8_t* u8_In, DESFireCipher e_Cipher);
    
private:
    static void aes_enc_dec(unsigned char state[16], unsigned char key[16], unsigned char dir);
    static unsigned char galois_mul2(unsigned char value);
};

#endif // TI_OPT_AES_H_

