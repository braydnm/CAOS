
#ifndef OS_CMOS_H
#define OS_CMOS_H

#include "../../kernel/kernel.h"

struct timeval {
    time_t      tv_sec;     /* seconds */
    suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
};

extern int gettimeofday(struct timeval *p, void *z);

extern uint32_t boot_time;

void get_time(uint16_t * hours, uint16_t * minutes, uint16_t * seconds);
int gettimeofday(struct timeval * t, void *z);
uint32_t read_cmos(void);
uint32_t now();

#endif //OS_CMOS_H
