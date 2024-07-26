#ifndef UTILS_H
#define UTILS_H

#ifdef _ARDUINO_VER
    #include <Arduino.h>
    #define TRUE   true
    #define FALSE  false

#else 
    #include <soc.h>
    #include <zephyr/kernel.h>
    #include <zephyr/sys/printk.h> 
    #include <zephyr/devicetree.h>
    #include <zephyr/drivers/nfc.h>
    #include <stdint.h>

#endif
#define CLRC663_PACKBUFFSIZE 256
#define LF  "\r\n" // LineFeed 

extern const struct device *clrc663;
const uint8_t mu8_DebugLevel = 0; // 0 = no debug output, 1 = some debug output, 2 = more debug output
// -------------------------------------------------------------------------------------------------------------------

enum eCardType
{
    CARD_Unknown   = 0, // Another card
    CARD_Desfire   = 1, // A Desfire card with normal 7 byte UID  (bit 0)
    CARD_DesRandom = 3, // A Desfire card with 4 byte random UID  (bit 0 + 1)
    CARD_CLASSIC   = 4, // A Mifare Classic card
};

// global variables
extern uint8_t mu8_PacketBuffer[];
extern bool gb_InitSuccess;
extern uint64_t gu64_LastID;
extern uint64_t gu64_LastPasswd;
extern uint32_t gu32_CommandPos;
extern int8_t gs8_CommandBuffer[];
/*
char        gs8_CommandBuffer[500];    // Stores commands typed by the user via Terminal and the password
uint32_t    gu32_CommandPos   = 0;     // Index in gs8_CommandBuffer
uint64_t    gu64_LastPasswd   = 0;     // Timestamp when the user has enetered the password successfully
uint64_t    gu64_LastID       = 0;     // The last card UID that has been read by the RFID reader  
bool        gb_InitSuccess    = false; // true if the PN532 has been initialized successfully
extern uint8_t     mu8_PacketBuffer[CLRC663_PACKBUFFSIZE]; // Buffer for the CLRC663
*/
class Utils
{
public:
    // returns the current tick counter
    // If you compile on Visual Studio see WinDefines.h
    static inline uint32_t GetMillis()
    {
        return k_uptime_get_32();
    }

    // If you compile on Visual Studio see WinDefines.h
    static inline void DelayMilli(int32_t s32_MilliSeconds)
    {
        k_sleep(K_MSEC(s32_MilliSeconds));
    }

    // This function is only required for Software SPI mode.
    // If you compile on Visual Studio see WinDefines.h
    static inline void DelayMicro(int32_t s32_MicroSeconds)
    {
        k_sleep(K_USEC(s32_MicroSeconds));
    }

    static uint64_t GetMillis64();
    static void     Print(const char*   s8_Text,  const char* s8_LF=NULL);
    static void     PrintDec  (int32_t      s32_Data, const char* s8_LF=NULL);
    static void     PrintHex8 (uint8_t     u8_Data,  const char* s8_LF=NULL);
    static void     PrintHex16(uint16_t u16_Data, const char* s8_LF=NULL);
    static void     PrintHex32(uint32_t u32_Data, const char* s8_LF=NULL);
    static void     PrintHexBuf(const uint8_t* u8_Data, const uint32_t u32_DataLen, const char* s8_LF=NULL, int32_t s32_Brace1=-1, int32_t S32_Brace2=-1);
    static void     PrintInterval(uint64_t u64_Time, const char* s8_LF=NULL);
    static void     GenerateRandom(uint8_t* u8_Random, int32_t s32_Length);
    static void     RotateBlockLeft(uint8_t* u8_Out, const uint8_t* u8_In, int32_t s32_Length);
    static void     BitShiftLeft(uint8_t* u8_Data, int32_t s32_Length);
    static void     XorDataBlock(uint8_t* u8_Out,  const uint8_t* u8_In, const uint8_t* u8_Xor, int32_t s32_Length);    
    static void     XorDataBlock(uint8_t* u8_Data, const uint8_t* u8_Xor, int32_t s32_Length);
    static uint16_t CalcCrc16(const uint8_t* u8_Data,  int32_t s32_Length);
    static uint32_t CalcCrc32(const uint8_t* u8_Data1, int32_t s32_Length1, const uint8_t* u8_Data2=NULL, int32_t s32_Length2=0);
    static int32_t  strnicmp(const char* str1, const char* str2, uint32_t u32_MaxCount);
    static int32_t  stricmp (const char* str1, const char* str2);

private:
    static uint32_t CalcCrc32(const uint8_t* u8_Data, int32_t s32_Length, uint32_t u32_Crc);
};

#endif // UTILS_H
