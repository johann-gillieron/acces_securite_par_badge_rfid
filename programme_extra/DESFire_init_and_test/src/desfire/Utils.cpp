/**************************************************************************
    
    @author   Elmü   
    class Utils: Some small functions.
  
**************************************************************************/

#include "Utils.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(nfc_utils, LOG_LEVEL_ERR);

// Utils::Print("Hello World", LF); --> prints "Hello World\r\n"
void Utils::Print(const char* s8_Text, const char* s8_LF) //=NULL
{
    LOG_INF("%s%s", s8_Text, s8_LF);
}
void Utils::PrintDec(int32_t s32_Data, const char* s8_LF) // =NULL
{
    LOG_INF("%d%s", s32_Data, s8_LF);
}
void Utils::PrintHex8(uint8_t u8_Data, const char* s8_LF) // =NULL
{
    LOG_INF("%02X%s", u8_Data, s8_LF);
}
void Utils::PrintHex16(uint16_t u16_Data, const char* s8_LF) // =NULL
{
    LOG_INF("%04X%s", u16_Data, s8_LF);
}
void Utils::PrintHex32(uint32_t u32_Data, const char* s8_LF) // =NULL
{
    LOG_INF("%08X%s", u32_Data, s8_LF);
}

// Prints a hexadecimal buffer as 2 digit HEX numbers
// At the uint8_t position s32_Brace1 a "<" will be inserted
// At the uint8_t position s32_Brace2 a ">" will be inserted
// Output will look like: "00 00 FF 03 FD <D5 4B 00> E0 00"
// This is used to mark the data bytes in the packet.
// If the parameters s32_Brace1, s32_Brace2 are -1, they do not appear
void Utils::PrintHexBuf(const uint8_t* u8_Data, const uint32_t u32_DataLen, const char* s8_LF, int32_t s32_Brace1, int32_t s32_Brace2)
{
    for (uint32_t i=0; i<u32_DataLen; i++)
    {
        if (i == (uint32_t)s32_Brace1) LOG_INF("<");
        LOG_INF("%02X ", u8_Data[i]);
        if (i == (uint32_t)s32_Brace2) LOG_INF(">");
    }
}

// Converts an interval in milliseconds into days, hours, minutes and prints it
void Utils::PrintInterval(uint64_t u64_Time, const char* s8_LF)
{
    u64_Time /= 60*1000;
    int32_t s32_Min  = (int32_t)(u64_Time % 60);
    u64_Time /= 60;
    int32_t s32_Hour = (int32_t)(u64_Time % 24);    
    u64_Time /= 24;
    int32_t s32_Days = (int32_t)u64_Time; 
    LOG_INF("%d days, %02d:%02d hours", s32_Days, s32_Hour, s32_Min);
}

// We need a special time counter that does not roll over after 49 days (as millis() does) 
uint64_t Utils::GetMillis64()
{
    uint64_t u64_Time = k_uptime_get();
    return u64_Time;
}

// Multi uint8_t XOR operation In -> Out
// If u8_Out and u8_In are the same buffer use the other function below.
void Utils::XorDataBlock(uint8_t* u8_Out, const uint8_t* u8_In, const uint8_t* u8_Xor, int32_t s32_Length)
{
    for (int32_t B=0; B<s32_Length; B++)
    {
        u8_Out[B] = u8_In[B] ^ u8_Xor[B];
    }
}

// Multi uint8_t XOR operation in the same buffer
void Utils::XorDataBlock(uint8_t* u8_Data, const uint8_t* u8_Xor, int32_t s32_Length)
{
    for (int32_t B=0; B<s32_Length; B++)
    {
        u8_Data[B] ^= u8_Xor[B];
    }
}

// Rotate a block of 8 uint8_t to the left by one uint8_t.
// ATTENTION: u8_Out and u8_In must not be the same buffer!
void Utils::RotateBlockLeft(uint8_t* u8_Out, const uint8_t* u8_In, int32_t s32_Length)
{
    int32_t s32_Last = s32_Length -1;
    memcpy(u8_Out, u8_In + 1, s32_Last);
    u8_Out[s32_Last] = u8_In[0];
}

// Logical Bit Shift Left. Shift MSB out, and place a 0 at LSB position
void Utils::BitShiftLeft(uint8_t* u8_Data, int32_t s32_Length)
{
    for (int32_t n=0; n<s32_Length-1; n++) 
    {
        u8_Data[n] = (u8_Data[n] << 1) | (u8_Data[n+1] >> 7);
    }
    u8_Data[s32_Length - 1] <<= 1;
}

// Generate multi uint8_t random
void Utils::GenerateRandom(uint8_t* u8_Random, int32_t s32_Length)
{
    uint32_t u32_Now = GetMillis();
    for (int32_t i=0; i<s32_Length; i++)
    {
        u8_Random[i] = (uint8_t)u32_Now;
        u32_Now *= 127773;
        u32_Now += 16807;
    }
}

// ITU-V.41 (ISO 14443A)
// This CRC is used only for legacy authentication. (not implemented anymore)
uint16_t Utils::CalcCrc16(const uint8_t* u8_Data, int32_t s32_Length)
{
    uint16_t u16_Crc = 0x6363;
    for (int32_t i=0; i<s32_Length; i++)
    {
        uint8_t ch = u8_Data[i];
        ch = ch ^ (uint8_t)u16_Crc;
        ch = ch ^ (ch << 4);
        u16_Crc = (u16_Crc >> 8) ^ ((uint16_t)ch << 8) ^ ((uint16_t)ch << 3) ^ ((uint16_t)ch >> 4);
    }
    return u16_Crc;
}

// This CRC is used for ISO and AES authentication.
// The new Desfire EV1 authentication calculates the CRC32 also over the command, but the command is not encrypted later.
// This function allows to include the command into the calculation without the need to add the command to the same buffer that is later encrypted.
uint32_t Utils::CalcCrc32(const uint8_t* u8_Data1, int32_t s32_Length1, // data to process
                          const uint8_t* u8_Data2, int32_t s32_Length2) // optional additional data to process (these parameters may be omitted)
{
    uint32_t u32_Crc = 0xFFFFFFFF;
    u32_Crc = CalcCrc32(u8_Data1, s32_Length1, u32_Crc);
    u32_Crc = CalcCrc32(u8_Data2, s32_Length2, u32_Crc);
    return u32_Crc;
}

// private
uint32_t Utils::CalcCrc32(const uint8_t* u8_Data, int32_t s32_Length, uint32_t u32_Crc)
{
    for (int32_t i=0; i<s32_Length; i++)
    {
        u32_Crc ^= u8_Data[i];
        for (int32_t b=0; b<8; b++)
        {
            bool b_Bit = (u32_Crc & 0x01) > 0;
            u32_Crc >>= 1;
            if (b_Bit) u32_Crc ^= 0xEDB88320;
        }
    }
    return u32_Crc;
}

// -----------------------------------------------------------------------------------------------
// These functions are only required for some boards which do not define stricmp() and strnicmp()
// They work only for english characters, but this is completely sufficient for this project.
// For Teensy they can be replaced with the original functions.
int32_t Utils::stricmp(const char* str1, const char* str2)
{
    return strnicmp(str1, str2, 0xFFFFFFFF);
}
int32_t Utils::strnicmp(const char* str1, const char* str2, uint32_t u32_MaxCount)
{
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    for (uint32_t i=0; i<u32_MaxCount; i++)
    {
        c1 = str1[i];
        c2 = str2[i];
        if (c1 >= 'a' && c1 <= 'z') c1 -= 32; // make upper case
        if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
        if (c1 != c2 || c1 == 0)
            break;
    }
    if (c1 < c2) return -1;
    if (c1 > c2) return  1;
    return 0;
}


// -----------------------------------------------------------------------------------------------