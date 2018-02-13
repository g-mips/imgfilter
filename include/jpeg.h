#pragma once

static inline bool res(uint8_t marker, uint8_t beg_res, uint8_t end_res)
{
    for (; beg_res < end_res; ++beg_res)
    {
        if (marker == beg_res)
        {
            return true;
        }
    }

    return false;
}

// Marker information
#define MARK_SIZE 0x02
#define BEG_MARK  0xFF // This goes in front of all other markers

// Invalid marker
#define BAD_MARK  0x00

// Temporary private use in arithmetic coding
#define TEM       0x01

// Reserved
#define RES(x)    res(x, 0x02, 0xBF)

// Start of Frames
#define SOF0      0xC0 // Non-differential Baseline DCT, Huffman coding
#define SOF1      0xC1 // Non-differential Extended Sequential DCT, Huffman coding
#define SOF2      0xC2 // Non-differential Progressive DCT, Huffman coding
#define SOF3      0xC3 // Non-differential Lossless (Sequential), Huffman coding
#define DHT       0xC4 // Define Huffman Table
#define SOF5      0xC5 // Differential Sequential DCT, Huffman coding
#define SOF6      0xC6 // Differential Progressive DCT, Huffman coding
#define SOF7      0xC7 // Differential Lossless (Sequential), Huffman coding
#define SOF9      0xC9 // Non-differential Extended Sequential DCT, Arithmetic coding
#define SOF10     0xCA // Non-differential Progressive DCT, Arithmetic coding
#define SOF11     0xCB // Non-differential Lossless (Sequential), Arithmetic coding
#define DAC       0xCC // Define Arithmetic Coding Conditioning(s)
#define SOF13     0xCD // Differential Sequential DCT, Arithmetic coding
#define SOF14     0xCE // Differential Progressive DCT, Arithmetic coding
#define SOF15     0xCF // Differential Lossless (Sequential), Arithmetic coding

// Restart Interval Termination
#define RST0      0xD0
#define RST1      0xD1
#define RST2      0xD2
#define RST3      0xD3
#define RST4      0xD4
#define RST5      0xD5
#define RST6      0xD6
#define RST7      0xD7

// Start and End of Image Markers
#define SOI       0xD8
#define EOI       0xD9

// Start of Scan
#define SOS       0xDA

// Define Quantization Table
#define DQT       0xDB

// Define Number of Lines
#define DNL       0xDC

// Define Restart Interval
#define DRI       0xDD

// Define Hierarchical Progression
#define DHP       0xDE

// Expand Reference Component(s)
#define EXP       0xDF

// Application Markers
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

// JPEG Extensions
#define JPG0      0xF0
#define JPG1      0xF1
#define JPG2      0xF2
#define JPG3      0xF3
#define JPG4      0xF4
#define JPG5      0xF5
#define JPG6      0xF6
#define JPG7      0xF7
#define JPG8      0xF8
#define JPG9      0xF9
#define JPG10     0xFA
#define JPG11     0xFB
#define JPG12     0xFC
#define JPG13     0xFD

// Comment
#define COM       0xFE

// JFIF
#define JFIF_IDENTIFIER     "JFIF\0" // 4A 46 49 46 00
#define JFIF_UNITS_NO_UNITS 0
#define JFIF_UNITS_PER_INCH 1
#define JFIF_UNITS_PER_CM   2

// Huffman Table Types
#define DC_TABLE 0
#define AC_TABLE 1

#define MAX_THUMBS 2
#define MAX_COMPS  255

typedef enum jpeg_error_codes
{
    JPEG_SUCCESS = 0,
    JPEG_FILE_READ_ERR = INT_MIN,
    JPEG_FILE_OPEN_ERR,
    JPEG_FILE_NOT_OPEN_ERR,
    JPEG_FILE_EARLY_EOF,
    JPEG_INVALID_MARKER_ERR,
    JPEG_NO_SOI_ERR,
    JPEG_NO_EOI_ERR
} jpeg_error_codes_t;

typedef struct component
{
    uint8_t id;
    uint8_t sampling_factors; // Low 4 bits --> Horiz. High 4 bits --> Vert.
    uint8_t quant_table_dest_selector;
} component_t;

typedef struct frame_header
{
    int type;
    uint16_t length;
    uint8_t sample_precision;
    uint16_t num_lines;
    uint16_t samples_per_line;
    uint8_t num_comps;
    component_t *comps;
} frame_header_t;

typedef struct jpeg_info
{
    struct jpeg_info *thumbnails[MAX_THUMBS];
    int num_fhs;
    frame_header_t *frame_headers;
} jpeg_info_t;

int jpeg_open(char const *const path);
void jpeg_close();
int jpeg_has_SOI();
int jpeg_has_EOI();
int jpeg_get_info();
