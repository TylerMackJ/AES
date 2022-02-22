#include <stdio.h>
#include <stdlib.h>

#include "shared.h"

#define ROTL8(x, shift) ((uint8_t)((x) << (shift)) | ((x) >> (8 - (shift))))

void printLength(uint8_t *data, int length)
{
    printf("%02d Bytes: ", length);
    for (int i = 0; i < length; i++)
    {
        printf("%02x ", (int)((uint8_t *)data)[i]);
    }
    printf("\n");
}
