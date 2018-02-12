#pragma once

// Markers
#define MARK_SIZE 0x02
#define BEG_MARK  0xFF // This goes in front of all other markers

#define SOI       0xD8 // Start of Image
#define EOI       0xD9 // End of Image

#define APP0      0xE0 // JFIF
#define APP1      0xE1
#define APP2      0xE2
#define APP3      0xE3
#define APP4      0xE4
#define APP5      0xE5
#define APP6      0xE6
#define APP7      0xE7
#define APP8      0xE8
#define APP9      0xE9
#define APP10     0xEA
#define APP11     0xEB
#define APP12     0xEC
#define APP13     0xED
#define APP14     0xEE
#define APP15     0xEF

#define SOF0      0xC0 // Start of Frame (baseline DCT)
#define SOF1      0xC1
#define SOF2      0xC2 // Start of Frame (progressive DCT)
#define SOF3      0xC3
#define SOF5      0xC5
#define SOF6      0xC6
#define SOF7      0xC7
#define SOF9      0xC9
#define SOF10     0xCA
#define SOF11     0xCB
#define SOF13     0xCC
#define SOF14     0xCD
#define SOF15     0xCE

#define DHT       0xC4 // Define Huffman Table
#define DQT       0xDB // Define Quantization Table
#define DAC       0xCC // Define Arithmetic Table

#define SOS       0xDA // Start of Scan

// JFIF
#define JFIF_IDENTIFIER     "JFIF\0" // 4A 46 49 46 00
#define JFIF_UNITS_NO_UNITS 0
#define JFIF_UNITS_PER_INCH 1
#define JFIF_UNITS_PER_CM   2

// Huffman Table Types
#define DC_TABLE 0
#define AC_TABLE 1

enum jpeg_error_codes
{
    JPEG_SUCCESS = 0,
    JPEG_FILE_READ_ERR = INT_MIN,
    JPEG_FILE_OPEN_ERR,
    JPEG_FILE_NOT_OPEN_ERR,
    JPEG_INVALID_MARKER_ERR,
    JPEG_NO_SOI_ERR,
    JPEG_NO_EOI_ERR
};

int jpeg_open(char const *const path);
void jpeg_close();
int jpeg_has_SOI();
int jpeg_has_EOI();
