#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "encryption.h"
#include "../keyExpansion/keyExpansion.h"

void encryptState(uint8_t* stateArray, uint32_t* expandedKey, enum KeySize keySize) {
    // XOR with first 4 words of key expansion
    addRoundKey(stateArray, 0, expandedKey);
    if (DEBUG)
    {
        printf("\nRound 0:\n\tAddRKey:\t");
        printLength(stateArray, 16);
    }

    // Loop through rounds
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

    for (int round = 1; round <= rounds; round++)
    {
        if (DEBUG)
        {
            printf("\nRound %d:\n", round);
        }

        // Substitute bytes
        subBytes(stateArray);
        if (DEBUG)
        {
            printf("\tSubBytes:\t");
            printLength(stateArray, 16);
        }

        // Shift rows
        shiftRows(stateArray);
        if (DEBUG)
        {
            printf("\tShiftRows:\t");
            printLength(stateArray, 16);
        }

        // Mix columns (not on last round)
        if (round != rounds)
        {
            for (int column = 0; column < 4; column++)
            {
                mixColumns(&(stateArray[index(column, 0)]));
            }
        }
        if (DEBUG)
        {
            printf("\tMixCols:\t");
            printLength(stateArray, 16);
        }

        // Add round key
        addRoundKey(stateArray, round, expandedKey);
        if (DEBUG)
        {
            printf("\tAddRKey:\t");
            printLength(stateArray, 16);
        }
    }
}

void mixColumns(uint8_t *r)
{
    int a[4];
    for (uint8_t c = 0; c < 4; c++)
    {
        a[c] = r[c];
    }
    r[0] = mb02[a[0]] ^ mb03[a[1]] ^      a[2]  ^       a[3] ;
    r[1] =      a[0]  ^ mb02[a[1]] ^ mb03[a[2]] ^       a[3] ;
    r[2] =      a[0]  ^      a[1]  ^ mb02[a[2]] ^  mb03[a[3]];
    r[3] = mb03[a[0]] ^      a[1]  ^      a[2]  ^  mb02[a[3]];
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
