//
// Created by Braydn on 2/15/18.
//
#include "../kernel/kernel.h"

void setInterruptGate(int n, uint32_t handler){
    interrupts[n].lowOffset = low_offset(handler);
    interrupts[n].kernelSegmentSelector = KERNEL_SEGMENT;
    interrupts[n].always0 = 0;
    interrupts[n].flags = 0x8E|0x60;
    interrupts[n].highOffset = high_offset(handler);
}

void setInterrupt() {
    reg.base = (uint32_t) &interrupts;
    reg.limit = NUM_INTERRUPT_ENTRIES * sizeof(interruptSignal) - 1;
    memset(&interrupts, 0, sizeof(interruptSignal)*256);
    /* Don't make the mistake of loading &idt -- always load &idt_reg */
    __asm__ __volatile__("lidtl (%0)" : : "r" (&reg));
}
