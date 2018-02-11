#include <stdio.h>
#include <limits.h>

#include "jpeg.h"

#define TEST_JPG "../resources/test.jpg"

int main(int argc, char **argv)
{
    if (jpeg_open(TEST_JPG) != JPEG_SUCCESS)
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

    jpeg_close(TEST_JPG);

    return 0;
}
