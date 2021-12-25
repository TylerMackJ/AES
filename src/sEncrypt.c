#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encryption/encryption.h"
#include "shared/shared.h"

// Used to print out steps of encryption
#define DEBUG 1

/*
Test
Key:        00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
Plaintext:  00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff
Ciphertext: 69 c4 e0 d8 6a 7b 04 30 d8 cd b7 80 70 b4 c5 5a

http://www.herongyang.com/Cryptography/AES-Example-Vector-of-AES-Encryption.html
*/

void encrypt(uint8_t *bytes, int length, uint8_t *key, enum KeySize keySize, uint8_t **encryptedData,
             int *paddedLength);

int main()
{
    // Input
    uint8_t data[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    int dataLength = 16;

    // Key
    enum KeySize keySize = AES128;
    uint8_t *key = (uint8_t *)malloc(sizeof(uint8_t) * (keySize / 8));

    // Output
    uint8_t *encryptedData;
    int encryptedLength;

    // Init key
    for (int i = 0; i < (keySize / 8); i++)
    {
        key[i] = (uint8_t)i;
    }

    // Print out plaintext
    printf("Plaintext:\t");
    printLength(data, dataLength);

    // Print out key
    printf("Key:\t\t");
    printLength(key, 16);

    // Encrypt Data
    encrypt(data, dataLength, key, AES128, &encryptedData, &encryptedLength);

    // Print out encrypted data
    printf("\nEncrypted:\t");
    printLength(encryptedData, encryptedLength);

    return 0;
}

void encrypt(uint8_t *data, int length, uint8_t *key, enum KeySize keySize, uint8_t **encryptedData, int *paddedLength)
{

    // Pad data (with 0) to a 16 byte divisble length
    padBytes(data, length, encryptedData, paddedLength);

    // Expand key
    uint32_t *expandedKey;
    keyExpansion(keySize, key, &expandedKey);

    // Deal with each 128 bit chunk seperately
    for (int chunk = 0; chunk < (*paddedLength / 16); chunk++)
    {
        // Get the beginning of the state array
        uint8_t *stateArray = (*encryptedData) + chunk * 16;

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
                    gMixColumns(&(stateArray[index(column, 0)]));
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
}
