#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "aes.h"
#include "../keyExpansion/keyExpansion.h"
#include "../encryption/encryption.h"

void encrypt(uint8_t *data, int length, uint8_t *key, enum KeySize keySize, uint8_t **encryptedData, int *paddedLength)
{
    // Pad data (with 0) to a 16 byte divisble length
    ePadBytes(data, length, encryptedData, paddedLength);

    // Get nonce and prepend it to encrypted data
    uint64_t nonce = rand_uint64();
    for (int i = 0; i < 8; i++) 
    {
        (*encryptedData)[i] = ((uint8_t*)&nonce)[i];
    }

    // Expand key
    uint32_t *expandedKey;
    keyExpansion(keySize, key, &expandedKey);

    // Deal with each 128 bit chunk seperately
    for (int chunk = 0; chunk < ((*paddedLength - 8) / 16); chunk++)
    {
        // Build state array and get pointer to it
        StateArray sA;
        sA.nonce = nonce;
        sA.counter = chunk;
        uint8_t* stateArray = (uint8_t*)&sA;

        encryptState(stateArray, expandedKey, keySize);
    
        // Get a pointer to the current part of the plain text
        uint8_t *plaintext = (*encryptedData) + (chunk * 16) + 8;

        if (DEBUG)  
        {
            printf("\n\tPlaintext:\t");
            printLength(plaintext, 16);
        }

        // XOR with stateArray
        for (int i = 0; i < 16; i++)
        {
            plaintext[i] ^= stateArray[i];
        }

        if (DEBUG)  
        {
            printf("\tEncrypted:\t");
            printLength(plaintext, 16);
            printf("\n");
        }
    }
}

void decrypt(uint8_t *data, int length, uint8_t *key, enum KeySize keySize, uint8_t **decryptedData, int *paddedLength)
{
    // Pad data (with 0) to a 16 byte divisble length
    dPadBytes(data, length, decryptedData, paddedLength);

    // Gather nonce
    uint64_t nonce;
    for (int i = 0; i < 8; i++)
    {
        ((uint8_t*)&nonce)[i] = data[i];
    }

    // Expand key
    uint32_t *expandedKey;
    keyExpansion(keySize, key, &expandedKey);

    for (int chunk = 0; chunk < (*paddedLength / 16); chunk++)
    {
        // Build state array and get pointer to it
        StateArray sA;
        sA.nonce = nonce;
        sA.counter = chunk;
        uint8_t* stateArray = (uint8_t*)&sA;

        encryptState(stateArray, expandedKey, keySize);

        // Get a pointer to the current part of the encrypted text
        uint8_t *encrypted = (*decryptedData) + chunk * 16;

        if (DEBUG)  
        {
            printf("\n\tEncrypted:\t");
            printLength(encrypted, 16);
        }

        // XOR with stateArray
        for (int i = 0; i < 16; i++)
        {
            encrypted[i] ^= stateArray[i];
        }

        if (DEBUG)  
        {
            printf("\tEncrypted:\t");
            printLength(encrypted, 16);
            printf("\n");
        }
    }
}

void ePadBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength)
{
    *paddedLength = length;

    // Pad data out to 128 bit chunks
    if (*paddedLength % 16 != 0)
    {
        // If not already divisable by 16 then add an additional chunk
        *paddedLength = ((*paddedLength / 16) + 1) * 16;
    }

    // Add 64 bits for nonce at beginning
    *paddedLength += 8;

    // Move bytes into padded byte buffer
    *paddedBytes = (uint8_t *)malloc(sizeof(uint8_t) * *paddedLength);
    for (int i = 0; i < length; i++)
    {
        (*paddedBytes)[i + 8] = ((uint8_t *)bytes)[i];
    }

    // Assure 0's for padded data
    for (int i = length + 8; i < *paddedLength; i++)
    {
        (*paddedBytes)[i] = '\0';
    }
}

void dPadBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength)
{
    *paddedLength = length - 8;

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
        (*paddedBytes)[i] = ((uint8_t *)bytes)[i + 8];
    }

    // Assure 0's for padded data
    for (int i = length + 8; i < *paddedLength; i++)
    {
        (*paddedBytes)[i] = '\0';
    }
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