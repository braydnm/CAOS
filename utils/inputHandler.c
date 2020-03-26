//
// Created by x3vikan on 4/5/18.
//

#include "../kernel/kernel.h"
#include "inputHandler.h"

hashmap_t* inputHandlers;
bool init = 0;

void inputHandlerInit(){
    init = 1;
    inputHandlers = hashmap_create(1);
}

void addInputHandler(char *name, inputHandlerFunction func, char* description){
    if (!init)
        return;

    terminalFunc* command = kmalloc(sizeof(terminalFunc));
    command->func = func;
    command->description = description;
    hashmap_set(inputHandlers, name, command);
}