#include "../kernel/kernel.h"

void __assert_func(const char * file, int line, const char * func, const char * failedexpr) {
    serialLog("Assertion failed in %s:%d (%s): %s\n", file, line, func, failedexpr);
    // void
}
