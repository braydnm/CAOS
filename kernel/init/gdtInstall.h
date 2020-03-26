
#ifndef OS_GDTINSTALL_H
#define OS_GDTINSTALL_H


#include "../kernel.h"

extern void gdt_flush();
extern void tss_flush();

typedef struct tss_entry {
    uint32_t	prev_tss;
    uint32_t	esp0;
    uint32_t	ss0;
    uint32_t	esp1;
    uint32_t	ss1;
    uint32_t	esp2;
    uint32_t	ss2;
    uint32_t	cr3;
    uint32_t	eip;
    uint32_t	eflags;
    uint32_t	eax;
    uint32_t	ecx;
    uint32_t	edx;
    uint32_t	ebx;
    uint32_t	esp;
    uint32_t	ebp;
    uint32_t	esi;
    uint32_t	edi;
    uint32_t	es;
    uint32_t	cs;
    uint32_t	ss;
    uint32_t	ds;
    uint32_t	fs;
    uint32_t	gs;
    uint32_t	ldt;
    uint16_t	trap;
    uint16_t	iomap_base;
} __attribute__ ((packed)) tss_entry_t;

struct gdt_entry {
    /* Limits */
    unsigned short limit_low;
    /* Segment address */
    unsigned short base_low;
    unsigned char base_middle;
    /* Access modes */
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[6];
struct gdt_ptr gdtPtr;

void gdtSetGate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char granularity);
void gdtInstall();
void setKernelStack(uintptr_t stack);

#endif //OS_GDTINSTALL_H
