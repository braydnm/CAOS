#include "kernel.h"
#include "../utils/types.h"
#include "../utils/multiboot.h"
#include "../utils/list.h"
#include "../utils/inputHandler.h"

uintptr_t initial_esp = 0;
fs_node_t * ramdisk_mount(uintptr_t, size_t);

char* prompt = "\n>";
char* intro = "\n+-+-+-+-+-+-+-+-+-+\n|C|A|O|S|\n+-+-+-+-+-+-+-+-+-+\n";

mboot_mod_t* setup_modules(struct multiboot* mboot){
    mboot_mod_t * mboot_mods = NULL;
    uintptr_t last_mod = (uintptr_t)&kernel_end;

    if (mboot->flags&MULTIBOOT_FLAG_MODS){
        serialLog("[*] There %s %d module%s starting at 0x%x.", mboot->mods_count == 1 ? "is" : "are", mboot->mods_count, mboot->mods_count == 1 ? "" : "s", mboot->mods_addr);
        serialLog("[*] Current kernel heap start point would be 0x%x.", &kernel_end);
        if (mboot->mods_count > 0) {
            uint32_t i;
            mboot_mods = (mboot_mod_t *)mboot->mods_addr;
            for (i = 0; i < mboot->mods_count; ++i ) {
                mboot_mod_t * mod = &mboot_mods[i];
                uint32_t module_start = mod->mod_start;
                uint32_t module_end   = mod->mod_end;
                if ((uintptr_t)mod + sizeof(mboot_mod_t) > last_mod) {
                    /* Just in case some silly person put this *behind* the modules... */
                    last_mod = (uintptr_t)mod + sizeof(mboot_mod_t);
                    serialLog("[*] moving forward to 0x%x", last_mod);
                }
                serialLog("[*] Module %d is at 0x%x:0x%x", i, module_start, module_end);
                if (last_mod < module_end) {
                    last_mod = module_end;
                }
            }
            serialLog("[*] Moving kernel heap start to 0x%x", last_mod);
        }
    }
    if ((uintptr_t)mboot > last_mod) {
        last_mod = (uintptr_t)mboot + sizeof(struct multiboot);
    }
    while (last_mod & 0x7FF) last_mod++;
    prealloc_start(last_mod);
    return mboot_mods;
}

void install_modules(char* path, char* ending){
    fs_node_t* modsFolder = kopen(path, 0);

    if (!modsFolder->flags&FS_DIRECTORY)
        goto error_loading;

    int index = 0;
    struct dirent* ent;

    while ((ent = readdir_fs(modsFolder, index++))){
        if (endswith(ent->name, ending)){
            char* fileName = kmalloc(strlen(path)+2+strlen(ent->name)+1);
            snprintf(fileName, "/%s/%s",path, ent->name);
            module_load(fileName);
        }
    }
    return;

    error_loading:
    kprintf("Error loading modules...\n");
    while (true);
}

void kmain(struct multiboot *mboot, uint32_t mboot_mag, uintptr_t esp){
    kprintf("[*] Installing serial\n");
    serial_install();
    initial_esp = esp;
    cwd = "/";
    kernelSize = mboot->size;
    bootInfo = mboot;
    kprintf("[*] Moving boot modules\n");
    mboot_mod_t* mboot_mods = setup_modules(mboot);
    gdtInstall();
    modules_install();
    initInterruptHandling();
    timer_install();
    initKeyboard();
    do_multiboot(mboot);
    paging_fini();
    heap_install();
    for (int counter = 0; counter<100; counter++)
        strdup("/");
    inputHandlerInit();
    vfs_install();
    ext2_initialize();
    kprintf("[*] Mounting root\n");
    ramdisk_mount(mboot_mods[0].mod_start, mboot_mods[0].mod_end-mboot_mods[0].mod_start);
    map_vfs_directory("/dev");
    vfs_mount_type("ext2", "/dev/ram0", "/");
    setBackground(BLUE);
    install_modules("/modules", ".ko");
    install_modules("/commands", ".cmd");
    vfs_mount_type("ext2", "/dev/hda0", "/");
    clearScreen();
    kprintfCentered("%s", intro);
    kprintf("\n%s", prompt);
}

void handleUserInput(char* input){
    kprintf("\n");
    list_t* args = split(input, " ");
    terminalFunc* func = ((terminalFunc*)hashmap_get(inputHandlers, list_get(args, 0)->value));
    if (func)
        func->func(args);
    else
        kprintf("Error no command %s\n", list_get(args, 0)->value);

    list_free(args);
    kprint(prompt);
}

void handleCtrlPress(char letter){
    if (letter=='l') {
        clearScreen();
        kprint(prompt);
    }
}
