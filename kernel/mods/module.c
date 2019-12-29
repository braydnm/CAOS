#include "../kernel.h"
#include "../exec/elf.h"
#include "module.h"

#define SYMBOLTABLE_HASHMAP_SIZE 10
#define MODULE_HASHMAP_SIZE 10

static hashmap_t * symboltable = NULL;
static hashmap_t * modules = NULL;

extern char kernel_symbols_start[];
extern char kernel_symbols_end[];

void* module_direct_load(void* blob, size_t size){
    Elf32_Header * target = (Elf32_Header *)blob;

    if (target->e_ident[0] != ELFMAG0 ||
        target->e_ident[1] != ELFMAG1 ||
        target->e_ident[2] != ELFMAG2 ||
        target->e_ident[3] != ELFMAG3) {

        serialLog("Module is not a valid ELF object.\n");

        goto mod_load_error_unload;
    }

    if ((target->e_shoff + (target->e_shstrndx*target->e_shentsize))>size) {
        serialLog("Could not locate module section header string table.\n");
        goto mod_load_error_unload;
    }
    char * sections_table = (char *)((uintptr_t)target + ((Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + (target->e_shstrndx*target->e_shentsize))))->sh_offset);
    char * symbol_string_table = NULL;
    Elf32_Shdr * symbol_section = NULL;
    char * deps = NULL;
    size_t deps_length = 0;

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_STRTAB && !strcmp((char *)((uintptr_t)sections_table + shdr->sh_name), ".strtab")) {
            symbol_string_table = (char *)((uintptr_t)target + shdr->sh_offset);
        }
    }

    serialLog("Checking dependencies.\n");
    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if ((!strcmp((char *)((uintptr_t)sections_table + shdr->sh_name), "moddeps"))) {
            deps = (char*)((Elf32_Addr)target + shdr->sh_offset);
            deps_length = shdr->sh_size;

            unsigned int i = 0;
            while (i < deps_length) {
                if (strlen(&deps[i]) && !hashmap_get(modules, &deps[i])) {
                    serialLog("   %s - not loaded\n", &deps[i]);
                    goto mod_load_error_unload;
                }
                serialLog("   %s\n", &deps[i]);
                i += strlen(&deps[i]) + 1;
            }
        }
    }

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_SYMTAB) {
            symbol_section = shdr;
        }
    }

    if (!symbol_section) {
        serialLog("Could not locate section for symbol table.\n");
        goto mod_load_error_unload;
    }

    serialLog("Loading sections.\n");
    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_NOBITS) {
            shdr->sh_addr = (Elf32_Addr)kmalloc(shdr->sh_size);
            memset((void *)shdr->sh_addr, 0x00, shdr->sh_size);
        } else {
            shdr->sh_addr = (Elf32_Addr)target + shdr->sh_offset;
        }
    }

    int undefined = 0;

    hashmap_t * local_symbols = hashmap_create(10);
    Elf32_Sym * table = (Elf32_Sym *)((uintptr_t)target + symbol_section->sh_offset);
    while ((uintptr_t)table - ((uintptr_t)target + symbol_section->sh_offset) < symbol_section->sh_size) {
        if (table->st_name) {
            if (ELF32_ST_BIND(table->st_info) == STB_GLOBAL) {
                char * name = (char *)((uintptr_t)symbol_string_table + table->st_name);
                if (table->st_shndx == 0) {
                    if (!hashmap_get(symboltable, name)) {
                        serialLog("Unresolved symbol in module: %s\n", name);
                        undefined = 1;
                    }
                } else {
                    Elf32_Shdr * s = NULL;
                    {
                        int i = 0;
                        int set = 0;
                        for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
                            Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
                            if (i == table->st_shndx) {
                                set = 1;
                                s = shdr;
                                break;
                            }
                            i++;
                        }
                        if (!set && table->st_shndx == 65522) {
                            if (!hashmap_get(symboltable, name)) {
                                void * final = calloc(1, table->st_value);
                                serialLog("point %s to 0x%x\n", name, (uintptr_t)final);
                                hashmap_set(symboltable, name, (void *)final);
                                hashmap_set(local_symbols, name, (void *)final);
                            }
                        }
                    }
                    if (s) {
                        uintptr_t final = s->sh_addr + table->st_value;
                        hashmap_set(symboltable, name, (void *)final);
                        hashmap_set(local_symbols, name, (void *)final);
                    } else {
                        serialLog("Not resolving %s\n", name);
                    }
                }
            } else if (ELF32_ST_BIND(table->st_info) == STB_LOCAL) {
                char * name = (char *)((uintptr_t)symbol_string_table + table->st_name);
                Elf32_Shdr * s = NULL;
                {
                    int i = 0;
                    int set = 0;
                    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
                        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
                        if (i == table->st_shndx) {
                            set = 1;
                            s = shdr;
                            break;
                        }
                        i++;
                    }
                    if (!set && table->st_shndx == 65522) {
                        if (!hashmap_get(symboltable, name)) {
                            void * final = calloc(1, table->st_value);
                            serialLog("point %s to 0x%x\n", name, (uintptr_t)final);
                            hashmap_set(local_symbols, name, (void *)final);
                        }
                    }
                }
                if (s) {
                    uintptr_t final = s->sh_addr + table->st_value;
                    hashmap_set(local_symbols, name, (void *)final);
                }
            }
        }
        table++;
    }

    if (undefined) {
        serialLog("This module is faulty! Verify it specifies all of its\n");
        serialLog("dependencies properly with MODULE_DEPENDS.\n");
        goto mod_load_error;
    }

    for (unsigned int x = 0; x < (unsigned int)target->e_shentsize * target->e_shnum; x += target->e_shentsize) {
        Elf32_Shdr * shdr = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + x));
        if (shdr->sh_type == SHT_REL) {
            Elf32_Rel * section_rel = (void *)(shdr->sh_addr);
            Elf32_Rel * table = section_rel;
            Elf32_Sym * symtable = (Elf32_Sym *)(symbol_section->sh_addr);
            while ((uintptr_t)table - (shdr->sh_addr) < shdr->sh_size) {
                Elf32_Sym * sym = &symtable[ELF32_R_SYM(table->r_info)];
                Elf32_Shdr * rs = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + shdr->sh_info * target->e_shentsize));

                uintptr_t addend = 0;
                uintptr_t place  = 0;
                uintptr_t symbol = 0;
                uintptr_t *ptr   = NULL;

                if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION) {
                    Elf32_Shdr * s = (Elf32_Shdr *)((uintptr_t)target + (target->e_shoff + sym->st_shndx * target->e_shentsize));
                    ptr = (uintptr_t *)(table->r_offset + rs->sh_addr);
                    addend = *ptr;
                    place  = (uintptr_t)ptr;
                    symbol = s->sh_addr;
                } else {
                    char * name = (char *)((uintptr_t)symbol_string_table + sym->st_name);
                    ptr = (uintptr_t *)(table->r_offset + rs->sh_addr);
                    addend = *ptr;
                    place  = (uintptr_t)ptr;
                    if (!hashmap_get(symboltable, name)) {
                        if (!hashmap_get(local_symbols, name)) {
                            serialLog("Wat? Missing symbol %s\n", name);
                            serialLog("Here's all the symbols:\n");
                        } else {
                            symbol = (uintptr_t)hashmap_get(local_symbols, name);
                        }
                    } else {
                        symbol = (uintptr_t)hashmap_get(symboltable, name);
                    }
                }
                switch (ELF32_R_TYPE(table->r_info)) {
                    case 1:
                        *ptr = addend + symbol;
                        break;
                    case 2:
                        *ptr = addend + symbol - place;
                        break;
                    default:
                        serialLog("Unsupported relocation type: %d\n", ELF32_R_TYPE(table->r_info));
                        goto mod_load_error;
                }

                table++;
            }
        }
    }

    serialLog("Locating module information...\n");
    module_defs * mod_info = NULL;
    list_t * hash_keys = hashmap_keys(local_symbols);
    foreach(_key, hash_keys) {
        char * key = (char *)_key->value;
        if (startswith(key, "module_info_")) {
            mod_info = hashmap_get(local_symbols, key);
        }
    }

    list_free(hash_keys);
    free(hash_keys);
    if (!mod_info) {
        serialLog("Failed to locate module information structure!\n");
        goto mod_load_error;
    }

    if (mod_info->initialize!=null)
        mod_info->initialize();

    if (mod_info->cmd!=null && mod_info->description != null)
        addInputHandler(mod_info->name, mod_info->cmd, mod_info->description);

    serialLog("Finished loading module %s\n", mod_info->name);

    module_data_t * mod_data = kmalloc(sizeof(module_data_t));
    mod_data->mod_info = mod_info;
    mod_data->bin_data = target;
    mod_data->symbols  = local_symbols;
    mod_data->end      = (uintptr_t)target + size;
    mod_data->deps     = deps;
    mod_data->deps_length = deps_length;

    hashmap_set(modules, mod_info->name, (void *)mod_data);

    return mod_data;

    mod_load_error_unload:
    return (void *)-1;

    mod_load_error:
    return NULL;
}

void* module_load(char* filename){
    fs_node_t* file = kopen(filename, 0);

    if (!file) {
        serialLog("Failed to load module: %s\n", filename);
        return NULL;
    }

    serialLog("Attempting to load kernel module: %s\n", filename);

    void * blob = (void *)kmalloc(file->length);
    read_fs(file, 0, file->length, (uint8_t *)blob);

    void * result = module_direct_load(blob, file->length);

    if (result == (void *)-1) {
        free(blob);
        result = NULL;
    }

    close_fs(file);
    return result;
}

void modules_install(void) {
    /* Initialize the symboltable, we use a hashmap of symbols to addresses  */
    symboltable = hashmap_create(SYMBOLTABLE_HASHMAP_SIZE);

    /* Load all of the kernel symbols into the symboltable */
    kernel_symbol_t * k = (kernel_symbol_t *)&kernel_symbols_start;
    //serialLog("0x%x\n", (uintptr_t)&kernel_symbols_end-(uintptr_t)&kernel_symbols_start);
    while ((uintptr_t)k < (uintptr_t)&kernel_symbols_end) {
        hashmap_set(symboltable, k->name, (void *)k->addr);
        k = (kernel_symbol_t *)((uintptr_t)k + sizeof(kernel_symbol_t) + strlen(k->name) + 1);
    }

    /* Also add the kernel_symbol_start and kernel_symbol_end (these were excluded from the generator) */
    hashmap_set(symboltable, "kernel_symbols_start", &kernel_symbols_start);
    hashmap_set(symboltable, "kernel_symbols_end",   &kernel_symbols_end);

    /* Initialize the module name -> object hashmap */
    modules = hashmap_create(MODULE_HASHMAP_SIZE);
}