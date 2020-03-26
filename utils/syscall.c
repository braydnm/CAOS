//
// Created by x3vikan on 3/19/18.
//

#include "syscall.h"

void syscall(int call){
    //__asm__ ("cli");
    __asm__ ("mov %%eax, %0\n" : : "b"(call));
    __asm__ ("int %0\n" : : "N"(80));
    //__asm__("sti");
}