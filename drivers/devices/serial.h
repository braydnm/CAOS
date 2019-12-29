
#ifndef OS_SERIAL_H
#define OS_SERIAL_H

#include "../../kernel/kernel.h"

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
#define SERIAL_PORT_C 0x3E8
#define SERIAL_PORT_D 0x2E8

void serial_enable(int device);
void serial_install();

int serial_rcvd(int device);
char serial_read_byte(int device);
char serial_read_byte_async(int device);
int serial_transmit_empty(int device);
void serial_send_char(int device, char out);
void serial_send_data(int device, char* data);

#endif //OS_SERIAL_H
