#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <string.h>

#include "jpeg.h"

#define TEST_JPG "../resources/test.jpg"

int main(int argc, char **argv)
{
    char file[256] = {'\0'};

    if (argc > 1)
    {
        strncpy(file, argv[1], strlen(argv[1]));
    }
    else
    {
        strncpy(file, TEST_JPG, sizeof(TEST_JPG));
    }

    if (jpeg_open(file) != JPEG_SUCCESS)
    {
        printf("Error opening\n");
        return -1;
    }

    if (jpeg_has_SOI() == JPEG_SUCCESS)
    {
        printf("JPEG has SOI\n");
    }

    if (jpeg_has_EOI() == JPEG_SUCCESS)
    {
        printf("JPEG has EOI\n");
    }

    jpeg_get_info();

    jpeg_close();

    return 0;
}
