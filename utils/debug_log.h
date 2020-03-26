
#ifndef OS_QEMULOGGING_H
#define OS_QEMULOGGING_H

#include "../kernel/kernel.h"

#define DEBUG_SERIAL 1
#define COM1_PORT  	0x3F8

void serialLog(char *, ...);

#endif //OS_QEMULOGGING_H
