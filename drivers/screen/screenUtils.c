//
// Created by x3vikan on 3/18/18.
//
#include "screenUtils.h"

int getLocation(int row, int col) { return 2 * (row * MAX_COLS + col); }
int getLocationRow(int location) { return location / (2 * MAX_COLS); }
int getLocationCol(int location) { return (location - (getLocationRow(location)*2*MAX_COLS))/2; }

// private screen functions
int messageLength(char* message){
    int pos = 0;
    int count = 0;
    while (message[pos]!=0)
        count+=(message[pos++]=='\n'?0:1);
    return count;
}

int getCursorLocation() {
    outportb(SCREEN_CTRL, 14);
    int location = inportb(SCREEN_DATA) << 8;
    outportb(SCREEN_CTRL, 15);
    location += inportb(SCREEN_DATA);
    return location * 2;
}

void setCursorLocation(int location) {
    location /= 2;
    outportb(SCREEN_CTRL, 14);
    outportb(SCREEN_DATA, (unsigned char) (location >> 8));
    outportb(SCREEN_CTRL, 15);
    outportb(SCREEN_DATA, (unsigned char) (location & 0xff));
}

