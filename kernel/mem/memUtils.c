//
// Created by x3vikan on 2/19/18.
//
#include "memUtils.h"

void* memcpy(char* dest, char* source, size_t size){
    for (int counter = 0; counter<size; counter++)
        *(dest+counter) = *(source+counter);
    return (dest);
}

void* memset(void* dest, int val, size_t size){
    uint8_t * temp = (uint8_t *)dest;
    do{ *temp++=val;} while (size--);
    return (dest);
}

void* memmove(void* destAddr, const void* sourceAddr, size_t size){
    char* dest = destAddr;
    const char* source = sourceAddr;

    // move from low mem to high mem
    if (source<dest)
        // copy from end to start
        for (source+=size, dest+=size; size; size--)
            *--dest = *--source;

    else if (source!=dest)
        for (; size; size--)
            *dest++ = *source++;
    return destAddr;
}
