#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <inttypes.h>

#include "jpeg.h"

static int _jpeg_has_marker(
    int const MARKER, long const FILE_OFFSET, int const FILE_ORIGIN);
static int _jpeg_handle_frame_header(int index, uint8_t id, jpeg_info_t *jpeg_info);
static bool _jpeg_is_frame_header(uint8_t marker);
static int _jpeg_handle_thumbnail(int index, jpeg_info_t *jpeg_info);
static void _jpeg_set_element(void *element, int size);

// TODO(g-mips): Maybe expand this to have an ever expanding array of files?
static FILE *jpeg_file = NULL;

int jpeg_open(char const *const path)
{
    jpeg_file = fopen(path, "rb");

    if (jpeg_file == NULL)
    {
        return JPEG_FILE_OPEN_ERR;
    }

    return JPEG_SUCCESS;
}

void jpeg_close()
{
    if (jpeg_file != NULL)
    {
        fclose(jpeg_file);
        jpeg_file = NULL;
    }
}

int jpeg_has_SOI()
{
    int error_code = _jpeg_has_marker(SOI, 0, SEEK_SET);

    if (error_code == JPEG_INVALID_MARKER_ERR)
    {
        error_code = JPEG_NO_SOI_ERR;
    }

    return error_code;
}

int jpeg_has_EOI()
{
    int error_code = _jpeg_has_marker(EOI, MARK_SIZE*-1, SEEK_END);

    if (error_code == JPEG_INVALID_MARKER_ERR)
    {
        error_code = JPEG_NO_SOI_ERR;
    }

    return error_code;
}

int jpeg_get_info()
{
    long size_of_file = 0;
    size_t count = 1;
    int error_code = JPEG_SUCCESS;

    if (jpeg_file == NULL)
    {
        error_code = JPEG_FILE_NOT_OPEN_ERR;
        goto jpeg_get_info_end;
    }

    if (fseek(jpeg_file, 0, SEEK_END) != 0)
    {
        error_code = JPEG_FILE_READ_ERR;
        goto jpeg_get_info_end;
    }

    size_of_file = ftell(jpeg_file);

    printf("SIZE_OF_FILE: %ld\n", size_of_file);

    rewind(jpeg_file);

    jpeg_has_SOI();
    jpeg_has_EOI();

    fseek(jpeg_file, MARK_SIZE, SEEK_SET);

    jpeg_info_t jpeg_info;
    jpeg_info.frame_headers = NULL;
    jpeg_info.num_fhs = 0;

    for (int i = MARK_SIZE; i < (size_of_file - MARK_SIZE); ++i)
    {
        uint8_t cur_byte = 0;
        fread(&cur_byte, sizeof(cur_byte), count, jpeg_file);

        if (cur_byte == BEG_MARK)
        {
            ++i;

            if (i >= size_of_file)
            {
                error_code = JPEG_FILE_EARLY_EOF;
                goto jpeg_get_info_end;
            }

            cur_byte = 0;
            fread(&cur_byte, sizeof(cur_byte), count, jpeg_file);

            if (cur_byte != BAD_MARK)
            {
                printf("MARKER: 0xFF 0x%"PRIX8"\n", cur_byte);
                if (cur_byte == SOI)
                {
                    i = _jpeg_handle_thumbnail(i, &jpeg_info);
                }
                else if (_jpeg_is_frame_header(cur_byte))
                {
                    i = _jpeg_handle_frame_header(i, cur_byte, &jpeg_info);
                }
            }
        }
    }

    printf("NUM_FHS: %d\n", jpeg_info.num_fhs);

    for (int i = 0; i < jpeg_info.num_fhs; ++i)
    {
        printf("\n\nFRAME_HEADER TYPE: 0x%X\n",
               jpeg_info.frame_headers[i].type);
        printf("FRAME_HEADER LENGTH: 0x%"PRIX16"\n",
               jpeg_info.frame_headers[i].length);
        printf("FRAME_HEADER SAMPLE_PRECISION: 0x%"PRIX8"\n",
               jpeg_info.frame_headers[i].sample_precision);
        printf("FRAME_HEADER NUM_LINES: 0x%"PRIX16"\n",
               jpeg_info.frame_headers[i].num_lines);
        printf("FRAME_HEADER SAMPLES_PER_LINE: 0x%"PRIX16"\n",
               jpeg_info.frame_headers[i].samples_per_line);
        printf("FRAME_HEADER NUM_COMPS: 0x%"PRIX8"\n",
               jpeg_info.frame_headers[i].num_comps);

        for (int j = 0; j < jpeg_info.frame_headers[i].num_comps; ++j)
        {
            printf("\n\n\tFRAME_HEADER COMPS[%d].ID: 0x%"PRIX8"\n", j,
                   jpeg_info.frame_headers[i].comps[j].id);
            printf("\tFRAME_HEADER COMPS[%d].SAMPLING_FACTORS: 0x%"PRIX8"\n", j,
                   jpeg_info.frame_headers[i].comps[j].sampling_factors);
            printf("\tFRAME_HEADER COMPS[%d].QUANT_TABLE_DEST_SELECTOR: 0x%"PRIX8"\n", j,
                   jpeg_info.frame_headers[i].comps[j].quant_table_dest_selector);
        }
    }

jpeg_get_info_end:
    return error_code;
}

static int _jpeg_handle_frame_header(int index, uint8_t id, jpeg_info_t *jpeg_info)
{
    if (jpeg_info == NULL)
    {
        // TODO(Grant): Error?
        return index;
    }

    frame_header_t *tmp_frame_headers = (frame_header_t *)realloc(
        jpeg_info->frame_headers,
        sizeof(*jpeg_info->frame_headers) * jpeg_info->num_fhs);

    if (tmp_frame_headers == NULL)
    {
        // TODO(Grant): Alloc error. free? or no?
        return index;
    }
    jpeg_info->frame_headers = tmp_frame_headers;
    tmp_frame_headers = NULL;

    jpeg_info->num_fhs++;

    frame_header_t *fh = &jpeg_info->frame_headers[jpeg_info->num_fhs-1];

    _jpeg_set_element((void *)&fh->length, sizeof(fh->length));
    index += sizeof(fh->length);

    _jpeg_set_element((void *)&fh->sample_precision, sizeof(fh->sample_precision));
    index += sizeof(fh->sample_precision);

    _jpeg_set_element((void *)&fh->num_lines, sizeof(fh->num_lines));
    index += sizeof(fh->num_lines);

    _jpeg_set_element((void *)&fh->samples_per_line, sizeof(fh->samples_per_line));
    index += sizeof(fh->samples_per_line);

    _jpeg_set_element((void *)&fh->num_comps, sizeof(fh->num_comps));
    index += sizeof(fh->num_comps);

    fh->comps = calloc(fh->num_comps, sizeof(component_t));

    if (fh->comps == NULL)
    {
        // TODO(Grant): Alloc error.
        return index;
    }

    for (int i = 0; i < fh->num_comps; ++i)
    {
        _jpeg_set_element((void *)&fh->comps[i].id, sizeof(fh->comps[i].id));
        index += sizeof(fh->comps[i].id);

        _jpeg_set_element((void *)&fh->comps[i].sampling_factors,
                          sizeof(fh->comps[i].sampling_factors));
        index += sizeof(fh->comps[i].sampling_factors);

        _jpeg_set_element((void *)&fh->comps[i].quant_table_dest_selector,
                          sizeof(fh->comps[i].quant_table_dest_selector));
        index += sizeof(fh->comps[i].quant_table_dest_selector);
    }

    return index;
}

static void _jpeg_set_element(void *element, int size)
{
    int count = 1;
    uint8_t byte = 0;
    uint8_t *ele8 = NULL;
    uint16_t *ele16 = NULL;
    uint32_t *ele32 = NULL;
    uint64_t *ele64 = NULL;

    if (size == sizeof(*ele8))
    {
        ele8 = (uint8_t *)element;
        *ele8 = 0;
    }
    else if  (size == sizeof(*ele16))
    {
        ele16 = (uint16_t *)element;
        *ele16 = 0;
    }
    else if (size == sizeof(*ele32))
    {
        ele32 = (uint32_t *)element;
        *ele32 = 0;
    }
    else if (size == sizeof(*ele64))
    {
        ele64 = (uint64_t *)element;
        *ele64 = 0;
    }
    else
    {
        // TODO(Grant): Error?
        return;
    }

    for (int i = size - 1; i >= 0; --i)
    {
        byte = 0;
        fread(&byte, sizeof(byte), count, jpeg_file);

        if (size == sizeof(*ele8))
        {
            *ele8 |= byte << (i * 8);
        }
        else if  (size == sizeof(*ele16))
        {
            *ele16 |= byte << (i * 8);
        }
        else if (size == sizeof(*ele32))
        {
            *ele32 |= byte << (i * 8);
        }
        else if (size == sizeof(*ele64))
        {
            *ele64 |= byte << (i * 8);
        }
    }
}

static bool _jpeg_is_frame_header(uint8_t marker)
{
    bool is_SOFX = false;

    switch (marker)
    {
    case SOF0:
    case SOF1:
    case SOF2:
    case SOF3:
    case SOF5:
    case SOF6:
    case SOF7:
    case SOF9:
    case SOF10:
    case SOF11:
    case SOF13:
    case SOF14:
    case SOF15:
        is_SOFX = true;
        break;
    }

    return is_SOFX;
}

static int _jpeg_handle_thumbnail(int index, jpeg_info_t *jpeg_info)
{
    int count = 1;
    uint8_t cur_byte = 0;

    do
    {
        fread(&cur_byte, sizeof(cur_byte), count, jpeg_file);
        index++;

        if (cur_byte == BEG_MARK)
        {
            cur_byte = 0;
            fread(&cur_byte, sizeof(cur_byte), count, jpeg_file);

            if (cur_byte != BAD_MARK)
            {
            }
        }
    }
    while (cur_byte != EOI);

    return index;
}

static int _jpeg_has_marker(
    int const MARKER, long const FILE_OFFSET, int const FILE_ORIGIN)
{
    uint8_t buffer[MARK_SIZE];
    size_t num_read = 0;
    int error_code = JPEG_SUCCESS;

    if (jpeg_file == NULL)
    {
        error_code = JPEG_FILE_NOT_OPEN_ERR;
        goto _jpeg_has_marker_end;
    }

    // This is done with a seek as well, so in order to reset end-of-file and
    // error indicators
    rewind(jpeg_file);

    if (fseek(jpeg_file, FILE_OFFSET, FILE_ORIGIN))
    {
        error_code = JPEG_FILE_READ_ERR;
        goto _jpeg_has_marker_end;
    }

    num_read = fread(buffer, sizeof(*buffer), MARK_SIZE, jpeg_file);

    if (num_read != MARK_SIZE)
    {
        error_code = JPEG_FILE_READ_ERR;
        goto _jpeg_has_marker_end;
    }

    if (buffer[0] != BEG_MARK || buffer[1] != MARKER)
    {
        error_code = JPEG_INVALID_MARKER_ERR;
    }

_jpeg_has_marker_end:
    if (error_code != JPEG_FILE_NOT_OPEN_ERR)
    {
        rewind(jpeg_file);
    }

    return error_code;
}
