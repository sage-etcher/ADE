unsigned char adv_kbd_scancodes[256][7] = {
/*

 hkc   hardware keycode - PC            = 0x26
 lock  capslock, numlock, none          = 0x01 (Can use CAPSLOCK)
 just  just the key itself     e.g. 'a' = 0x61
 shift the key when shifted         'A' = 0x41
 ctrl  the key when control        '^A' = 0x01
 C+s   both control and shift      '^A' = 0x01
 ahkc  advantage hardware keycode       =  38 (dec.)

 value 0xFF signifies NO output

*/

/* hkc     lock  just  shift ctrl  c+s   cmd   ahkc */
/* 00 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },  //unused - zero-based index
/* 01 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 02 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 03 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 04 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 05 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 06 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 07 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 08 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 09 */  { 0x00, 0x1B, 0x1B, 0x1B, 0x1B, 0xFF, 19 },  // ESC
/* 0A */  { 0x00, 0x31, 0x21, 0x31, 0x21, 0xFF,  3 },  // 1  !
/* 0B */  { 0x00, 0x32, 0x40, 0x32, 0x00, 0xFF,  4 },  // 2  @
/* 0C */  { 0x00, 0x33, 0x23, 0x33, 0x23, 0xFF,  5 },  // 3  #
/* 0D */  { 0x00, 0x34, 0x24, 0x34, 0x24, 0xFF,  6 },  // 4  \$
/* 0E */  { 0x00, 0x35, 0x25, 0x35, 0x25, 0xFF,  7 },  // 5  \%
/* 0F */  { 0x00, 0x36, 0x5E, 0x36, 0x1E, 0xFF,  8 },  // 6  ^
/* 10 */  { 0x00, 0x37, 0x26, 0x37, 0x26, 0xFF,  9 },  // 7  &
/* 11 */  { 0x00, 0x38, 0x2A, 0x38, 0x2A, 0xFF, 10 },  // 8  *
/* 12 */  { 0x00, 0x39, 0x28, 0x39, 0x28, 0xFF, 11 },  // 9  (
/* 13 */  { 0x00, 0x30, 0x29, 0x30, 0x29, 0xFF, 12 },  // 0  )
/* 14 */  { 0x00, 0x2D, 0x5F, 0x2D, 0x1F, 0xFF, 13 },  // -  _
/* 15 */  { 0x00, 0x3D, 0x2B, 0x3D, 0x2B, 0xFF, 14 },  // =  +
/* 16 */  { 0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 15 },  // DEL
/* 17 */  { 0x00, 0x09, 0x09, 0x09, 0x09, 0xFF, 20 },  // TAB
/* 18 */  { 0x01, 0x71, 0x51, 0x11, 0x11, 0xD1, 21 },  // Q
/* 19 */  { 0x01, 0x77, 0x57, 0x17, 0x17, 0xD7, 22 },  // W
/* 1A */  { 0x01, 0x65, 0x45, 0x05, 0x05, 0xc5, 23 },  // E
/* 1B */  { 0x01, 0x72, 0x52, 0x12, 0x12, 0xD2, 24 },  // R
/* 1C */  { 0x01, 0x74, 0x54, 0x14, 0x14, 0xD4, 25 },  // T
/* 1D */  { 0x01, 0x79, 0x59, 0x19, 0x19, 0xD9, 26 },  // Y
/* 1E */  { 0x01, 0x75, 0x55, 0x15, 0x15, 0xD5, 27 },  // U
/* 1F */  { 0x01, 0x69, 0x49, 0x09, 0x09, 0xc9, 28 },  // I
/* 20 */  { 0x01, 0x6F, 0x4F, 0x0F, 0x0F, 0xCF, 29 },  // O
/* 21 */  { 0x01, 0x70, 0x50, 0x10, 0x10, 0xD0, 30 },  // P
/* 22 */  { 0x00, 0x5B, 0x7B, 0x1B, 0x7B, 0xFF, 31 },  // [  {
/* 23 */  { 0x00, 0x5D, 0x7D, 0x1D, 0x7D, 0xFF, 32 },  // ]  }
/* 24 */  { 0x00, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 49 },  // RETURN
/* 25 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 37 },  // CTRL L
/* 26 */  { 0x01, 0x61, 0x41, 0x01, 0x01, 0xc1, 38 },  // A
/* 27 */  { 0x01, 0x73, 0x53, 0x13, 0x13, 0xD3, 39 },  // S
/* 28 */  { 0x01, 0x64, 0x44, 0x04, 0x04, 0xc4, 40 },  // D
/* 29 */  { 0x01, 0x66, 0x46, 0x06, 0x06, 0xc6, 41 },  // F
/* 2A */  { 0x01, 0x67, 0x47, 0x07, 0x07, 0xc7, 42 },  // G
/* 2B */  { 0x01, 0x68, 0x48, 0x08, 0x08, 0xc8, 43 },  // H
/* 2C */  { 0x01, 0x6A, 0x4A, 0x0A, 0x0A, 0xCA, 44 },  // J
/* 2D */  { 0x01, 0x6B, 0x4B, 0x0B, 0x0B, 0xCB, 45 },  // K
/* 2E */  { 0x01, 0x6C, 0x4C, 0x0C, 0x0C, 0xCC, 46 },  // L
/* 2F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 30 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 31 */  { 0x00, 0x60, 0x7e, 0x60, 0x7e, 0xff, 01 },  // ` ~   ***
/* 32 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 54 },  // SHIFT L
/* 33 */  { 0x00, 0x5C, 0x7C, 0x1C, 0x7C, 0xFF, 02 },  // \  |  ***
/* 34 */  { 0x00, 0x7A, 0x5A, 0x1A, 0x1A, 0xDA, 55 },
/* 35 */  { 0x01, 0x78, 0x58, 0x18, 0x18, 0xD8, 56 },  // X
/* 36 */  { 0x01, 0x63, 0x43, 0x03, 0x03, 0xc3, 57 },  // C
/* 37 */  { 0x01, 0x76, 0x56, 0x16, 0x16, 0xD6, 58 },  // V
/* 38 */  { 0x01, 0x62, 0x42, 0x02, 0x02, 0xc2, 59 },  // B
/* 39 */  { 0x01, 0x6E, 0x4E, 0x0E, 0x0E, 0xCE, 60 },  // N
/* 3A */  { 0x01, 0x6D, 0x4D, 0x0D, 0x0D, 0xCD, 61 },  // M
/* 3B */  { 0x00, 0x2C, 0x3C, 0x2C, 0x3C, 0xFF, 62 },  // ,  <
/* 3C */  { 0x00, 0x2E, 0x3E, 0x2E, 0x3E, 0xFF, 63 },  // .  >
/* 3D */  { 0x00, 0x2F, 0x3F, 0x2F, 0x3F, 0xFF, 64 },  // /  ?
/* 3E */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 65 },  // SHIFT R
/* 3F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 40 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 36 },  // CMD=ALT
/* 41 */  { 0x00, 0x20, 0x20, 0x20, 0x20, 0xFF, 69 },  // SPACE
/* 42 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 53 },  // CAPSLOCK
/* 43 */  { 0x00, 0xDB, 0xEA, 0xDB, 0xEA, 0x9B, 70 },  // F1
/* 44 */  { 0x00, 0xDC, 0xEB, 0xDC, 0xEB, 0x9C, 71 },  // F2
/* 45 */  { 0x00, 0xDD, 0xEC, 0xDD, 0xEC, 0x9D, 72 },  // F3
/* 46 */  { 0x00, 0xDE, 0xED, 0xDE, 0xED, 0x9E, 73 },  // F4
/* 47 */  { 0x00, 0xDF, 0xEE, 0xDF, 0xEE, 0x9F, 74 },  // F5
/* 48 */  { 0x00, 0xE0, 0xEF, 0xE0, 0xEF, 0xA0, 75 },  // F6
/* 49 */  { 0x00, 0xE1, 0xF0, 0xE1, 0xF0, 0xA1, 76 },  // F7
/* 4A */  { 0x00, 0xE2, 0xF1, 0xE2, 0xF1, 0xA2, 77 },  // F8
/* 4B */  { 0x00, 0xE3, 0xF2, 0xE3, 0xF2, 0xA3, 78 },  // F9
/* 4C */  { 0x00, 0xE4, 0xF3, 0xE4, 0xF3, 0xA4, 79 },  // F10
/* 4D */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 87 },  // NUMLOCK (Cursor Lock)
/* 4E */  { 0x00, 0xE8, 0xF7, 0xE8, 0xF7, 0xA8, 83 },  // F14
/* 4F */  { 0x02, 0x37, 0x87, 0xB7, 0x97, 0xBC, 16 },  // K-7
/* 50 */  { 0x02, 0x38, 0x82, 0xB8, 0x98, 0xBD, 17 },  // K-8
/* 51 */  { 0x02, 0x39, 0x89, 0xB9, 0x99, 0xBE, 18 },  // K-9
/* 52 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 53 */  { 0x02, 0x34, 0x88, 0xB4, 0x94, 0xFD, 33 },  // K-4
/* 54 */  { 0x02, 0x35, 0x85, 0xB5, 0x95, 0xBA, 34 },  // K-5
/* 55 */  { 0x02, 0x36, 0x86, 0xB6, 0x96, 0xBB, 35 },  // K-6
/* 56 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0 },  // K-+
/* 57 */  { 0x02, 0x31, 0x84, 0xB1, 0x91, 0xFA, 50 },  // K-1
/* 58 */  { 0x02, 0x32, 0x8A, 0xB2, 0x92, 0xFB, 51 },  // K-2
/* 59 */  { 0x02, 0x33, 0x83, 0xB3, 0x93, 0xFC, 52 },  // K-3
/* 5A */  { 0x02, 0x30, 0xB0, 0x90, 0x90, 0xC0, 66 },  // K-0
/* 5B */  { 0x02, 0x2E, 0xAE, 0x8E, 0x8E, 0x81, 67 },  // K-.
/* 5C */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 5D */  { 0x00, 0x5D, 0x7D, 0x1D, 0x7D, 0xFF,  0 },
/* 5E */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 5F */  { 0x00, 0xE5, 0xF4, 0xE5, 0xF4, 0xA5, 80 },  // F11
/* 60 */  { 0x00, 0xE6, 0xF5, 0xE6, 0xF5, 0xA6, 81 },  // F12
/* 61 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 62 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 63 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 64 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 65 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 66 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 67 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 68 */  { 0x00, 0x0D, 0x8D, 0x9A, 0x9A, 0xAA, 68 },
/* 69 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0 },  // CTRL R
/* 6A */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 6B */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 6C */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0 },  // ALT R
/* 6D */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 6E */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 6F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 70 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 71 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 72 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 73 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 74 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 75 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 76 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 77 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 78 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 79 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7A */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7B */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7C */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7D */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7E */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 7F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 80 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 81 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 82 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 83 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 84 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 85 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 86 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0 },  // WIN R
/* 87 */  { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  0 },  // PAGE R
/* 88 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 89 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8A */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8B */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8C */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8D */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8E */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 8F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 90 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 91 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 92 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 93 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 94 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 95 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 96 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 97 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 98 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 99 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9A */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9B */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9C */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9D */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9E */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* 9F */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* A9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AD */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* AF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* B9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BD */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* BF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* C9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CD */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* CF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* D9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DD */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* DF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* E9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* EA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* EB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* EC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* ED */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* EE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* EF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F0 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F1 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F2 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F3 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F4 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F5 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F6 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F7 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F8 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* F9 */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FA */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FB */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FC */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FD */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FE */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 },
/* FF */  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0 }
};
