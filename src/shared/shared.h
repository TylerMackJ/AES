#ifndef shared
#define shared

#define DEBUG 0

#include <stdint.h>
#define index(x, y) ((x * 4) + y)

typedef struct StateArray {
    uint64_t nonce;
    uint64_t counter;
} StateArray;


/* REQUIRES SBOX FOR 256 https://samiam.org/key-schedule.html */
enum KeySize
{
    AES128 = 128,
    AES192 = 192,
    AES256 = 256
};

void printLength(uint8_t *data, int length);

#endif