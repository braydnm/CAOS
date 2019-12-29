//
// Created by x3vikan on 2/19/18.
//

#ifndef OS_KERNEL_H
#define OS_KERNEL_H
#ifndef NULL
    #define NULL 0
#endif

#define handlerDecleration(functionName) void functionName(list_t*)

#define MIN(A, B) ((A) < (B) ? (A) : (B))

#include "../utils/types.h"
#include "../utils/va_args.h"
#include "init/gdtInstall.h"
#include "../drivers/devices/device.h"
#include "../drivers/devices/keyboard.h"
#include "../drivers/devices/cmos.h"
#include "../drivers/devices/serial.h"
#include "../drivers/pci.h"
#include "../drivers/ports.h"
#include "../drivers/screen/screen.h"
#include "../drivers/screen/screenUtils.h"
#include "exec/elf.h"
#include "mem/memUtils.h"
#include "mem/mem.h"
#include "filesystem/fs.h"
#include "filesystem/ext2.h"
#include "../signalHandling/interruptDescriptorTable.h"
#include "../signalHandling/interruptServiceRoutine.h"
#include "../utils/list.h"
#include "../utils/qsort.h"
#include "../utils/snprintf.h"
#include "../utils/sort.h"
#include "../utils/hashmap.h"
#include "mods/module.h"
#include "filesystem/pipe.h"
#include "../utils/colors.h"
#include "../utils/error.h"
#include "../utils/inputHandler.h"
#include "../utils/math.h"
#include "../utils/multiboot.h"
#include "../utils/debug_log.h"
#include "../utils/panic.h"
#include "../utils/stdint.h"
#include "../utils/string.h"
#include "../utils/suppressWarning.h"
#include "../utils/syscall.h"
#include "../drivers/devices/timer.h"
#include "../utils/tree.h"
#include "../utils/wordsize.h"

char* cwd;

uint32_t kernelSize;
struct multiboot* bootInfo;

extern unsigned long timer_ticks;
extern unsigned long timer_subticks;
extern signed long timer_drift;
extern int kernel_end;

#define low_offset(address) (uint16_t)((address) & 0xFFFF)
#define high_offset(address) (uint16_t)(((address) >> 16) & 0xFFFF)

void handleUserInput(char input[]);
void handleCtrlPress(char input);

extern void __assert_func(const char * file, int line, const char * func, const char * failedexpr);
#define assert(statement) ((statement) ? (void)0 : __assert_func(__FILE__, __LINE__, __FUNCTION__, #statement))

#endif //OS_KERNEL_H
