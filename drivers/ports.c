#include "ports.h"

unsigned short inports(unsigned short _port) {
    unsigned short rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outports(unsigned short _port, unsigned short _data) {
    asm volatile ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

unsigned int inportl(unsigned short _port) {
    unsigned int rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportl(unsigned short _port, unsigned int _data) {
    asm volatile ("outl %%eax, %%dx" : : "dN" (_port), "a" (_data));
}

unsigned char inportb(unsigned short _port) {
    unsigned char rv;
    asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb(unsigned short _port, unsigned char _data) {
    asm volatile ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void outportsm(unsigned short port, unsigned char * data, unsigned long size) {
    asm volatile ("rep outsw" : "+S" (data), "+c" (size) : "d" (port));
}

void inportsm(unsigned short port, unsigned char * data, unsigned long size) {
    asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}

uint16_t inportw(uint16_t p) {
    uint16_t r;
    asm volatile("inw %%dx, %%ax":"=a"(r):"d"(p));
    return r;
}

void pic_send_eoi(uint8_t irq){
    if(irq >= 8)
        outportb(PIC_SLAVE_CMD, PIC_CMD_EOI);
    outportb(PIC_MASTER_CMD, PIC_CMD_EOI);
}
