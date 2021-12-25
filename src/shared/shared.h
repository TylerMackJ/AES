#ifndef shared
#define shared

#include <stdint.h>
#define index(x, y) ((x * 4) + y)

enum KeySize
{
    AES128 = 128,
    AES192 = 192,
    AES256 = 256
};

void printLength(uint8_t *data, int length);

// From https://en.wikipedia.org/wiki/Rijndael_S-box
uint8_t *get_sbox();

#endif