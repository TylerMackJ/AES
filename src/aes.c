#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void* aes128(void* bytes, int length, void* key);
void padBytes(void *bytes, int length, char **paddedBytes, int *paddedLength);

int main()
{
    char* data = "Hello world";

    aes128((void*)data, sizeof(char) * strlen(data), (void*)0); 
    return 0;
}

void* aes128(void* bytes, int length, void* key)
{
    char *paddedBytes;
    int paddedLength;

    padBytes(bytes, length, &paddedBytes, &paddedLength);
    
}

void padBytes(void *bytes, int length, char **paddedBytes, int *paddedLength)
{
    *paddedLength = length;

    // Pad data out to 128 bit chunks
    if (*paddedLength % 16 != 0)
    {
        // If not already divisable by 16 then add an additional chunk
        *paddedLength = ((*paddedLength / 16) + 1) * 16;
    }

    // Move bytes into padded byte buffer
    *paddedBytes = (char*)malloc(sizeof(char) * *paddedLength);
    for (int i = 0; i < length; i++) 
    {
        (*paddedBytes)[i] = ((char*)bytes)[i];
    }

    // Assure 0's for padded data
    for (int i = length; i < *paddedLength; i++)
    {
        (*paddedBytes)[i] = '.';
    }
}