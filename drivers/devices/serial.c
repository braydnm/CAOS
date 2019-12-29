#include "serial.h"

void serial_enable(int device){
    outportb(device + 1, 0x00);
    outportb(device + 3, 0x80); /* Enable divisor mode */
    outportb(device + 0, 0x03); /* Div Low:  03 Set the port to 38400 bps */
    outportb(device + 1, 0x00); /* Div High: 00 */
    outportb(device + 3, 0x03);
    outportb(device + 2, 0xC7);
    outportb(device + 4, 0x0B);
}

void serial_install(){
    serial_enable(SERIAL_PORT_A);
    serial_enable(SERIAL_PORT_B);
}

int serial_rcvd(int device){
    return inportb(device+5)&1;
}

char serial_read_byte(int device){
    while(!serial_rcvd(device));
    return inportb(device);
}

char serial_read_byte_async(int device){
    return inportb(device);
}

int serial_transmit_empty(int device) {
    return inportb(device + 5) & 0x20;
}

void serial_send_char(int device, char c){
    while (!serial_transmit_empty(device));
    outportb(device, c);
}

void serial_send_data(int device, char* data){
    for (uint32_t i = 0; i<strlen(data); i++)
        serial_send_char(device, data[i]);
}
