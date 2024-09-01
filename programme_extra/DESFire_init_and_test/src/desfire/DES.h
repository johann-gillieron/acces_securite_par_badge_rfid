
#ifndef DES_H
#define DES_H

#include "DesFireKey.h"

class DES : public DESFireKey
{
public:
    DES();
    ~DES();
    bool SetKeyData(const uint8_t* u8_Key, int s32_KeySize, uint8_t u8_Version);
    bool CryptDataBlock(uint8_t u8_Out[8], const uint8_t u8_In[8], DESFireCipher e_Cipher);
        
private:
    enum DES_MODE
    {
        DES_DECRYPT = 0,
        DES_ENCRYPT = 1
    };

    #define DES_LONG uint32_t
    typedef uint8_t DES_cblock[8];
    
    struct DES_key_schedule
    {
        union 
        {
            DES_cblock cblock;
            DES_LONG deslong[2];
        } ks[16];
    };

    static void StoreKeyVersion(uint8_t* u8_KeyOut, const uint8_t* u8_KeyIn, int s32_KeySize, uint8_t u8_Version);
    static void set_key(const DES_cblock* key, DES_key_schedule* schedule);
    static void ecb_encrypt(const DES_cblock* in, DES_cblock* out, DES_key_schedule* ks, int enc);
    static void encrypt1(DES_LONG* data, DES_key_schedule* ks, int enc);

    DES_key_schedule mk_ks1; // first  component of a TDEA key
    DES_key_schedule mk_ks2; // second component of a TDEA key
    DES_key_schedule mk_ks3; // third  component of a TDEA key
};

#endif // DES_H
