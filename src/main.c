#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes/aes.h"


int main()
{
   uint8_t *data;

   FILE *fp;
   fp = fopen("./in.bin", "rb");
   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }

   fseek(fp, 0L, SEEK_END);
   long dataLength = ftell(fp);

   rewind(fp);

    data = (uint8_t*)malloc(sizeof(uint8_t) * dataLength);

    fread(data, sizeof(uint8_t), dataLength, fp);

    fclose(fp);

    // Key
    enum KeySize keySize = AES128;
    //uint8_t *key = (uint8_t *)malloc(sizeof(uint8_t) * (keySize / 8));

    // Output
    uint8_t *encryptedData;
    int encryptedLength;
    uint8_t *decryptedData;
    int decryptedLength;

    // Init key
    /*
    for (int i = 0; i < (keySize / 8); i++)
    {
        key[i] = (uint8_t)i;
    }
    */

   uint8_t key[16] = { 0xbf, 0x02, 0xc4, 0xa0, 0x53, 0xef, 0x1b, 0x0b, 0x11, 0x15, 0x62, 0x0e, 0x39, 0x36, 0x30, 0x75 };

    // Print out key
    printf("Key:\t\t");
    printLength(key, keySize / 8);

    // Print out plaintext
    printf("Plaintext:\t");
    printLength(data, dataLength);

    // Encrypt Data
    encrypt(data, dataLength, key, keySize, rand_uint64(), &encryptedData, &encryptedLength);

    // Print out encrypted data
    printf("\nEncrypted:\t");
    printLength(encryptedData, encryptedLength);

    // Decrypt Data
    decrypt(encryptedData, encryptedLength, key, keySize, &decryptedData, &decryptedLength);

    // Print out decrypted data
    printf("\nPlaintext:\t");
    printLength(decryptedData, decryptedLength);

    return 0;
}
