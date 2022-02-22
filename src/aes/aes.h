#ifndef aes
#define aes

#include <stdint.h>

#include "../shared/shared.h"

void encrypt(uint8_t *data, int length, uint8_t *key, enum KeySize keySize, uint8_t **encryptedData, int *paddedLength);

void decrypt(uint8_t *data, int length, uint8_t *key, enum KeySize keySize, uint8_t **decryptedData, int *paddedLength);

void ePadBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength);

void dPadBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength);

// https://stackoverflow.com/questions/33010010/how-to-generate-random-64-bit-unsigned-integer-in-c
uint64_t rand_uint64(void);

#endif