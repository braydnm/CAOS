#ifndef SCREEN_UTILS_H
#define SCREEN_UTILS_H
#include "../../kernel/kernel.h"

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

/* Screen i/o ports */
#define SCREEN_CTRL 0x3d4
#define SCREEN_DATA 0x3d5

int getLocation(int row, int col);
int getLocationRow(int location);
int getLocationCol(int location);

// private screen functions
int messageLength(char* message);

int getCursorLocation();

void setCursorLocation(int location);

#endif
