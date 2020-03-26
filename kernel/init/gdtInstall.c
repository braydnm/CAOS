#include <init/gdtInstall.h>

tss_entry_t tssEntry;

static void writeTSS(int32_t num, uint16_t ss0, uint32_t esp0){
    uintptr_t base = (uintptr_t)&tssEntry;
    uintptr_t limit = base+ sizeof(tssEntry);

    // add TSS descriptor to GDT
    gdtSetGate(num, base, limit, 0xE9, 0x00);

    memset(&tssEntry, 0, sizeof(tssEntry));

    tssEntry.ss0 = ss0;
    tssEntry.esp0 = esp0;
    tssEntry.ss =
    tssEntry.ds =
    tssEntry.es =
    tssEntry.fs =
    tssEntry.gs = 0x13;

    tssEntry.iomap_base = sizeof(tssEntry);
}

void gdtSetGate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char granularity){
    /* Base Address */
    gdt[num].base_low = (unsigned short) (base & 0xFFFF);
    gdt[num].base_middle = (unsigned char) ((base >> 16) & 0xFF);
    gdt[num].base_high = (unsigned char) ((base >> 24) & 0xFF);
    /* Limits */
    gdt[num].limit_low = (unsigned short) (limit & 0xFFFF);
    gdt[num].granularity = (unsigned char) ((limit >> 16) & 0X0F);
    /* Granularity */
    gdt[num].granularity |= (granularity & 0xF0);
    /* Access flags */
    gdt[num].access = access;
}

void gdtInstall(){
    gdtPtr.limit = (sizeof(struct gdt_entry)*6)-1;
    gdtPtr.base = (unsigned int)&gdt;

    // null gate
    gdtSetGate(0,0,0,0,0);
    // code segment
    gdtSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // data segment
    gdtSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    // user code
    gdtSetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    // user data
    gdtSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    writeTSS(5, 0x10, 0x0);

    // write everything
    gdt_flush();
    tss_flush();
}

void setKernelStack(uintptr_t stack){
    tssEntry.esp0 = stack;
}