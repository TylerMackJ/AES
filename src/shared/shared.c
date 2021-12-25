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

uint8_t *get_sbox()
{
    static uint8_t initialized = 0;
    static uint8_t *sbox;

    // Only initialize sbox on first call to function
    if (initialized == 0)
    {
        initialized = 1;

        sbox = (uint8_t *)malloc(sizeof(uint8_t) * 256);

        uint8_t p = 1, q = 1;

        /* loop invariant: p * q == 1 in the Galois field */
        do
        {
            /* multiply p by 3 */
            p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

            /* divide q by 3 (equals multiplication by 0xf6) */
            q ^= q << 1;
            q ^= q << 2;
            q ^= q << 4;
            q ^= q & 0x80 ? 0x09 : 0;

            /* compute the affine transformation */
            uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

            sbox[p] = xformed ^ 0x63;
        } while (p != 1);

        /* 0 is a special case since it has no inverse */
        sbox[0] = 0x63;
    }

    return sbox;
}