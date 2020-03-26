//
// Created by x3vikan on 2/18/18.
//
#include "interruptServiceRoutine.h"

uint8_t __enabled = 1;
interruptHandler interruptHandlers[NUM_INTERRUPT_ENTRIES];

/* Can't do this with a loop because we need the address
 * of the function names */
void initInterruptHandling() {
    setInterrupt();

    setInterruptGate(0, (uint32_t)error0);
    setInterruptGate(1, (uint32_t)error1);
    setInterruptGate(2, (uint32_t)error2);
    setInterruptGate(3, (uint32_t)error3);
    setInterruptGate(4, (uint32_t)error4);
    setInterruptGate(5, (uint32_t)error5);
    setInterruptGate(6, (uint32_t)error6);
    setInterruptGate(7, (uint32_t)error7);
    setInterruptGate(8, (uint32_t)error8);
    setInterruptGate(9, (uint32_t)error9);
    setInterruptGate(10, (uint32_t)error10);
    setInterruptGate(11, (uint32_t)error11);
    setInterruptGate(12, (uint32_t)error12);
    setInterruptGate(13, (uint32_t)error13);
    setInterruptGate(14, (uint32_t)error14);
    setInterruptGate(15, (uint32_t)error15);
    setInterruptGate(16, (uint32_t)error16);
    setInterruptGate(17, (uint32_t)error17);
    setInterruptGate(18, (uint32_t)error18);
    setInterruptGate(19, (uint32_t)error19);
    setInterruptGate(20, (uint32_t)error20);
    setInterruptGate(21, (uint32_t)error21);
    setInterruptGate(22, (uint32_t)error22);
    setInterruptGate(23, (uint32_t)error23);
    setInterruptGate(24, (uint32_t)error24);
    setInterruptGate(25, (uint32_t)error25);
    setInterruptGate(26, (uint32_t)error26);
    setInterruptGate(27, (uint32_t)error27);
    setInterruptGate(28, (uint32_t)error28);
    setInterruptGate(29, (uint32_t)error29);
    setInterruptGate(30, (uint32_t)error30);
    setInterruptGate(31, (uint32_t)error31);

    // Remap the peripheral interface controller
    // Weird masks and stuff, still looking into theory but makes everything work
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);

    // Install the interrupt service routines
    setInterruptGate(32, (uint32_t)interruptServiceRoutine0);
    setInterruptGate(33, (uint32_t)interruptServiceRoutine1);
    setInterruptGate(34, (uint32_t)interruptServiceRoutine2);
    setInterruptGate(35, (uint32_t)interruptServiceRoutine3);
    setInterruptGate(36, (uint32_t)interruptServiceRoutine4);
    setInterruptGate(37, (uint32_t)interruptServiceRoutine5);
    setInterruptGate(38, (uint32_t)interruptServiceRoutine6);
    setInterruptGate(39, (uint32_t)interruptServiceRoutine7);
    setInterruptGate(40, (uint32_t)interruptServiceRoutine8);
    setInterruptGate(41, (uint32_t)interruptServiceRoutine9);
    setInterruptGate(42, (uint32_t)interruptServiceRoutine10);
    setInterruptGate(43, (uint32_t)interruptServiceRoutine11);
    setInterruptGate(44, (uint32_t)interruptServiceRoutine12);
    setInterruptGate(45, (uint32_t)interruptServiceRoutine13);
    setInterruptGate(46, (uint32_t)interruptServiceRoutine14);
    setInterruptGate(47, (uint32_t)interruptServiceRoutine15);
    setInterruptGate(80, (uint32_t)syscallRoutine);
    asm volatile ("sti");
}

// Message exception based on interrupt type
char *exception_messages[] = {
        "Division By Zero",
        "Debug",
        "Non Maskable Interrupt",
        "Breakpoint",
        "Into Detected Overflow",
        "Out of Bounds",
        "Invalid Opcode",
        "No Coprocessor",

        "Double Fault",
        "Coprocessor Segment Overrun",
        "Bad TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        "Unknown Interrupt",

        "Coprocessor Fault",
        "Alignment Check",
        "Machine Check",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",

        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved"
};

void errorHandler(state r) {
    kprint("received interrupt: ");
    char s[3];
    itoa(r.interruptNumber, 10, s);
    kprint(s);
    kprint("\n");
    kprint(exception_messages[r.interruptNumber]);
    kprint("\n");
    while(true);
}

void addInterruptHandler(uint8_t ptr, interruptHandler handler){interruptHandlers[ptr] = handler;}

void interruptServiceRoutineHandler(state compState){
    // after all interrupts send an EOI to the PICs so they will still send interrupts
    // who the hell designed computers
    if (compState.interruptNumber>=40) outportb(0xA0, 0x20); // fix slave PIC
    outportb(0x20, 0x20); // write to master PIC

    if (compState.interruptNumber==80) kprintf("\nSyscall happened with code %d\n", compState.code);
    // call the han50dler passing the state in
    else if (interruptHandlers[compState.interruptNumber]!=0) interruptHandlers[compState.interruptNumber](compState);
}

void schedule_no_irq() {
    //if(!__enabled) return;
    asm volatile("int $0x2e");
    return;
}