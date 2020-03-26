#include <kernel.h>
#include <utils/list.h>

static void write(list_t* args){
    if (args->length<3)
        return;

    fs_node_t* node = kopen(list_get(args, 1)->value, 0);
    if (node==null) {
        create_file_fs(list_get(args, 1)->value, 0);
        node = kopen(list_get(args,1)->value, 0);
    }
    char* val = kmalloc(strlen(list_get(args, 2)->value)+1);
    snprintf(val, "%s\0", list_get(args,2)->value);
    write_fs(node, 0, strlen(list_get(args, 2)->value)+1, val);
    close_fs(node);
}

COMMAND_LINE_MODULE(write, write, Write something to a file);