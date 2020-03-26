#ifndef SCREEN_H
#define SCREEN_H

#include "../../kernel/kernel.h"

extern int colors;

extern unsigned char* screenBuffer;
void clearScreen();
void kprintPos(char* message, int row, int col, int centered);
void kprint(char* message);
void kprintCentered(char* message);
int _kprintf(int centered, char* message, args_list list);
int kprintfCentered(char* message, ...);
int kprintf(char* message, ...);
void backspace();
void setColor(int newColor);
void setBackground(int background);
void setText(int text);

#endif