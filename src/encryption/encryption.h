#ifndef encryption
#define encryption

#include "../shared/shared.h"
#include <stdint.h>

// https://www.samiam.org/key-schedule.html
void keyExpansion(enum KeySize keySize, uint8_t *key, uint32_t **expansion);

void padBytes(uint8_t *bytes, int length, uint8_t **paddedBytes, int *paddedLength);

// https://en.wikipedia.org/wiki/Rijndael_MixColumns
void gMixColumns(uint8_t *r);

void shiftRows(uint8_t *stateArray);

void subBytes(uint8_t *stateArray);

void addRoundKey(uint8_t *stateArray, int round, uint32_t *expandedKey);

#endif