/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: --bpp 1 --size 14 --no-compress --font Poppins-SemiBold.ttf --range 0-255 --format lvgl -o font_poppins_14.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef FONT_POPPINS_14
#define FONT_POPPINS_14 1
#endif

#if FONT_POPPINS_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0000 "\u0000" */
    0x0,

    /* U+000D "\r" */
    0x0,

    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xfc, 0x30,

    /* U+0022 "\"" */
    0xff, 0xf0,

    /* U+0023 "#" */
    0xc, 0x40, 0xcc, 0xc, 0xc7, 0xff, 0x18, 0xc1,
    0x98, 0x19, 0x8f, 0xfe, 0x31, 0x83, 0x18, 0x33,
    0x0,

    /* U+0024 "$" */
    0x10, 0x73, 0xf6, 0xbd, 0x1f, 0xf, 0x8b, 0xd7,
    0xfc, 0xe0, 0x80,

    /* U+0025 "%" */
    0x71, 0x9b, 0x23, 0x6c, 0x3b, 0x0, 0x60, 0x18,
    0x3, 0x70, 0xdb, 0x13, 0x66, 0x38,

    /* U+0026 "&" */
    0x3c, 0x19, 0x86, 0x1, 0xc0, 0x78, 0x3f, 0x6c,
    0xfb, 0x1c, 0xc7, 0x9f, 0x70,

    /* U+0027 "'" */
    0xfc,

    /* U+0028 "(" */
    0x19, 0x98, 0xce, 0x63, 0x18, 0xc6, 0x31, 0x86,
    0x30, 0xc7,

    /* U+0029 ")" */
    0xc3, 0xc, 0x63, 0xc, 0x63, 0x18, 0xc6, 0x73,
    0x19, 0x9c,

    /* U+002A "*" */
    0x27, 0xc9, 0xf2, 0x0,

    /* U+002B "+" */
    0x18, 0x30, 0x67, 0xf1, 0x83, 0x6, 0x0,

    /* U+002C "," */
    0x6d, 0x60,

    /* U+002D "-" */
    0xfc,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x18, 0xc6, 0x63, 0x18, 0xc4, 0x63, 0x18, 0xcc,
    0x63, 0x18,

    /* U+0030 "0" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x3c,

    /* U+0031 "1" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x30,

    /* U+0032 "2" */
    0x7d, 0xcb, 0x18, 0x30, 0x61, 0xc7, 0xc, 0x38,
    0xe3, 0xf8,

    /* U+0033 "3" */
    0x7d, 0xcf, 0x18, 0x30, 0xe3, 0x3, 0x83, 0xc7,
    0x8d, 0xf0,

    /* U+0034 "4" */
    0xe, 0xe, 0x1e, 0x1e, 0x36, 0x76, 0x66, 0xc6,
    0xff, 0x6, 0x6,

    /* U+0035 "5" */
    0xfd, 0x83, 0x6, 0xf, 0xdd, 0xc1, 0x83, 0xc7,
    0x9d, 0xf0,

    /* U+0036 "6" */
    0x3c, 0xcb, 0x6, 0xf, 0xdd, 0xf1, 0xe3, 0xc6,
    0xd8, 0xf0,

    /* U+0037 "7" */
    0xfe, 0xc, 0x30, 0x60, 0xc3, 0x6, 0x18, 0x30,
    0x61, 0x80,

    /* U+0038 "8" */
    0x7d, 0x8f, 0x1e, 0x3c, 0x6f, 0xb1, 0xe3, 0xc7,
    0x8d, 0xe0,

    /* U+0039 "9" */
    0x79, 0xdb, 0x1e, 0x3c, 0x7d, 0xdf, 0x83, 0xc6,
    0xd8, 0xe0,

    /* U+003A ":" */
    0xc0, 0x3,

    /* U+003B ";" */
    0xc0, 0x3e, 0x80,

    /* U+003C "<" */
    0x18, 0xc6, 0x18, 0x30, 0x60,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0x60, 0xc1, 0x86, 0x31, 0x80,

    /* U+003F "?" */
    0x7b, 0x3c, 0xc3, 0x19, 0x80, 0x0, 0x1, 0x80,

    /* U+0040 "@" */
    0x7, 0xc0, 0xc3, 0x8c, 0xc, 0xcf, 0xbe, 0xd9,
    0xec, 0x4f, 0x62, 0x7b, 0x36, 0xce, 0xe7, 0x0,
    0x1c, 0x0, 0x3e, 0x0,

    /* U+0041 "A" */
    0xc, 0x7, 0x81, 0xe0, 0x78, 0x33, 0xc, 0xc3,
    0x31, 0xfe, 0x61, 0x98, 0x60,

    /* U+0042 "B" */
    0xfd, 0x8f, 0x1e, 0x3f, 0x98, 0xf1, 0xe3, 0xc7,
    0xf8,

    /* U+0043 "C" */
    0x1e, 0x18, 0xe6, 0x1b, 0x0, 0xc0, 0x30, 0xc,
    0x1, 0x86, 0x63, 0x87, 0x80,

    /* U+0044 "D" */
    0xfc, 0x63, 0xb0, 0xd8, 0x3c, 0x1e, 0xf, 0x7,
    0x86, 0xc7, 0x7e, 0x0,

    /* U+0045 "E" */
    0xff, 0xc, 0x30, 0xff, 0xc, 0x30, 0xc3, 0xf0,

    /* U+0046 "F" */
    0xff, 0xc, 0x30, 0xc3, 0xec, 0x30, 0xc3, 0x0,

    /* U+0047 "G" */
    0x1f, 0x1c, 0x66, 0x1f, 0x0, 0xc0, 0x31, 0xfc,
    0xd, 0x83, 0x71, 0x87, 0x80,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3,

    /* U+0049 "I" */
    0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0xc, 0x30, 0xc3, 0xc, 0x30, 0xc3, 0xcd, 0xe0,

    /* U+004B "K" */
    0xc7, 0xce, 0xcc, 0xd8, 0xf0, 0xf0, 0xd8, 0xcc,
    0xce, 0xc7,

    /* U+004C "L" */
    0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc7, 0xc0,

    /* U+004D "M" */
    0xe0, 0xfc, 0x1f, 0x83, 0xf8, 0xff, 0x1f, 0xb6,
    0xf6, 0xde, 0x73, 0xce, 0x78, 0xcc,

    /* U+004E "N" */
    0xc3, 0xe3, 0xe3, 0xf3, 0xdb, 0xdb, 0xcf, 0xc7,
    0xc7, 0xc3,

    /* U+004F "O" */
    0x1e, 0x18, 0x66, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xd, 0x86, 0x61, 0x87, 0x80,

    /* U+0050 "P" */
    0xfd, 0x8f, 0x1e, 0x3c, 0x7f, 0xb0, 0x60, 0xc1,
    0x80,

    /* U+0051 "Q" */
    0x1e, 0x18, 0x66, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xd, 0x87, 0x61, 0x87, 0xc0, 0x18, 0x3,

    /* U+0052 "R" */
    0xfd, 0x8f, 0x1e, 0x3c, 0x7f, 0xb6, 0x66, 0xcd,
    0x8c,

    /* U+0053 "S" */
    0x7d, 0x9f, 0x7, 0xf, 0xcf, 0xc3, 0xe3, 0xc6,
    0xf8,

    /* U+0054 "T" */
    0xfe, 0x30, 0x60, 0xc1, 0x83, 0x6, 0xc, 0x18,
    0x30,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0x66, 0x3c,

    /* U+0056 "V" */
    0xc1, 0xd8, 0x66, 0x19, 0x8c, 0x33, 0xc, 0xc1,
    0xe0, 0x78, 0x1e, 0x3, 0x0,

    /* U+0057 "W" */
    0xe3, 0xd, 0x8e, 0x36, 0x79, 0x99, 0xe6, 0x66,
    0x98, 0xd3, 0x63, 0xcf, 0xf, 0x3c, 0x1c, 0x70,
    0x61, 0x80,

    /* U+0058 "X" */
    0xc3, 0xe6, 0x6e, 0x3c, 0x38, 0x38, 0x3c, 0x6e,
    0xe6, 0xc7,

    /* U+0059 "Y" */
    0xc3, 0xe7, 0x66, 0x7e, 0x3c, 0x3c, 0x18, 0x18,
    0x18, 0x18,

    /* U+005A "Z" */
    0xfe, 0x1c, 0x30, 0xe1, 0x86, 0x1c, 0x30, 0xe1,
    0xfc,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcf,

    /* U+005C "\\" */
    0x41, 0x86, 0x18, 0x20, 0xc3, 0xc, 0x10, 0x61,
    0x86, 0x8, 0x30, 0xc3,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3f,

    /* U+005E "^" */
    0xc, 0xe, 0x7, 0x86, 0xc3, 0x31, 0x99, 0x8c,
    0xc3,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0x19, 0x80,

    /* U+0061 "a" */
    0x3f, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0x67, 0x3f,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xe6, 0xfc,

    /* U+0063 "c" */
    0x3c, 0xcf, 0x6, 0xc, 0x18, 0x19, 0x9e,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3f, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x67, 0x3f,

    /* U+0065 "e" */
    0x3c, 0x66, 0x63, 0xff, 0x40, 0x63, 0x66, 0x3c,

    /* U+0066 "f" */
    0x3, 0x66, 0xf6, 0x66, 0x66, 0x66,

    /* U+0067 "g" */
    0x3f, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0x67, 0x3f,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+0068 "h" */
    0xc1, 0x83, 0x7, 0xec, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x18,

    /* U+0069 "i" */
    0x30, 0xff, 0xff,

    /* U+006A "j" */
    0xc, 0x6, 0xdb, 0x6d, 0xb6, 0xde,

    /* U+006B "k" */
    0xc1, 0x83, 0x6, 0x7d, 0xdb, 0x3c, 0x78, 0xd9,
    0xbb, 0x38,

    /* U+006C "l" */
    0xff, 0xff, 0xfc,

    /* U+006D "m" */
    0xfd, 0xec, 0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63,

    /* U+006E "n" */
    0xfd, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xe3,

    /* U+006F "o" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,

    /* U+0070 "p" */
    0xfc, 0xe6, 0xc3, 0xc3, 0xc3, 0xc3, 0xe6, 0xfc,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3f, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0x67, 0x3f,
    0x3, 0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xff, 0x31, 0x8c, 0x63, 0x18,

    /* U+0073 "s" */
    0x7b, 0x3c, 0x3e, 0x7c, 0x3c, 0xde,

    /* U+0074 "t" */
    0x63, 0x3e, 0xc6, 0x31, 0x8c, 0x61, 0xc0,

    /* U+0075 "u" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xbf,

    /* U+0076 "v" */
    0xc3, 0x63, 0x66, 0x66, 0x36, 0x3c, 0x3c, 0x1c,

    /* U+0077 "w" */
    0xc6, 0x34, 0x66, 0x6f, 0x66, 0xf6, 0x6b, 0x42,
    0x94, 0x39, 0xc3, 0x9c,

    /* U+0078 "x" */
    0xe6, 0xdc, 0xf1, 0xc3, 0x87, 0x9b, 0xf3,

    /* U+0079 "y" */
    0xc3, 0x63, 0x66, 0x66, 0x36, 0x3c, 0x1c, 0x18,
    0x18, 0x18, 0x30, 0x30,

    /* U+007A "z" */
    0xfc, 0x71, 0x8c, 0x71, 0x8e, 0x3f,

    /* U+007B "{" */
    0x3b, 0x18, 0xc6, 0x31, 0x98, 0xc3, 0x18, 0xc6,
    0x31, 0x87,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xe1, 0x8c, 0x63, 0x18, 0xc3, 0x19, 0x8c, 0x63,
    0x18, 0xdc,

    /* U+007E "~" */
    0x73, 0x4e,

    /* U+00A0 " " */
    0x0,

    /* U+00A1 "¡" */
    0xcf, 0xff, 0xc0,

    /* U+00A2 "¢" */
    0x10, 0x71, 0xf7, 0x3c, 0x18, 0x39, 0xbe, 0x38,
    0x20,

    /* U+00A3 "£" */
    0x1e, 0x33, 0x63, 0x60, 0x60, 0x60, 0xfc, 0x60,
    0x60, 0x60, 0xff,

    /* U+00A4 "¤" */
    0x87, 0xfc, 0xf3, 0xcf, 0xf8, 0x0,

    /* U+00A5 "¥" */
    0x61, 0x9c, 0xe3, 0x30, 0xfc, 0x7f, 0x83, 0x0,
    0xc1, 0xfe, 0xc, 0x3, 0x0,

    /* U+00A6 "¦" */
    0xff, 0xc0, 0xff, 0xc0,

    /* U+00A7 "§" */
    0x7d, 0x8f, 0x3, 0x87, 0xdc, 0xf1, 0xe3, 0x7c,
    0x38, 0x1e, 0x37, 0xc0,

    /* U+00A8 "¨" */
    0xd8,

    /* U+00A9 "©" */
    0x1e, 0x18, 0x65, 0xea, 0xcd, 0xb0, 0x6c, 0xdb,
    0xe5, 0x72, 0x61, 0x87, 0x80,

    /* U+00AA "ª" */
    0x7f, 0x3c, 0xf3, 0x7c,

    /* U+00AB "«" */
    0x4b, 0x69, 0x36, 0x48,

    /* U+00AC "¬" */
    0xff, 0x80, 0xc0, 0x60,

    /* U+00AD "­" */
    0xfc,

    /* U+00AE "®" */
    0x7b, 0xfb, 0x6d, 0xcd, 0xe0,

    /* U+00AF "¯" */
    0xf8,

    /* U+00B0 "°" */
    0x7b, 0x3c, 0xf3, 0x78,

    /* U+00B1 "±" */
    0x18, 0x33, 0xf8, 0xc1, 0x83, 0x3f, 0x80,

    /* U+00B2 "²" */
    0xff, 0x34, 0xf0,

    /* U+00B3 "³" */
    0xfb, 0x6b, 0xf0,

    /* U+00B4 "´" */
    0xf, 0x0,

    /* U+00B5 "µ" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x78, 0xf1, 0xff, 0xc1,
    0x83, 0x6, 0x0,

    /* U+00B6 "¶" */
    0x3f, 0x7f, 0x7f, 0x7f, 0x7f, 0x3f, 0xf, 0xf,
    0xf, 0xf,

    /* U+00B7 "·" */
    0xc0,

    /* U+00B8 "¸" */
    0x61, 0xf0,

    /* U+00B9 "¹" */
    0xed, 0xb6,

    /* U+00BA "º" */
    0x7b, 0x2c, 0xf2, 0x78,

    /* U+00BB "»" */
    0xdb, 0x66, 0xf6, 0xd8,

    /* U+00BC "¼" */
    0xe3, 0x19, 0x86, 0x61, 0xb0, 0x6c, 0x6, 0xe1,
    0xb8, 0xd6, 0x77, 0xd8, 0x60,

    /* U+00BD "½" */
    0xe3, 0x18, 0x86, 0x61, 0xb0, 0x6c, 0x6, 0xe1,
    0xac, 0xc3, 0x31, 0x18, 0xf0,

    /* U+00BE "¾" */
    0xf1, 0x86, 0x61, 0x8c, 0x5b, 0xf, 0xe0, 0x1b,
    0x86, 0xf0, 0xd6, 0x37, 0xe6, 0x18,

    /* U+00BF "¿" */
    0x18, 0x0, 0x0, 0xce, 0x18, 0x33, 0x66, 0x78,

    /* U+00C0 "À" */
    0x0, 0xc, 0x3, 0x0, 0x1, 0xc0, 0xe0, 0x78,
    0x6c, 0x36, 0x1b, 0x98, 0xcf, 0xe6, 0x1e, 0xc,

    /* U+00C1 "Á" */
    0x0, 0x6, 0x6, 0x0, 0x1, 0xc0, 0xe0, 0x78,
    0x6c, 0x36, 0x1b, 0x98, 0xcf, 0xe6, 0x1e, 0xc,

    /* U+00C2 "Â" */
    0xc, 0x7, 0x80, 0x0, 0x30, 0x1e, 0x7, 0x81,
    0xe0, 0xcc, 0x33, 0xc, 0xc7, 0xf9, 0x86, 0x61,
    0x80,

    /* U+00C3 "Ã" */
    0x1e, 0x0, 0x0, 0xc0, 0x78, 0x1e, 0x7, 0x83,
    0x30, 0xcc, 0x33, 0x1f, 0xe6, 0x19, 0x86,

    /* U+00C4 "Ä" */
    0x36, 0x0, 0x7, 0x3, 0x83, 0xe1, 0xb0, 0xd8,
    0xc6, 0x63, 0x7f, 0xf0, 0x78, 0x30,

    /* U+00C5 "Å" */
    0x1c, 0xa, 0x7, 0x0, 0x1, 0xc0, 0xe0, 0x70,
    0x6c, 0x36, 0x1b, 0x18, 0xcf, 0xee, 0x36, 0xc,

    /* U+00C6 "Æ" */
    0x7, 0xf8, 0x3c, 0x3, 0x60, 0x1b, 0x1, 0x9f,
    0xc, 0xc0, 0xe6, 0x7, 0xf0, 0x61, 0x83, 0xf,
    0xc0,

    /* U+00C7 "Ç" */
    0x1e, 0x18, 0x66, 0x1f, 0x0, 0xc0, 0x30, 0xc,
    0x3, 0x87, 0x61, 0x8f, 0xc0, 0xc0, 0x38, 0x3,
    0x3, 0x80,

    /* U+00C8 "È" */
    0x1, 0xc3, 0x80, 0xff, 0xc, 0x30, 0xff, 0xc,
    0x30, 0xc3, 0xf0,

    /* U+00C9 "É" */
    0x8, 0xe6, 0x0, 0xff, 0xc, 0x30, 0xff, 0xc,
    0x30, 0xc3, 0xf0,

    /* U+00CA "Ê" */
    0x31, 0xe0, 0x3f, 0xc3, 0xc, 0x3f, 0xc3, 0xc,
    0x30, 0xfc,

    /* U+00CB "Ë" */
    0xd8, 0xf, 0xf0, 0xc3, 0xf, 0xf0, 0xc3, 0xc,
    0x3f,

    /* U+00CC "Ì" */
    0x8e, 0x30, 0x66, 0x66, 0x66, 0x66, 0x66,

    /* U+00CD "Í" */
    0x17, 0xc0, 0x66, 0x66, 0x66, 0x66, 0x66,

    /* U+00CE "Î" */
    0x6f, 0x6, 0x66, 0x66, 0x66, 0x66, 0x60,

    /* U+00CF "Ï" */
    0xd8, 0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xc0,

    /* U+00D0 "Ð" */
    0x7e, 0x18, 0xe6, 0x19, 0x83, 0x60, 0xfe, 0x36,
    0xd, 0x86, 0x63, 0x9f, 0x80,

    /* U+00D1 "Ñ" */
    0x3c, 0x0, 0xc3, 0xe3, 0xe3, 0xf3, 0xdb, 0xdb,
    0xcf, 0xc7, 0xc7, 0xc3,

    /* U+00D2 "Ò" */
    0x0, 0x6, 0x0, 0xe0, 0x8, 0x1e, 0x18, 0x66,
    0x1b, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0x86, 0x61,
    0x87, 0x80,

    /* U+00D3 "Ó" */
    0x2, 0x1, 0x81, 0xc0, 0x0, 0x1e, 0x18, 0x66,
    0x1b, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0x86, 0x61,
    0x87, 0x80,

    /* U+00D4 "Ô" */
    0xc, 0x7, 0x80, 0x0, 0x78, 0x61, 0x98, 0x6c,
    0xf, 0x3, 0xc0, 0xf0, 0x36, 0x19, 0x86, 0x1e,
    0x0,

    /* U+00D5 "Õ" */
    0x1b, 0x5, 0x80, 0x0, 0x78, 0x61, 0x98, 0x6c,
    0xf, 0x3, 0xc0, 0xf0, 0x36, 0x19, 0x86, 0x1e,
    0x0,

    /* U+00D6 "Ö" */
    0x1e, 0x0, 0x1, 0xe1, 0x86, 0x61, 0xb0, 0x3c,
    0xf, 0x3, 0xc0, 0xd8, 0x66, 0x18, 0x78,

    /* U+00D7 "×" */
    0x66, 0xf8, 0xe1, 0xe6, 0xe8, 0x80,

    /* U+00D8 "Ø" */
    0x1f, 0xd8, 0x66, 0x3b, 0x1b, 0xc4, 0xf2, 0x3d,
    0x8f, 0xc6, 0x61, 0x9f, 0x80,

    /* U+00D9 "Ù" */
    0x20, 0x38, 0x1c, 0x0, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,

    /* U+00DA "Ú" */
    0x4, 0x1c, 0x30, 0x0, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c,

    /* U+00DB "Û" */
    0x18, 0x3c, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0x66, 0x3c,

    /* U+00DC "Ü" */
    0x3c, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+00DD "Ý" */
    0x4, 0x1c, 0x30, 0x0, 0xc3, 0x67, 0x66, 0x3e,
    0x3c, 0x1c, 0x18, 0x18, 0x18, 0x18,

    /* U+00DE "Þ" */
    0xc1, 0x83, 0xf6, 0x3c, 0x78, 0xf1, 0xfe, 0xc1,
    0x80,

    /* U+00DF "ß" */
    0x0, 0x3e, 0x31, 0x98, 0xcc, 0xe6, 0xe3, 0x71,
    0x9e, 0xc7, 0xe0, 0xf6, 0x79, 0xe0,

    /* U+00E0 "à" */
    0x0, 0x18, 0xc, 0x0, 0x3f, 0x67, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x3f,

    /* U+00E1 "á" */
    0x0, 0xc, 0x18, 0x0, 0x3f, 0x67, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x3f,

    /* U+00E2 "â" */
    0x18, 0x3c, 0x0, 0x3f, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x67, 0x3f,

    /* U+00E3 "ã" */
    0x32, 0x2c, 0x0, 0x3f, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x67, 0x3f,

    /* U+00E4 "ä" */
    0x36, 0x0, 0x0, 0x3f, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x67, 0x3f,

    /* U+00E5 "å" */
    0x1c, 0x14, 0x1c, 0x0, 0x3f, 0x67, 0xc3, 0xc3,
    0xc3, 0xc3, 0x67, 0x3f,

    /* U+00E6 "æ" */
    0x3f, 0xf1, 0x9e, 0x6c, 0x30, 0xf0, 0xff, 0xc3,
    0x3, 0xc, 0x36, 0x79, 0x8f, 0xfc,

    /* U+00E7 "ç" */
    0x3c, 0xcf, 0x6, 0xc, 0x18, 0x39, 0xbe, 0x10,
    0x38, 0x11, 0xe0,

    /* U+00E8 "è" */
    0x0, 0x18, 0xc, 0x0, 0x3c, 0x66, 0x63, 0xff,
    0x40, 0x63, 0x66, 0x3c,

    /* U+00E9 "é" */
    0x0, 0xc, 0x18, 0x0, 0x3c, 0x66, 0x63, 0xff,
    0x40, 0x63, 0x66, 0x3c,

    /* U+00EA "ê" */
    0x18, 0x3c, 0x0, 0x3c, 0x66, 0xc3, 0xff, 0xc0,
    0xe3, 0x66, 0x3c,

    /* U+00EB "ë" */
    0x36, 0x0, 0x0, 0x3c, 0x66, 0x43, 0xff, 0x40,
    0x63, 0x66, 0x3c,

    /* U+00EC "ì" */
    0x8e, 0x30, 0x66, 0x66, 0x66, 0x66,

    /* U+00ED "í" */
    0x17, 0xc0, 0x66, 0x66, 0x66, 0x66,

    /* U+00EE "î" */
    0x6f, 0x6, 0x66, 0x66, 0x66, 0x60,

    /* U+00EF "ï" */
    0xd8, 0x0, 0xc6, 0x31, 0x8c, 0x63, 0x18,

    /* U+00F0 "ð" */
    0xe, 0x1c, 0x6, 0x3e, 0x67, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x3c,

    /* U+00F1 "ñ" */
    0x34, 0x98, 0x7, 0xec, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x18,

    /* U+00F2 "ò" */
    0x20, 0x38, 0xc, 0x0, 0x3c, 0x66, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+00F3 "ó" */
    0x4, 0x1c, 0x38, 0x0, 0x3c, 0x66, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+00F4 "ô" */
    0x18, 0x3c, 0x0, 0x3c, 0x66, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x3c,

    /* U+00F5 "õ" */
    0x32, 0x2c, 0x0, 0x3c, 0x66, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x3c,

    /* U+00F6 "ö" */
    0x3c, 0x0, 0x0, 0x3c, 0x66, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x3c,

    /* U+00F7 "÷" */
    0x18, 0x0, 0x7, 0xf0, 0x0, 0x0, 0xc,

    /* U+00F8 "ø" */
    0x3f, 0x66, 0xcf, 0xcb, 0xd3, 0xf3, 0xe6, 0x7c,

    /* U+00F9 "ù" */
    0x0, 0x60, 0x60, 0xc, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x1b, 0xf0,

    /* U+00FA "ú" */
    0x0, 0x30, 0xc0, 0xc, 0x78, 0xf1, 0xe3, 0xc7,
    0x8f, 0x1b, 0xf0,

    /* U+00FB "û" */
    0x18, 0x78, 0x6, 0x3c, 0x78, 0xf1, 0xe3, 0xc7,
    0x8d, 0xf8,

    /* U+00FC "ü" */
    0x6c, 0x0, 0x6, 0x3c, 0x78, 0xf1, 0xe3, 0xc7,
    0x8d, 0xf8,

    /* U+00FD "ý" */
    0x0, 0x18, 0x30, 0x0, 0xc6, 0xc6, 0xe6, 0x6c,
    0x6c, 0x3c, 0x38, 0x38, 0x30, 0x30, 0x30, 0x60,

    /* U+00FE "þ" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xe6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xe6, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+00FF "ÿ" */
    0x36, 0x0, 0x0, 0x18, 0x36, 0x33, 0x19, 0xd8,
    0x6c, 0x36, 0xe, 0x7, 0x3, 0x1, 0x81, 0x80,
    0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 0, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 53, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 53, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 79, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 82, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 8, .adv_w = 199, .box_w = 12, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 25, .adv_w = 146, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 36, .adv_w = 186, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 174, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 44, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 64, .adv_w = 109, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 74, .adv_w = 109, .box_w = 5, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 84, .adv_w = 116, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 88, .adv_w = 151, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 95, .adv_w = 57, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 97, .adv_w = 131, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 98, .adv_w = 58, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 109, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 109, .adv_w = 145, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 81, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 129, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 148, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 157, .adv_w = 144, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 143, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 123, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 144, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 197, .adv_w = 140, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 59, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 73, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 212, .adv_w = 131, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 217, .adv_w = 165, .box_w = 8, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 221, .adv_w = 127, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 226, .adv_w = 121, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 235, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 254, .adv_w = 160, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 144, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 276, .adv_w = 172, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 161, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 119, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 172, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 161, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 62, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 343, .adv_w = 128, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 351, .adv_w = 149, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 103, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 201, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 165, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 176, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 136, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 176, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 429, .adv_w = 144, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 438, .adv_w = 136, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 129, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 156, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 159, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 229, .box_w = 14, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 154, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 142, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 129, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 111, .box_w = 4, .box_h = 16, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 534, .adv_w = 169, .box_w = 6, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 546, .adv_w = 111, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 554, .adv_w = 152, .box_w = 9, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 563, .adv_w = 177, .box_w = 8, .box_h = 1, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 564, .adv_w = 61, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 566, .adv_w = 152, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 574, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 585, .adv_w = 135, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 592, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 603, .adv_w = 138, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 611, .adv_w = 77, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 617, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 629, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 639, .adv_w = 62, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 642, .adv_w = 62, .box_w = 3, .box_h = 16, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 648, .adv_w = 131, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 62, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 661, .adv_w = 235, .box_w = 12, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 673, .adv_w = 148, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 143, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 700, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 712, .adv_w = 90, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 717, .adv_w = 122, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 723, .adv_w = 87, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 730, .adv_w = 148, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 737, .adv_w = 134, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 745, .adv_w = 189, .box_w = 12, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 757, .adv_w = 121, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 136, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 776, .adv_w = 108, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 782, .adv_w = 114, .box_w = 5, .box_h = 16, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 792, .adv_w = 69, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 796, .adv_w = 114, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 806, .adv_w = 133, .box_w = 8, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 808, .adv_w = 53, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 809, .adv_w = 79, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 812, .adv_w = 149, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 821, .adv_w = 146, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 832, .adv_w = 126, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 838, .adv_w = 142, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 851, .adv_w = 78, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 855, .adv_w = 130, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 867, .adv_w = 76, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 868, .adv_w = 176, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 881, .adv_w = 103, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 885, .adv_w = 117, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 889, .adv_w = 153, .box_w = 9, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 893, .adv_w = 131, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 894, .adv_w = 116, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 899, .adv_w = 90, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 900, .adv_w = 99, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 904, .adv_w = 151, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 911, .adv_w = 81, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 914, .adv_w = 80, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 917, .adv_w = 56, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 919, .adv_w = 152, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 930, .adv_w = 149, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 940, .adv_w = 60, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 941, .adv_w = 66, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 943, .adv_w = 51, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 945, .adv_w = 101, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 949, .adv_w = 117, .box_w = 6, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 953, .adv_w = 164, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 966, .adv_w = 170, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 979, .adv_w = 182, .box_w = 11, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 993, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 1001, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1017, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1033, .adv_w = 160, .box_w = 10, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1050, .adv_w = 160, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1065, .adv_w = 160, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1079, .adv_w = 160, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1095, .adv_w = 210, .box_w = 13, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1112, .adv_w = 172, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1130, .adv_w = 119, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1141, .adv_w = 119, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1152, .adv_w = 119, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1162, .adv_w = 119, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1171, .adv_w = 62, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1178, .adv_w = 62, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1185, .adv_w = 62, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1192, .adv_w = 62, .box_w = 5, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1200, .adv_w = 163, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1213, .adv_w = 165, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1225, .adv_w = 176, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1243, .adv_w = 176, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1261, .adv_w = 176, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1278, .adv_w = 176, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1295, .adv_w = 176, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1310, .adv_w = 150, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 1316, .adv_w = 176, .box_w = 10, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1329, .adv_w = 156, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1343, .adv_w = 156, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1357, .adv_w = 156, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1370, .adv_w = 156, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1382, .adv_w = 142, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1396, .adv_w = 136, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1405, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1419, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1431, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1443, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1454, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1465, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1476, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1488, .adv_w = 241, .box_w = 14, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1502, .adv_w = 135, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1513, .adv_w = 138, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1525, .adv_w = 138, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1537, .adv_w = 138, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1548, .adv_w = 138, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1559, .adv_w = 65, .box_w = 4, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1565, .adv_w = 65, .box_w = 4, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1571, .adv_w = 65, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1577, .adv_w = 65, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1584, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1595, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1605, .adv_w = 143, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1617, .adv_w = 143, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1629, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1640, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1651, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1662, .adv_w = 145, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 1669, .adv_w = 142, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1677, .adv_w = 148, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1688, .adv_w = 148, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1699, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1709, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1719, .adv_w = 136, .box_w = 8, .box_h = 16, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1735, .adv_w = 152, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1750, .adv_w = 136, .box_w = 9, .box_h = 15, .ofs_x = 0, .ofs_y = -4}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 0, .range_length = 14, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 3,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 160, .range_length = 96, .glyph_id_start = 98,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 3,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

extern const lv_font_t lv_font_montserrat_14;


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t font_poppins_14 = {
#else
lv_font_t font_poppins_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = &lv_font_montserrat_14,
#endif
    .user_data = NULL,
};



#endif /*#if FONT_POPPINS_14*/

