#ifndef OS_MODULE_H
#define OS_MODULE_H

#include "../kernel.h"

typedef struct {
    uintptr_t addr;
    char name[];
} kernel_symbol_t;

typedef void (*inputHandlerModuleFunc)(list_t*);

typedef struct {
    char * name;
    int (* initialize)(void);
    int (* finalize)(void);
    inputHandlerModuleFunc cmd;
    char* description;
} module_defs;

typedef struct {
    module_defs * mod_info;
    void * bin_data;
    hashmap_t * symbols;
    uintptr_t end;
    size_t deps_length;
    char * deps;
} module_data_t;

extern void * module_direct_load(void * blob, size_t size);
extern void * module_load(char * filename);
extern void module_unload(char * name);
extern void modules_install(void);

#define MULTI_PART_MODULE(n,init,fini, func, desc) \
        module_defs module_info_ ## n = { \
            .name       = #n, \
            .initialize = &(init), \
            .finalize   = &(fini), \
            .cmd = (func)==null?null:&(func),\
            .description = #desc\
        }

#define KERNEL_MODULE_DEF(n, init, fini)\
    module_defs module_info_ ## n = { \
            .name       = #n, \
            .initialize = &(init), \
            .finalize   = &(fini), \
            .cmd = null,\
            .description = null\
        }

#define COMMAND_LINE_MODULE(n, func, desc)\
        module_defs module_info_ ## n = { \
            .name       = #n, \
            .initialize = null, \
            .finalize   = null, \
            .cmd = &(func),\
            .description = #desc\
        }

extern hashmap_t * modules_get_list(void);
extern hashmap_t * modules_get_symbols(void);

#define MODULE_DEPENDS(n) \
static char _mod_dependency_ ## n [] __attribute__((section("moddeps"), used)) = #n

#endif //OS_MODULE_H
