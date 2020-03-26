#include <kernel.h>

static void reboot(list_t* args){
    outportb(0x64, 0xFE);
}

COMMAND_LINE_MODULE(reboot, reboot, Reboot the computer);
