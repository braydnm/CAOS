//
// Created by x3vikan on 2/18/18.
//

#ifndef OS_KEYBOARD_H
#define OS_KEYBOARD_H

#include "../../kernel/kernel.h"

#define keyReleased(x) (x&0x80)

void initKeyboard();
void handleSpecialPress(uint8_t scancode);

#endif //OS_KEYBOARD_H
