//
// Created by x3vikan on 2/18/18.
//
#include "../kernel/kernel.h"

#ifndef OS_ERRORS_H
#define OS_ERRORS_H

extern void IRQ_START();
extern void IRQ_END();

#undef getErrorMethod
#define getErrorMethod(method) error##method

#undef getInterruptMethod
#define getInterruptMethod(method) interruptServiceRoutine##method

/* CPU Exception Error Methods */
extern void error0();
extern void error1();
extern void error2();
extern void error3();
extern void error4();
extern void error5();
extern void error6();
extern void error7();
extern void error8();
extern void error9();
extern void error10();
extern void error11();
extern void error12();
extern void error13();
extern void error14();
extern void error15();
extern void error16();
extern void error17();
extern void error18();
extern void error19();
extern void error20();
extern void error21();
extern void error22();
extern void error23();
extern void error24();
extern void error25();
extern void error26();
extern void error27();
extern void error28();
extern void error29();
extern void error30();
extern void error31();

// Interrupt Service Routine Methods
extern void interruptServiceRoutine0();
extern void interruptServiceRoutine1();
extern void interruptServiceRoutine2();
extern void interruptServiceRoutine3();
extern void interruptServiceRoutine4();
extern void interruptServiceRoutine5();
extern void interruptServiceRoutine6();
extern void interruptServiceRoutine7();
extern void interruptServiceRoutine8();
extern void interruptServiceRoutine9();
extern void interruptServiceRoutine10();
extern void interruptServiceRoutine11();
extern void interruptServiceRoutine12();
extern void interruptServiceRoutine13();
extern void interruptServiceRoutine14();
extern void interruptServiceRoutine15();
extern void syscallRoutine();

#define INTERRUPTSERVICEROUTINE0 32
#define INTERRUPTSERVICEROUTINE1 33
#define INTERRUPTSERVICEROUTINE2 34
#define INTERRUPTSERVICEROUTINE3 35
#define INTERRUPTSERVICEROUTINE4 36
#define INTERRUPTSERVICEROUTINE5 37
#define INTERRUPTSERVICEROUTINE6 38
#define INTERRUPTSERVICEROUTINE7 39
#define INTERRUPTSERVICEROUTINE8 40
#define INTERRUPTSERVICEROUTINE9 41
#define INTERRUPTSERVICEROUTINE10 42
#define INTERRUPTSERVICEROUTINE11 43
#define INTERRUPTSERVICEROUTINE12 44
#define INTERRUPTSERVICEROUTINE13 45
#define INTERRUPTSERVICEROUTINE14 46
#define INTERRUPTSERVICEROUTINE15 47

/* State of all the cpu registers */
typedef struct {
    uint32_t ds; /* Data segment selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
    uint32_t interruptNumber, code; /* Interrupt number and error code (if applicable) */
    uint32_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} state;

void initInterruptHandling();
void errorHandler(state r);

typedef void (*interruptHandler)(state compState);

void addInterruptHandler(uint8_t ptr, interruptHandler routine);
void schedule_no_irq();

#endif //OS_ERRORS_H
