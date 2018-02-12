#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "jpeg.h"

static int _jpeg_has_marker(
    int const MARKER, long const FILE_OFFSET, int const FILE_ORIGIN);

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
    fseek(jpeg_file, FILE_OFFSET, FILE_ORIGIN);

    num_read = fread(buffer, sizeof *buffer, MARK_SIZE, jpeg_file);

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
    return error_code;
}
