
#ifndef OS_PANIC_H
#define OS_PANIC_H

#include "../kernel/kernel.h"

#define panic(input, ...) kprintf(input, ##__VA_ARGS__);\
                            while(true)

#endif //OS_PANIC_H
