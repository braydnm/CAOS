#include "../kernel/kernel.h"
#include "../utils/inputHandler.h"

static void help(){
    list_t* commands = hashmap_keys(inputHandlers);
    kprintf("\n");
    foreach(command, commands){
        kprintf("%s: %s\n", command->value, ((terminalFunc*)hashmap_get(inputHandlers, command->value))->description);
    }
}

COMMAND_LINE_MODULE(help, help, Prints out the description of all commands);