#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "jpeg.h"

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
    uint8_t buffer[MARK_SIZE];
    size_t num_read = 0;
    int error_code = JPEG_SUCCESS;

    rewind(jpeg_file);

    num_read = fread(buffer, sizeof *buffer, MARK_SIZE, jpeg_file);

    if (num_read != MARK_SIZE)
    {
        error_code = JPEG_FILE_READ_ERR;
    }
    else if (buffer[0] != BEG_MARK || buffer[1] != SOI)
    {
        error_code = JPEG_NO_SOI_ERR;
    }

    return error_code;
}

int jpeg_has_EOI()
{
    uint8_t buffer[MARK_SIZE];
    size_t num_read = 0;
    int error_code = JPEG_SUCCESS;

    fseek(jpeg_file, MARK_SIZE*-1, SEEK_END);

    num_read = fread(buffer, sizeof *buffer, MARK_SIZE, jpeg_file);

    if (num_read != MARK_SIZE)
    {
        error_code = JPEG_FILE_READ_ERR;
    }
    else if (buffer[0] != BEG_MARK || buffer[1] != EOI)
    {
        error_code = JPEG_NO_EOI_ERR;
    }

    return error_code;
}
