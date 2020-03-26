#include <kernel.h>

static void echo(list_t* args){
    kprintf("%s\n", list_get(args, 1)->value);
}

COMMAND_LINE_MODULE(echo, echo, Prints what you type);
