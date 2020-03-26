#include "../kernel/kernel.h"

static void readFile(list_t* args){
    if (args->length<2)
        return;
    fs_node_t* file = kopen(list_get(args, 1)->value, NULL);
    if (!file){
        kprintf("%s not found\n", list_get(args, 1)->value);
        free(file);
        return;
    }
    int size = file->length;
    uint8_t* content = kmalloc(size);
    read_fs(file, 0, size, content);
    kprintf("%s\n", (char*)content);
    free(content);
    close_fs(file);
}

COMMAND_LINE_MODULE(read, readFile, Read the contents of a file);
