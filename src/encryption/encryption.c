#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../shared/shared.h"
#include "encryption.h"

void padBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength)
{
    *paddedLength = length;

    // Pad data out to 128 bit chunks
    if (*paddedLength % 16 != 0)
    {
        // If not already divisable by 16 then add an additional chunk
        *paddedLength = ((*paddedLength / 16) + 1) * 16;
    }

    // Move bytes into padded byte buffer
    *paddedBytes = (uint8_t *)malloc(sizeof(uint8_t) * *paddedLength);
    for (int i = 0; i < length; i++)
    {
        (*paddedBytes)[i] = ((char *)bytes)[i];
    }

    // Assure 0's for padded data
    for (int i = length; i < *paddedLength; i++)
    {
        (*paddedBytes)[i] = '\0';
    }
}

void keyExpansion(enum KeySize keySize, uint8_t *key, uint32_t **expansion)
{
    uint8_t *sbox = get_sbox();

    if (keySize == AES128)
    {
        *expansion = (uint32_t *)malloc(sizeof(uint32_t) * 44);

        uint32_t rcon[] = {0x00000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                           0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

        // Set first round key = K
        (*expansion)[0] = ((key[0] << 24) & (0xFF << 24)) | ((key[1] << 16) & (0xFF << 16)) |
                          ((key[2] << 8) & (0xFF << 8)) | (key[3] & 0xFF);
        (*expansion)[1] = ((key[4] << 24) & (0xFF << 24)) | ((key[5] << 16) & (0xFF << 16)) |
                          ((key[6] << 8) & (0xFF << 8)) | (key[7] & 0xFF);
        (*expansion)[2] = ((key[8] << 24) & (0xFF << 24)) | ((key[9] << 16) & (0xFF << 16)) |
                          ((key[10] << 8) & (0xFF << 8)) | (key[11] & 0xFF);
        (*expansion)[3] = ((key[12] << 24) & (0xFF << 24)) | ((key[13] << 16) & (0xFF << 16)) |
                          ((key[14] << 8) & (0xFF << 8)) | (key[15] & 0xFF);

        // Get other round keys
        for (int i = 4; i < 44; i++)
        {
            uint32_t temp = (*expansion)[i - 1];
            if (i % 4 == 0)
            {
                // RotWord
                temp = ((temp << 8) & 0xFFFFFF00) | ((temp >> 24) & 0xFF);

                // SubWord
                uint8_t b0 = sbox[(temp >> 24) & 0xFF];
                uint8_t b1 = sbox[(temp >> 16) & 0xFF];
                uint8_t b2 = sbox[(temp >> 8) & 0xFF];
                uint8_t b3 = sbox[(temp >> 0) & 0xFF];

                temp =
                    ((b0 << 24) & (0xFF << 24)) | ((b1 << 16) & (0xFF << 16)) | ((b2 << 8) & (0xFF << 8)) | (b3 & 0xFF);

                // Rcon
                temp = temp ^ rcon[i / 4];
            }

            (*expansion)[i] = temp ^ (*expansion)[i - 4];
        }
    }
    else
    {
        printf("Key scheduling not implemented for AES%d\n", keySize);
        exit(1);
    }
}

void gMixColumns(uint8_t *r)
{
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c;
    unsigned char h;
    /* The array 'a' is simply a copy of the input array 'r'
     * The array 'b' is each element of the array 'a' multiplied by 2
     * in Rijndael's Galois field
     * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
    for (c = 0; c < 4; c++)
    {
        a[c] = r[c];
        /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
        h = (r[c] >> 7) & 1; /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = r[c] << 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b
                             in the next line */
        b[c] ^= h * 0x1B; /* Rijndael's Galois field */
    }
    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

void shiftRows(uint8_t *stateArray)
{
    uint8_t t0;
    uint8_t t1;
    // Row 0 no change
    // Row 1 rotate 1 left
    t0 = stateArray[index(0, 1)];
    stateArray[index(0, 1)] = stateArray[index(1, 1)];
    stateArray[index(1, 1)] = stateArray[index(2, 1)];
    stateArray[index(2, 1)] = stateArray[index(3, 1)];
    stateArray[index(3, 1)] = t0;
    // Row 2 rotate 2
    t0 = stateArray[index(0, 2)];
    stateArray[index(0, 2)] = stateArray[index(2, 2)];
    t1 = stateArray[index(1, 2)];
    stateArray[index(1, 2)] = stateArray[index(3, 2)];
    stateArray[index(2, 2)] = t0;
    stateArray[index(3, 2)] = t1;
    // Row 3 rotate 1 right
    t0 = stateArray[index(3, 3)];
    stateArray[index(3, 3)] = stateArray[index(2, 3)];
    stateArray[index(2, 3)] = stateArray[index(1, 3)];
    stateArray[index(1, 3)] = stateArray[index(0, 3)];
    stateArray[index(0, 3)] = t0;
}

void subBytes(uint8_t *stateArray)
{
    uint8_t *sbox = get_sbox();
    for (int i = 0; i < 16; i++)
    {
        stateArray[i] = sbox[stateArray[i]];
    }
}

void addRoundKey(uint8_t *stateArray, int round, uint32_t *expandedKey)
{
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            stateArray[index(column, row)] ^= (uint8_t)(expandedKey[column + (round * 4)] >> (24 - (8 * row)));
        }
    }
}