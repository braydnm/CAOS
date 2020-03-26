#include "../kernel/kernel.h"
#include "../utils/list.h"

static void clear(list_t* args){
    if (args->length>1)
        return;
    clearScreen();
}

COMMAND_LINE_MODULE(clear, clear, Clears the terminal screen);