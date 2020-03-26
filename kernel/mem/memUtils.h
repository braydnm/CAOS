//
// Created by x3vikan on 2/19/18.
//


#ifndef OS_MEMORY_H
#define OS_MEMORY_H

#include "../kernel.h"

void* memcpy(char* dest, char* source, size_t size);
void* memset(void* dest, int val, size_t size);
void* memmove(void* destAddr, const void* sourceAddr, size_t size);

#endif //OS_MEMORY_H
