//
// Created by x3vikan on 4/5/18.
//

#ifndef OS_INPUTHANDLER_H
#define OS_INPUTHANDLER_H

#include "../kernel/kernel.h"

typedef void (*inputHandlerFunction)(list_t*);
typedef struct{
    inputHandlerFunction func;
    char* description;
} terminalFunc;
hashmap_t* inputHandlers;

void addInputHandler(char *name, inputHandlerFunction func, char* description);
void inputHandlerInit();

#define CMDLINE_RUN(NAME, ENTRY)addInputHandler(NAME, ENTRY)

#endif //OS_MODULE_H
