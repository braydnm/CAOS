//
// Created by Braydn on 2/15/18.
//

#ifndef OS_INTERRUPTS_H
#define OS_INTERRUPTS_H

#include "../kernel/kernel.h"


#define KERNEL_SEGMENT 0x08

typedef struct {

    uint16_t lowOffset; // lower 16 bits of handler function
    uint16_t kernelSegmentSelector;
    uint8_t always0; // this is needed so the struct is the right size and the cpu doesn't throw a hissy fit
    /*
     * Flag Structure:
     * Bit 7: Interrupt is present
     * Bit 5-6: Privilege level of the calling user(lower is higher priv)
     * Bit 4: Set to 0 for interrupt gates
     * Bit 0-3: 32 bit interrupt gate
     */
    uint8_t flags;
    uint16_t highOffset; // higher 16 bits of handler function
} __attribute__((packed)) interruptSignal;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) interruptRegister;

#define NUM_INTERRUPT_ENTRIES 256
interruptSignal interrupts[NUM_INTERRUPT_ENTRIES];
interruptRegister reg;

void setInterruptGate(int i, uint32_t handler);
void setInterrupt();

#endif //OS_INTERRUPTS_H