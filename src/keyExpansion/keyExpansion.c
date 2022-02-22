#include <stdlib.h>

#include "keyExpansion.h"

void keyExpansion(enum KeySize keySize, uint8_t *key, uint32_t **expansion)
{
    int rounds;

    switch (keySize)
    {
    case AES128:
        rounds = 10;
        break;
    case AES192:
        rounds = 12;
        break;
    case AES256:
        rounds = 14;
        break;
    }

    *expansion = (uint32_t *)malloc(sizeof(uint32_t) * (rounds + 1) * 4);

    uint32_t rcon[] = {0x00000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                       0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

    // Set beginning of expansion as the key
    for (int i = 0; i < (keySize / 32); i++)
    {
        (*expansion)[i] = ((key[(i * 4) + 0] << 24) & (0xFF << 24)) | ((key[(i * 4) + 1] << 16) & (0xFF << 16)) |
                          ((key[(i * 4) + 2] << 8) & (0xFF << 8)) | (key[(i * 4) + 3] & 0xFF);
    }

    // Get other round keys
    for (int i = (keySize / 32); i < ((rounds + 1) * 4); i++)
    {
        uint32_t temp = (*expansion)[i - 1];

        if (i % (keySize / 32) == 0)
        {
            // RotWord
            temp = ((temp << 8) & 0xFFFFFF00) | ((temp >> 24) & 0xFF);

            // SubWord
            uint8_t b0 = sbox[(temp >> 24) & 0xFF];
            uint8_t b1 = sbox[(temp >> 16) & 0xFF];
            uint8_t b2 = sbox[(temp >> 8) & 0xFF];
            uint8_t b3 = sbox[(temp >> 0) & 0xFF];

            temp = ((b0 << 24) & (0xFF << 24)) | ((b1 << 16) & (0xFF << 16)) | ((b2 << 8) & (0xFF << 8)) | (b3 & 0xFF);

            // Rcon
            temp = temp ^ rcon[i / (keySize / 32)];
        }

        (*expansion)[i] = temp ^ (*expansion)[i - (keySize / 32)];
    }
}