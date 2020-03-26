#include <kernel.h>

static void ls(list_t* args){
    fs_node_t* dir = kopen(args->length>=2?list_get(args, 1)->value:"", NULL);
    if (!dir->flags&FS_DIRECTORY){
        kprintf("%s is not a directory\n", list_get(args, 1)->value);
        return;
    }

    list_t* files = list_create();
    int index = 0;
    struct dirent* ent;
    while ((ent = readdir_fs(dir, index++)))
        if (ent->name[0]!='.')
            list_insert(files, ent->name);
    free(dir);
    free(ent);
    kprintf("Contents of %s:\n", args->length>=2?list_get(args, 1)->value:cwd);
    foreach(item, files){
        kprintf("\t%s\n", item->value);
    }
    list_free(files);
}

COMMAND_LINE_MODULE(ls, ls, Print the contents of the current directory);
