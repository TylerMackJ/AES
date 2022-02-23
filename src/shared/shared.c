#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

#if RAND_MAX/256 >= 0xFFFFFFFFFFFFFF
  #define LOOP_COUNT 1
#elif RAND_MAX/256 >= 0xFFFFFF
  #define LOOP_COUNT 2
#elif RAND_MAX/256 >= 0x3FFFF
  #define LOOP_COUNT 3
#elif RAND_MAX/256 >= 0x1FF
  #define LOOP_COUNT 4
#else
  #define LOOP_COUNT 5
#endif

uint64_t rand_uint64(void) {
    time_t t;
    srand((unsigned) time(&t));
    uint64_t r = 0;
    for (int i=LOOP_COUNT; i > 0; i--) {
        r = r*(RAND_MAX + (uint64_t)1) + rand();
    }
    return r;
}